"""
This is the base class for the synchronous and asynchronous command classes.
Commands can be executed either locally or remotely (via ssh) and can run with
sudo privilege.  The reason to use this facility instead of one of the existing
subprocess packages is for proper handling of the ssh and sudo password
challenges.

If a ssh and/or sudo password challenge is expected then a pty is allocated and
the slave side is configured to disable echo and CRLF mapping.  Password
challenges, if needed, occur as follows:

    - Ssh password challenges always occur on the pty master.

    - Sudo password challenges when not ssh-ing also occur on the pty master.

    - If ssh-ing then sudo password challenges occur on stdin and stdout; this
      is a limitation of sudo over ssh.

The ssh and sudo passwords are assumed to be the same.  Note that the password
challenge scenario must be explictly specified.  For example, if a sudo password
challenge is expected, it must occur, and not be circumvented based on settings
in the sudoers file.

The existing python subprocess packages do most of the work; however, they do
not support allocating a PTY and setting the slave side as the controlling
terminal for the child command process.  This is accomplished in this code by
allocating a PTY and using the "preexec_fn" argument to make the proper fcntl()
call to set the slave side as the controlling terminal.

The base class is responsible for constructing the command, allocating the PTY,
password management, and defined the base FSM for executing the command.  The
derived classes are responsible for providing an event loop and overriding the
base FSM action methods as necessary.

It is important to note that although the derived classes call the close()
method at command completion, either the context manager form should be used or
an explicit call to close() should occur to help avoid file descriptor leaks.
"""

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
import fcntl
import re
from termios import TIOCSCTTY
from typing import Any, ClassVar, Iterable, Optional

from arguments.integer import Integer
from arguments.string import String
from events.pty_manager import PTYManager
from events.io_endpoint import IOEndpoint
from events.fsm import FiniteStateMachine, State

class CommandBase(FiniteStateMachine):
    """ Command Execution Base Class """
    # pylint: disable=too-many-instance-attributes
    # pylint: disable=too-many-public-methods
    DEFAULT_SSH_PROMPT : ClassVar[str] = r'[Pp]assword:'
    DEFAULT_SUDO_PROMPT : ClassVar[str] = r'\[sudo\] password for [\w\-]+:'
    DEFAULT_TIMEOUT : ClassVar[float] = 10.0  # seconds

    # STATES
    #
    #   START:
    #       Initial state that selects either one of the password states or the
    #       running state.
    #
    #   SSH_MASTER:
    #       Performing an ssh password authentication on the PTY master.
    #
    #   SUDO_MASTER:
    #       Performing a sudo password authentication on the PTY master.
    #
    #   SUDO_STDIO:
    #       Performing a sudo password authentication on stdin/stdout.
    #
    #   RUNNING:
    #       Command is executing and collecting data from stdout/stderr.
    #
    #   EXITED:
    #       The command child process has exited; however, there may still be
    #       data on stdout/stderr to read.
    #
    #   TERM:
    #       Password or command execution timeout or a stop event has occurred.
    #       A SIGTERM is sent to the child command process and waiting for the
    #       child command process to exit.
    #
    #   KILL:
    #       The child command process did not terminate.  A SIGKILL is sent to
    #       the child command process and waiting for the child command process
    #       to exit.
    #
    #   DONE:
    #       Either the child command process has successfully terminated or all
    #       attempts to terminate it have failed.
    #
    Q_START : ClassVar[int] = 0
    Q_SSH_MASTER : ClassVar[int] = 1
    Q_SUDO_MASTER : ClassVar[int] = 2
    Q_SUDO_STDIO : ClassVar[int] = 3
    Q_RUNNING : ClassVar[int] = 4
    Q_EXITED : ClassVar[int] = 5
    Q_TERM : ClassVar[int] = 6
    Q_KILL : ClassVar[int] = 7
    Q_DONE : ClassVar[int] = 8

    # EVENTS
    #
    #   STDOUT_READ:
    #       The child process's stdout is readable.  This is only used when
    #       doing sudo password challenges over stdio.
    #
    #   MASTER_READ:
    #       The PTY master side is readable.  This is only used during ssh or
    #       sudo password challenges over the master.
    #
    #   MASTER_WRITE:
    #       The PTY master side is writable.  This is only used during ssh or
    #       sudo password challenges over the master.
    #
    #   CLOSED:
    #       An indication that stdin, stdout, or stderr has closed.
    #
    #   EXITED:
    #       The child command process has exited.
    #
    #   TIMEOUT:
    #       Authentication or command timeout has occurred.
    #
    #   STOP:
    #       A request to cancel the command and force termination.
    #
    E_STDOUT_READ : ClassVar[int] = 0
    E_MASTER_READ : ClassVar[int] = 1
    E_MASTER_WRITE : ClassVar[int] = 2
    E_CLOSED : ClassVar[int] = 3
    E_EXIT : ClassVar[int] = 4
    E_TIMEOUT : ClassVar[int] = 5
    E_STOP : ClassVar[int] = 6

    command : list[str]

    remote : bool
    privileged : bool
    use_ssh_password : bool
    use_sudo_password : bool
    ssh_prompt : re.Pattern
    sudo_prompt : re.Pattern
    password : bytes
    password_timeout : float
    command_timeout : float
    term_timeout : float
    kill_timeout : float
    redirect_stderr : bool

    pty : PTYManager
    master : IOEndpoint
    stdin : IOEndpoint
    stdout : IOEndpoint
    stderr : IOEndpoint

    status : int
    reason : str

    def __init__(
        # pylint: disable=too-many-arguments
        # pylint: disable=too-many-locals
        self,
        args : list[str],
        *,
        remote_host : Optional[str] = None,
        remote_user : Optional[str] = None,
        privileged : Optional[bool] = False,
        use_ssh_password : Optional[bool] = False,
        use_sudo_password : Optional[bool] = False,
        ssh_prompt : Optional[str] = DEFAULT_SSH_PROMPT,
        sudo_prompt : Optional[str] = DEFAULT_SUDO_PROMPT,
        password: Optional[str] = None,
        password_timeout : Optional[float] = DEFAULT_TIMEOUT,
        command_timeout : Optional[float] = DEFAULT_TIMEOUT,
        term_timeout : Optional[float] = DEFAULT_TIMEOUT,
        kill_timeout : Optional[float] = DEFAULT_TIMEOUT,
        redirect_stderr : Optional[bool] = False
    ):
        """
        Setup command execution

        Arguments:
            args:
                List of command line tokens for the command to execute.  By
                convention, args[0] is the command invocation token.
            remote_host:
                If specified then the command is executed remotely (via ssh) on
                the specified host.
            remote_user:
                Alternate user to use for remote execution.
            privileged:
                If True then the command is executed with privilege (via sudo).
            use_ssh_password:
                If True then an ssh password challenge is expected.  If False
                then ssh key authentication is used.  Ignored if not executing
                remotely.
            use_sudo_password:
                If True then a sudo password challenge is expected.  If False
                then it is assumed that the sudo settings make a password
                challenge unnecessary.  Ignored if not executing with privilege.
            ssh_prompt:
                A regex for matching the expected ssh password prompt, if using
                ssh username/password authentication.
            sudo_prompt:
                A regex for matching the expected sudo password prompt, if a
                sudo password prompt is expected.
            password:
                Password to use for for ssh and sudo password challenges.  The
                trailing newline is automatically added.  Note that if not
                specified then an expected password challenge is answered with a
                single newline, which will fail the challenge.
            password_timeout:
                Timeout (in seconds) for receipt of an expected ssh or sudo
                password challenge.  If timeout occurs then the child command
                process is killed with a SIGTERM.
            command_timeout:
                Timeout (in seconds) for command completion and exit.  If
                timeout occurs then the child command process is killed with a
                SIGTERM.
            term_timeout:
                Timeout (in seconds) for command child process exit after a
                SIGTERM.  If timeout occurs then the child command process is
                killed with a SIGKILL.
            kill_timeout:
                Timeout (in seconds) for command child process exit after a
                SIGKILL.  If timeout occurs then it is assumed that the child
                command process would not exit.
            redirect_stderr:
                Redirect stderr to stdout.
        """
        super().__init__(self.setup_fsm())

        self.remote = bool(remote_host)
        self.privileged = bool(privileged)
        self.use_ssh_password = self.remote and use_ssh_password
        self.use_sudo_password = self.privileged and use_sudo_password

        self.make_command(
            args,
            remote_host=remote_host or None,
            remote_user=remote_user or None,
        )

        if self.use_ssh_password:
            self.ssh_prompt = re.compile(ssh_prompt) if ssh_prompt else ''
        else:
            self.ssh_prompt = None

        if self.use_sudo_password:
            self.sudo_prompt = re.compile(sudo_prompt) if sudo_prompt else ''
        else:
            self.sudo_prompt = None

        if self.use_ssh_password or self.use_sudo_password:
            password = password.strip() + "\n" if password else "\n"
            self.password = password.encode()
        else:
            self.password = None

        self.password_timeout = password_timeout
        self.command_timeout = command_timeout
        self.term_timeout = term_timeout
        self.kill_timeout = kill_timeout
        self.redirect_stderr = bool(redirect_stderr)

        if (
            self.use_ssh_password or
            (self.use_sudo_password and not self.remote)
        ):
            self.allocate_pty()
            self.setup_master()
        else:
            self.pty = None
            self.master = None

        self.stdin = None
        self.stdout = None
        self.stderr = None

        self.status = None
        self.reason = None

    def make_command(
        # pylint: disable=too-many-arguments
        self,
        args : list[str],
        *,
        remote_host : Optional[str] = None,
        remote_user : Optional[str] = None,
    ) -> None:
        """
        Generate the command line

        Arguments:
            args:
                List of command line tokens for the command to execute.  By
                convention, args[0] is the command invocation token.
            remote_host:
                If specified then the command is executed remotely (via ssh) on
                the specified host.
            remote_user:
                Alternate user to use for remote execution.
        """
        self.command = []

        if self.privileged:
            args = ['sudo', *args]

        if remote_host:
            # The "-t -t" is needed to force pty allocation and to ensure that
            # all signals are propagated to the child process.
            self.command.extend(['ssh', '-q', '-t', '-t'])

            # Choose password or public key authentication.
            if self.use_ssh_password:
                self.command.extend(
                    [
                        '-o', 'PasswordAuthentication=yes',
                        '-o', 'PubkeyAuthentication=no',
                        '-o', 'NumberOfPasswordPrompts=1'
                    ]
                )
            else:
                self.command.extend(
                    [
                        '-o', 'PubkeyAuthentication=yes',
                        '-o', 'PasswordAuthentication=no',
                        '-o', 'StrictHostKeyChecking=no'
                    ]
                )

            remote_user = remote_user or None

            if remote_user:
                self.command.append(f"{remote_user}@{remote_host}")
            else:
                self.command.append(remote_host)

            # For ssh, the arguments are in a string.
            self.command.append(' '.join(args))
        else:
            # For non-ssh, the arguments are in a list.
            self.command.extend(args)

    def allocate_pty(self) -> None:
        """ Allocate and configure a PTY """
        self.pty = PTYManager()
        self.pty.disable_echo_crlf()
        self.pty.set_nonblocking()

    def set_terminal(self) -> None:
        """ Set the controlling terminal to the PTY slave """
        fcntl.ioctl(self.pty.slave, TIOCSCTTY, 0)

    def setup_master(self) -> None:
        """ Set up the PTY master for password challenges """
        self.master = None

    def start_timer(self, timeout : float) -> None:
        """
        [Re]start a timer

        Arguments:
            Timeout, in seconds.
        """

    def cancel_timer(self) -> None:
        """ Cancel any started timer """

    def setup_fsm(self) -> list[State]:
        """ Create an instance of the command FSM """
        return [
            # Q_START
            State(
                self.initial_state, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_CLOSED
                    None,  # E_EXIT
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            ),

            # Q_SSH_MASTER
            State(
                self.start_auth_timer, self.stop_timer,
                [
                    None,               # E_STDOUT_READ
                    self.ssh_password,  # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.ssh_timeout,   # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_SUDO_MASTER
            State(
                self.start_auth_timer, self.stop_timer,
                [
                    None,               # E_STDOUT_READ
                    self.sudo_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.sudo_timeout,  # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_SUDO_STDIO
            State(
                self.start_sudo_stdio, self.stop_sudo_stdio,
                [
                    self.sudo_stdio,    # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.sudo_timeout,  # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_RUNNING
            State(
                self.wait_for_exit, None,
                [
                    None,               # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.run_timeout,   # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_EXITED
            State(
                None, self.stop_timer,
                [
                    None,               # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.check_done,    # E_CLOSED
                    None,               # E_EXIT
                    self.no_close,      # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_TERM
            State(
                self.term_child, self.stop_timer,
                [
                    None,               # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.term_failed,   # E_TIMEOUT
                    None                # E_STOP
                ]
            ),

            # Q_KILL
            State(
                self.kill_child, self.stop_timer,
                [
                    None,               # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_CLOSED
                    self.check_done,    # E_EXIT
                    self.kill_failed,   # E_TIMEOUT
                    None                # E_STOP
                ]
            ),

            # Q_DONE
            State(
                self.close_all, None,
                [
                    None,  # E_STDIN_WRITE
                    None,  # E_STDOUT_READ
                    None,  # E_STDERR_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_SIGCHLD
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            )
        ]

    def initial_state(self, _state : int, _event : int, _data : Any) -> int:
        """ Determine the initial state """
        if self.use_ssh_password:
            q_next = self.Q_SSH_MASTER
        elif self.use_sudo_password:
            q_next = self.Q_SUDO_STDIO if self.remote else self.Q_SUDO_MASTER
        else:
            q_next = self.Q_RUNNING

        return q_next

    def start_auth_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ Start the authentication timer """
        self.start_timer(self.password_timeout)

    def stop_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ Stop any running timer """
        self.cancel_timer()

    def ssh_password(self, state : int, event : int, data : Any) -> int:
        """ Attempt SSH authentication """
        q_next = self.read_master(state, event, data)
        if q_next is not None:
            return q_next

        text = self.master.fetch_input(text=True)
        if not self.ssh_prompt.search(text):
            return None

        self.master.write(self.password)

        if self.master.errno:
            self.status = self.status or self.master.errno
            self.reason = self.reason or self.master.error_text
            return self.Q_TERM

        if self.use_sudo_password:
            return self.Q_SUDO_STDIO

        return self.Q_RUNNING

    def read_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Read new data from master """
        self.master.read()

        if self.master.errno:
            self.status = self.status or self.master.errno
            self.reason = self.reason or self.master.error_text
            return self.Q_TERM

        return None

    def write_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Write outstanding data to master """
        self.master.write()

        if self.master.errno:
            self.status = self.status or self.master.errno
            self.reason = self.reason or self.master.error_text
            return self.Q_TERM

        return None

    def ssh_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to ssh authentication timeout """
        self.cancel_timer()

        self.reason = self.reason or 'SSH authentication timeout'

        return self.Q_TERM

    def sudo_master(self, state : int, event : int, data : Any) -> int:
        """ Attempt SUDO authentication (on master) """
        q_next = self.read_master(state, event, data)
        if q_next is not None:
            return q_next

        text = self.master.fetch_input(text=True)
        if not self.sudo_prompt.search(text):
            return None

        self.master.write(self.password)

        if self.master.errno:
            self.status = self.status or self.master.errno
            self.reason = self.reason or self.master.error_text
            return self.Q_TERM

        return self.Q_RUNNING

    def sudo_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to sudo authentication timeout """
        self.cancel_timer()

        self.reason = self.reason or 'SUDO authentication timeout'

        return self.Q_TERM

    def start_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Start SUDO authentication on stdio """
        return self.start_auth_timer(state, event, data)

    def stop_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Stop SUDO authentication on stdio """
        return self.stop_timer(state, event, data)

    def sudo_stdio(self, _state : int, _event : int, _data : Any) -> int:
        """ Attempt SUDO authentication (on stdio, derived override) """
        return None

    def wait_for_exit(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Start waiting for child command process exit """
        self.start_timer(self.command_timeout)

    def check_done(self, _state : int, _event : int, _data : Any) -> int:
        """ Check for all stdio closed (derived override) """
        return None

    def stop_command(self, state : int, _event : int, _data : Any) -> int:
        """ Start termination sequence action method """
        self.reason = self.reason or 'Command canceled'

        if state == self.Q_EXITED:
            return self.Q_DONE

        return self.Q_TERM

    def run_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to command timeout action method """
        self.cancel_timer()

        self.reason = self.reason or 'Command timeout'

        return self.Q_TERM

    def no_close(self, _state : int, _event : int, _data : Any) -> int:
        """ Stdio did not close as expected """
        self.cancel_timer()

        self.reason = self.reason or 'Warning: stdio did not close as expected'

        return self.Q_DONE

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child command process termination (derived override) """

    def term_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Termination failed so kill child """
        return self.Q_KILL

    def kill_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Kill child command process (derived override) """

    def kill_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Child command process would not exit """
        self.status = self.status or 1
        self.reason = self.reason or 'Command did not exit'

        return self.Q_DONE

    def close_all(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Close all resources """
        self.close()

    def close(self) -> None:
        """ Release all resources """
        self.cancel_timer()

        if self.master:
            self.master.close()

        if self.pty:
            self.pty.close()

        if self.stdin:
            self.stdin.close()

        if self.stdout:
            self.stdout.close()

        if self.stderr:
            self.stderr.close()

    def __enter__(self):
        """ Return self """
        return self

    def __exit__(self, *args, **kwargs):
        """ Close all resources """
        self.close()

    @classmethod
    def add_arguments(
        cls, parser : ArgumentParser, exclude : Optional[Iterable[str]] = None
    ) -> None:
        # pylint: disable=too-many-branches
        """
        Add command line arguments to parser

        Arguments:
            parser:
                Argument parser to receive argument definitions.
            exclude:
                List of argument names to exclude.
        """
        exclude = exclude or []

        if 'remote_host' not in exclude:
            parser.add_argument(
                '-r', '--remote_host',
                type=String(),
                help='host on which to execute via ssh'
            )
        if 'remote_user' not in exclude:
            parser.add_argument(
                '-u', '--remote_user',
                type=String(),
                help='username for remote authentication'
            )
        if 'privileged' not in exclude:
            parser.add_argument(
                '-S', '--privileged',
                action='store_true',
                help='run commnd with sudo privilege'
            )
        if 'use_ssh_password' not in exclude:
            parser.add_argument(
                '-sp', '--use_ssh_password',
                action='store_true',
                help='use ssh password instead of ssh key authentication'
            )
        if 'use_ssh_password' not in exclude:
            parser.add_argument(
                '-Sp', '--use_sudo_password',
                action='store_true',
                help='use sudo password authentication'
            )
        if 'ssh_prompt' not in exclude:
            parser.add_argument(
                '-sP', '--ssh_prompt',
                type=String(),
                default=cls.DEFAULT_SSH_PROMPT,
                help='expected ssh password prompt regex'
            )
        if 'sudo_prompt' not in exclude:
            parser.add_argument(
                '-SP', '--sudo_prompt',
                type=String(),
                default=cls.DEFAULT_SUDO_PROMPT,
                help='expected sudo password prompt regex'
            )
        if 'password' not in exclude:
            parser.add_argument(
                '-p', '--password',
                type=String(),
                help='password for remote/sudo authentication'
            )
        if 'password_timeout' not in exclude:
            parser.add_argument(
                '-tP', '--password_timeout',
                type=Integer(min_value=0),
                metavar='seconds',
                default=cls.DEFAULT_TIMEOUT,
                help='Timeout for password challenge'
            )
        if 'command_timeout' not in exclude:
            parser.add_argument(
                '-t', '--command_timeout',
                type=Integer(min_value=0),
                metavar='seconds',
                default=cls.DEFAULT_TIMEOUT,
                help='Timeout for child process exit'
            )
        if 'term_timeout' not in exclude:
            parser.add_argument(
                '-tT', '--term_timeout',
                type=Integer(min_value=0),
                metavar='seconds',
                default=cls.DEFAULT_TIMEOUT,
                help='Timeout for child process exit after SIGTERM'
            )
        if 'kill_timeout' not in exclude:
            parser.add_argument(
                '-tK', '--kill_timeout',
                type=Integer(min_value=0),
                metavar='seconds',
                default=cls.DEFAULT_TIMEOUT,
                help='Timeout for child process exit after SIGKILL'
            )
        if 'redirect_stderr' not in exclude:
            parser.add_argument(
                '-R', '--redirect_stderr',
                action='store_true',
                help='redirect stderr to stdout'
            )
        if 'args' not in exclude:
            parser.add_argument(
                'arg', nargs='+', help='command line arguments'
            )

    @classmethod
    def create_command(cls) -> 'CommandBase':
        """ Create a command instance from the command line """
        parser = ArgumentParser(
            description='Execute Command',
            formatter_class=ArgumentDefaultsHelpFormatter
        )
        cls.add_arguments(parser)
        values = parser.parse_args()

        return cls(
            values.arg,
            remote_host=values.remote_host,
            remote_user=values.remote_user,
            privileged=values.privileged,
            use_ssh_password=values.use_ssh_password,
            use_sudo_password=values.use_sudo_password,
            ssh_prompt=values.ssh_prompt,
            sudo_prompt=values.sudo_prompt,
            password=values.password,
            password_timeout=values.password_timeout,
            command_timeout=values.command_timeout,
            term_timeout=values.term_timeout,
            kill_timeout=values.kill_timeout,
            redirect_stderr=values.redirect_stderr
        )
