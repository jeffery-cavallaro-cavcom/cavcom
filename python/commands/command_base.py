"""
This is the base class for the synchronous and asynchronous command classes.
Commands can be executed either locally or remotely (via ssh) and can run with
sudo privilege.  The reason to use this facility instead of one of the existing
subprocess packages is for proper handling of the ssh and sudo password
challenges.

A pty is always allocated and the slave side is configured to disable echo and
CRLF mapping.  Password challenges, if needed, occur as follows:

    - For ssh or sudo (but not both), the password challenge occurs on the
      PTY master.

    - For ssh and sudo, the ssh password challenge occurs on the PTY master;
      however, the sudo password challenge occurs on stdin and stdout; this is a
      limitation of sudo over ssh.

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
and password management.  The derived classes are responsible for actually
executing the command.

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
from events.io_endpoint import IOEndpoint
from events.pty_manager import PTYManager

class CommandBase:
    """ Command Execution Base Class """
    # pylint: disable=too-many-instance-attributes
    # pylint: disable=too-many-public-methods
    DEFAULT_SSH_PROMPT : ClassVar[str] = r'[Pp]assword:'
    DEFAULT_SUDO_PROMPT : ClassVar[str] = r'\[sudo\] password for [\w\-]+:'
    DEFAULT_TIMEOUT : ClassVar[float] = 10.0  # seconds

    # STATES
    #
    # These are the states for the derived class FSMs that execute the command.
    # The events are defined by the derived classes, since they tend to differ
    # depending on the underlying python packages and event loop types.
    #
    #   START:
    #       Initial state that selects either one of the password states or the
    #       running state.
    #
    #   SSH_MASTER:
    #       Performing an ssh password authentication on the PTY master,
    #       watching for authentication timeout.
    #
    #   SUDO_MASTER:
    #       Performing a sudo password authentication on the PTY master, since
    #       no ssh, watching for authentication timeout.
    #
    #   SUDO_STDIO:
    #       Performing a sudo password authentication on stdin/stdout, due to
    #       ssh, watching for authentication timeout.
    #
    #   RUNNING:
    #       Command is executing and collecting data from stdout/stderr,
    #       waiting for child command process exit.
    #
    #   EXITED:
    #       The command child process has exited.  Keep reading until all of
    #       stdio closes.
    #
    #   TERM:
    #       Password or command execution timeout or a stop signal (SIGINT or
    #       SIGTERM) has occurred.  A SIGTERM is sent to the child command
    #       process and waiting for the child command process to exit.
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

    command : list[str]

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

    status : int
    reason : str

    def __init__(
        # pylint: disable=too-many-arguments
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
        self.use_ssh_password = remote_host and use_ssh_password
        self.use_sudo_password = privileged and use_sudo_password

        self.make_command(
            args,
            remote_host=remote_host or None,
            remote_user=remote_user or None,
            privileged=bool(privileged),
            use_ssh_password=self.use_ssh_password
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

        self.pty = None
        self.master = None

        self.status = None
        self.reason = None

    def make_command(
        # pylint: disable=too-many-arguments
        self,
        args : list[str],
        *,
        remote_host : Optional[str] = None,
        remote_user : Optional[str] = None,
        privileged : Optional[bool] = False,
        use_ssh_password : Optional[bool] = False,
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
            privileged:
                If True then the command is executed with privilege (via sudo).
            use_ssh_password:
                If True then use ssh username/password authentication.  If False
                then use ssh key authentication.  Ignored if not executing
                remotely.
        """
        self.command = []

        if privileged:
            args = ['sudo', *args]

        if remote_host:
            # The "-t -t" is needed to force pty allocation and to ensure that
            # all signals are propagated to the child process.
            self.command.extend(['ssh', '-t', '-t'])

            # Choose password or public key authentication.
            if use_ssh_password:
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
        self.pty.set_nonblocking()
        self.pty.disable_echo_crlf()

    def set_terminal(self) -> None:
        """ Set the controlling terminal to the PTY slave """
        fcntl.ioctl(self.pty.slave, TIOCSCTTY, 0)

    def start_timer(self, timeout : float) -> None:
        """
        [Re]start a timer

        Arguments:
            Timeout, in seconds.
        """

    def cancel_timer(self) -> None:
        """ Cancel any started timer """

    def match_ssh_prompt(self, prompt : bytes) -> bool:
        """
        Check for a matching ssh password prompt

        Returns:
            True if the specified raw input matches the expected prompt.
        """
        if not bytes:
            return None

        try:
            text = prompt.decode()
        except:  # pylint: disable=bare-except
            # Unicode sequence may be incomplete.
            return None

        return bool(self.ssh_prompt.search(text))

    def match_sudo_prompt(self, prompt : bytes) -> int:
        """
        Check for a matching sudo password prompt

        Returns:
            True if the specified raw input matches the expected prompt.
        """
        if not bytes:
            return None

        try:
            text = prompt.decode()
        except:  # pylint: disable=bare-except
            # Unicode sequence may be incomplete.
            return None

        return bool(self.sudo_prompt.search(text))

    def initial_state(self, _state : int, _event : int, _data : Any) -> int:
        """ Q_START begin method to determine the initial state """
        if self.use_ssh_password:
            q_next = self.Q_SSH_MASTER
        elif self.use_sudo_password:
            q_next = self.Q_SUDO_MASTER
        else:
            q_next = self.Q_RUNNING

        return q_next

    def start_auth_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ Password state begin method to start the authentication timer """
        self.start_timer(self.password_timeout)

    def start_command_timer(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Q_RUNNING state begin method to start the command timer """
        self.start_timer(self.command_timeout)

    def stop_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ End method to stop any running timer """
        self.cancel_timer()

    def read_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Read master action method """
        self.master.read()

        if self.master.read_errno:
            self.status = self.master.read_errno
            self.reason = self.master.read_error_text
            return self.Q_TERM

        return None

    def write_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Write outstanding data to PTY master action method """
        self.master.write()

        if self.master.write_errno:
            self.status = self.master.write_errno
            self.reason = self.master.write_error_text
            return self.Q_TERM

        return self.Q_RUNNING

    def ssh_password(self, state : int, event : int, data : Any) -> int:
        """ SSH authentication action method """
        q_next = self.read_master(state, event, data)
        if q_next is not None:
            return q_next

        if not self.match_ssh_prompt(self.master.input_data):
            return None

        self.master.write(self.password)

        if self.master.write_errno:
            self.status = self.master.write_errno
            self.reason = self.master.write_error_text
            return self.Q_TERM

        if self.use_sudo_password:
            return self.Q_SUDO_STDIO

        return self.Q_RUNNING

    def sudo_master(self, state : int, event : int, data : Any) -> int:
        """ SUDO authentication action method (on master) """
        q_next = self.read_master(state, event, data)
        if q_next is not None:
            return q_next

        if not self.match_sudo_prompt(self.master.input_data):
            return None

        self.master.write(self.password)

        if self.master.write_errno:
            self.status = self.master.write_errno
            self.reason = self.master.write_error_text
            return self.Q_TERM

        return self.Q_RUNNING

    def ssh_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to ssh timeout action method """
        self.cancel_timer()

        if not self.reason:
            self.reason = 'SSH authentication timeout'

        return self.Q_TERM

    def sudo_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to sudo timeout action method """
        self.cancel_timer()

        if not self.reason:
            self.reason = 'SUDO authentication timeout'

        return self.Q_TERM

    def run_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start termination due to command timeout action method """
        self.cancel_timer()

        if not self.reason:
            self.reason = 'Command timeout'

        return self.Q_TERM

    def no_close(self, _state : int, _event : int, _data : Any) -> int:
        """ Indicate stdio did not close action method """
        self.cancel_timer()

        if not self.reason:
            self.reason = 'Warning: stdio did not close as expected'

        return self.Q_DONE

    def stop_command(self, state : int, _event : int, _data : Any) -> int:
        """ Start termination sequence action method """
        if not self.reason:
            self.reason = 'Command canceled'

        if state == self.Q_EXITED:
            return self.Q_DONE

        return self.Q_TERM

    def term_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Move on to kill child action method """
        return self.Q_KILL

    def kill_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Child command process would not terminate action method """
        if not self.status:
            self.status = 1

        if not self.reason:
            self.reason = 'Command did not terminate'

        return self.Q_DONE

    def close_all(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Q_DONE begin method to close all resources """
        self.close()

    def close(self) -> None:
        """ Release all resources """
        self.cancel_timer()

        if self.master:
            self.master.close()
            self.master = None

        if self.pty:
            self.pty.close()
            self.pty = None

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
