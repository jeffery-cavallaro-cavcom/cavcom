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

Note that the ssh and sudo passwords are assumed to be the same.

Upon a fork, the pty slave is established as the child's controlling terminal
and standard I/O communication between the parent and child is accomplished
using pipes.  Note that standard error can be redirected to stdout.

This base class is responsible for the following:

    - Construct the command to execute from the constructor arguments.
    - Set up the PTY and stdio pipes.
    - Fork and perform the child exec.
    - Execute events from the event loop in the parent.
    - Handle password challenges in the parent.

Derived classes provide methods to do the follow:

    - Set up the stdio communication endpoints per the target event loop.
    - Set up the (pty) master endpoint per the target event loop.
    - Set up the SIGCHLD signal handler.
    - Set up a timer handler (for timeouts).
    - Call the command FSM run() method once with no event in order to establish
      the initial state.
    - Obtain the next event from the target event loop and call the command FSM
      run() method until the done state is achieved.

Note that the password challenge scenario must be explictly specified.  For
example, if a sudo password challenge is expected, it must occur, and not be
circumvented based on settings in the sudoers file.

It is also important to note that although the close() method is called at
command completion, either the context manager form should be used or an
explicit call to close() should occur to help avoid file descriptor leaks.
"""

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter
import fcntl
import os
import re
from signal import SIGTERM, SIGKILL
import sys
import termios
from typing import Any, ClassVar, Iterable, Optional

from arguments.integer import Integer
from arguments.string import String
from events.fsm import FiniteStateMachine, State
from events.io_endpoint import IOEndpoint
from events.pty_manager import PTYManager

class CommandBase(FiniteStateMachine):
    """ Command Execution Base Class """
    # pylint: disable=too-many-instance-attributes
    # pylint: disable=too-many-public-methods
    DEFAULT_SSH_PROMPT : ClassVar[str] = r'[Pp]assword:'
    DEFAULT_SUDO_PROMPT : ClassVar[str] = r'\[sudo\] password for [\w\-]+:'
    DEFAULT_TIMEOUT : ClassVar[int] = 10  # seconds

    # STATES
    # ------
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
    #       Command is executing and collecting data from stdout/stderr.
    #       Waiting for a child signal.
    #
    #   EXITED:
    #       The command child process has exited.  Keep reading until
    #       stdout closes.  Note that stderr does not appear to close.
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

    # EVENTS
    E_STDIN_WRITE : ClassVar[int] = 0
    E_STDOUT_READ : ClassVar[int] = 1
    E_STDERR_READ : ClassVar[int] = 2
    E_MASTER_READ : ClassVar[int] = 3
    E_MASTER_WRITE : ClassVar[int] = 4
    E_SIGCHLD : ClassVar[int] = 5
    E_TIMEOUT : ClassVar[int] = 6
    E_STOP : ClassVar[int] = 7

    command : list[str]

    use_ssh_password : bool
    use_sudo_password : bool
    ssh_prompt : re.Pattern
    sudo_prompt : re.Pattern
    password : bytes
    password_timeout : int
    command_timeout : int
    term_timeout : int
    kill_timeout : int
    redirect_stderr : bool

    stdin : IOEndpoint
    stdout : IOEndpoint
    stderr : IOEndpoint
    master : IOEndpoint

    child_pid : int
    slave : int
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
        password_timeout : Optional[str] = DEFAULT_TIMEOUT,
        command_timeout : Optional[int] = DEFAULT_TIMEOUT,
        term_timeout : Optional[int] = DEFAULT_TIMEOUT,
        kill_timeout : Optional[int] = DEFAULT_TIMEOUT,
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

        self.stdin = None
        self.stdout = None
        self.stderr = None
        self.master = None
        self.child_pid = None
        self.slave = None

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

    def setup_endpoints(self) -> None:
        """ Set up stdin, stdout, and stderr endpoints """

    def setup_master(self, _master : int) -> None:
        """
        Set up the master endpoint

        Arguments:
            master:
                Master side of pty.
        """

    def enable_sigchld(self) -> None:
        """ Enable SIGCHLD event """

    def disable_sigchld(self) -> None:
        """ Disable SIGCHLD event """

    def start_timer(self, _timeout : float) -> None:
        """
        Start a timer for E_TIMEOUT events

        Arguments:
            timeout:
                Timer expiration duration (in seconds).
        """

    def cancel_timer(self) -> None:
        """ Cancel timer E_TIMEOUT events """

    def fork_and_exec(self) -> None:
        """ Setup parent and fork child """
        self.setup_endpoints()

        # The slave side is kept open in the master; closing it seems to cause
        # problems in the child.
        pty = PTYManager()
        pty.set_nonblocking()
        pty.disable_echo_crlf()

        master = pty.master
        self.slave = pty.slave

        # Do this before the fork so we do not lose the SIGCHLD.
        self.enable_sigchld()

        self.child_pid = os.fork()

        if not self.child_pid:
            # In the child.
            os.close(master)
            master = None
            self.exec_child()

        # In the parent.
        self.stdin.write_only()

        self.stdout.read_only()
        self.stdout.register_read()

        if self.stderr:
            self.stdout.read_only()
            self.stderr.register_read()

        self.setup_master(master)
        self.master.register_read()

    def exec_child(self) -> None:
        """ Exec child command """
        # Create a new session and attach the slave side of the pty as the
        # controlling terminal for the new session.
        os.setsid()
        fcntl.ioctl(self.slave, termios.TIOCSCTTY, 0)

        # Redirect all stdio.
        self.stdin.read_only()
        os.dup2(self.stdin.reader, sys.stdin.fileno())

        self.stdout.write_only()
        os.dup2(self.stdout.writer, sys.stdout.fileno())

        if self.stderr:
            self.stderr.write_only()
            os.dup2(self.stderr.writer, sys.stderr.fileno())
        else:
            os.dup2(self.stdout.writer, sys.stderr.fileno())

        # Execute the command.
        os.execvp(self.command[0], self.command)

    def setup_fsm(self) -> list[State]:
        """ Create an instance of the command FSM """
        return [
            # Q_START
            State(
                self.initial_state, None,
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
            ),

            # Q_SSH_MASTER
            State(
                self.start_auth_timer, self.stop_timer,
                [
                    self.write_stdin,   # E_STDIN_WRITE
                    self.read_stdout,   # E_STDOUT_READ
                    self.read_stderr,   # E_STDERR_READ
                    self.ssh_password,  # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.reap_child,    # E_SIGCHLD
                    self.ssh_timeout,   # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_SUDO_MASTER
            State(
                self.start_auth_timer, self.stop_timer,
                [
                    self.write_stdin,    # E_STDIN_WRITE
                    self.read_stdout,    # E_STDOUT_READ
                    self.read_stderr,    # E_STDERR_READ
                    self.sudo_password,  # E_MASTER_READ
                    self.write_master,   # E_MASTER_WRITE
                    self.reap_child,     # E_SIGCHLD
                    self.sudo_timeout,   # E_TIMEOUT
                    self.stop_command    # E_STOP
                ]
            ),

            # Q_SUDO_STDIO
            State(
                self.start_auth_timer, self.stop_timer,
                [
                    self.write_stdin,    # E_STDIN_WRITE
                    self.read_stdout,    # E_STDOUT_READ
                    self.read_stderr,    # E_STDERR_READ
                    self.sudo_password,  # E_MASTER_READ
                    self.write_master,   # E_MASTER_WRITE
                    self.reap_child,     # E_SIGCHLD
                    self.sudo_timeout,   # E_TIMEOUT
                    self.stop_command    # E_STOP
                ]
            ),

            # Q_RUNNING
            State(
                self.start_command_timer, None,
                [
                    self.write_stdin,   # E_STDIN_WRITE
                    self.read_stdout,   # E_STDOUT_READ
                    self.read_stderr,   # E_STDERR_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.reap_child,    # E_SIGCHLD
                    self.run_timeout,   # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_EXITED
            State(
                None, self.stop_timer,
                [
                    self.write_stdin,   # E_STDIN_WRITE
                    self.check_stdout,  # E_STDOUT_READ
                    self.read_stderr,   # E_STDERR_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    None,               # E_SIGCHLD
                    self.no_close,      # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_TERM
            State(
                self.term_child, self.stop_timer,
                [
                    self.write_stdin,   # E_STDIN_WRITE
                    self.read_stdout,   # E_STDOUT_READ
                    self.read_stderr,   # E_STDERR_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.reap_child,    # E_SIGCHLD
                    self.term_failed,   # E_TIMEOUT
                    None                # E_STOP
                ]
            ),

            # Q_KILL
            State(
                self.kill_child, self.stop_timer,
                [
                    self.write_stdin,   # E_STDIN_WRITE
                    self.read_stdout,   # E_STDOUT_READ
                    self.read_stderr,   # E_STDERR_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.reap_child,    # E_SIGCHLD
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
        """ Determine initial state """
        if self.use_ssh_password:
            q_next = CommandBase.Q_SSH_MASTER
        elif self.use_sudo_password:
            q_next = CommandBase.Q_SUDO_MASTER
        else:
            q_next = CommandBase.Q_RUNNING

        return q_next

    def write_stdin(self, _state : int, _event : int, _data : Any) -> int:
        """ Write outstanding data to stdin """
        self.stdin.write()
        # same state

    def read_stdout(self, _state : int, _event : int, _data : Any) -> int:
        """ Collect data from stdout """
        self.stdout.read()
        # same state

    def check_stdout(self, _state : int, _event : int, _data : Any) -> int:
        """ Collect data from stdout and check for close """
        self.stdout.read()

        if self.stdout.is_reading:
            return None

        return CommandBase.Q_DONE

    def read_stderr(self, _state : int, _event : int, _data : Any) -> int:
        """ Collect data from stderr """
        self.stderr.read()
        # same state

    def read_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Collect data from PTY master """
        self.master.read()
        # same state

    def write_master(self, _state : int, _event : int, _data : Any) -> int:
        """ Write outstanding data to PTY master """
        self.master.write()
        # same state

    def start_auth_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ Start the authentication timer """
        self.start_timer(self.password_timeout)
        # same state

    def start_command_timer(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Start command timer """
        self.start_timer(self.command_timeout)

    def stop_timer(self, _state : int, _event : int, _data : Any) -> int:
        """ Stop any running timer """
        self.cancel_timer()
        # no change

    def ssh_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start process termination """
        if not self.reason:
            self.reason = 'SSH authentication timeout'

        return CommandBase.Q_TERM

    def sudo_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start process termination """
        if not self.reason:
            self.reason = 'SUDO authentication timeout'

        return CommandBase.Q_TERM

    def run_timeout(self, _state : int, _event : int, _data : Any) -> int:
        """ Start process termination """
        if not self.reason:
            self.reason = 'Command timeout'

        return CommandBase.Q_TERM

    def ssh_password(self, _state : int, _event : int, _data : Any) -> int:
        """ Check for ssh password prompt """
        self.master.read()

        if not self.master.input_data:
            return None

        try:
            text = self.master.input_data.decode()
        except:  # pylint: disable=bare-except
            # Unicode sequence may be incomplete.
            return None

        if not self.ssh_prompt.search(text):
            return None

        self.master.write(self.password)

        if self.use_sudo_password:
            return CommandBase.Q_SUDO_STDIO

        return CommandBase.Q_RUNNING

    def sudo_password(self, state : int, _event : int, _data : Any) -> int:
        """ Check for sudo password prompt """
        if state == CommandBase.Q_SUDO_MASTER:
            reader = self.master
            writer = self.master
        else:
            reader = self.stdout
            writer = self.stdin

        reader.read()

        if not reader.input_data:
            return None
        try:
            text = reader.input_data.decode()
        except:  # pylint: disable=bare-except
            # Unicode sequence may be incomplete.
            return None

        if not self.sudo_prompt.search(text):
            return None

        writer.write(self.password)

        return CommandBase.Q_RUNNING

    def reap_child(self, state : int, _event : int, _data : Any) -> int:
        """ Reap the child process """
        if not self.child_pid:
            return None

        pid, status = os.waitpid(self.child_pid, os.WNOHANG)
        if pid != self.child_pid:
            return None

        self.disable_sigchld()
        self.child_pid = None
        self.status = os.waitstatus_to_exitcode(status)

        if not self.stdout.is_reading or state > CommandBase.Q_EXITED:
            return CommandBase.Q_DONE

        return CommandBase.Q_EXITED

    def no_close(self, _state : int, _event : int, _data : Any) -> int:
        """ Indicate that stdout did not close """
        self.reason = 'Warning: stdout did not close as expected'
        return CommandBase.Q_DONE

    def stop_command(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Start termination sequence """
        if not self.reason:
            self.reason = 'Command canceled'

        if self.child_pid is None:
            return CommandBase.Q_DONE

        return CommandBase.Q_TERM

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child process termination """
        if not self.child_pid:
            return CommandBase.Q_DONE

        os.kill(self.child_pid, SIGTERM)
        self.start_timer(self.term_timeout)

        return None

    def term_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Move on to kill child """
        return CommandBase.Q_KILL

    def kill_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child process kill """
        if not self.child_pid:
            return CommandBase.Q_DONE

        os.kill(self.child_pid, SIGKILL)
        self.start_timer(self.kill_timeout)

        return None

    def kill_failed(self, _state : int, _event : int, _data : Any) -> int:
        """ Child command process would not terminate """
        if not self.status:
            self.status = 1

        if not self.reason:
            self.reason = 'Command did not terminate'

        return CommandBase.Q_DONE

    def close_all(
        self, _state : int, _event : int, _data : Any
    ) -> int:
        """ Close all events and I/O endpoints """
        self.close()

    def close(self) -> None:
        """ Cancel timer events and close all endpoints """
        self.cancel_timer()
        self.disable_sigchld()

        if self.stdin:
            self.stdin.close()

        if self.stdout:
            self.stdout.close()

        if self.stderr:
            self.stderr.close()

        if self.master:
            self.master.close()

        if self.slave is not None:
            os.close(self.slave)
            self.slave = None

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
