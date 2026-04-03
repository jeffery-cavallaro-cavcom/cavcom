"""
This class extends the base command class to provide an asynchronous command
execution class.  The event loop is based on a Future whose value is a set of
current events.
"""

import asyncio
from asyncio.subprocess import DEVNULL, PIPE, STDOUT
from errno import EIO
import signal
import sys
from typing import Any, ClassVar, Optional

from events.aio_endpoint import AIOEndpoint
from events.fsm import FiniteStateMachine, State

from commands.command_base import CommandBase

class IOManager:
    """ Manage a STDIO Endpoint """
    mode : int
    data : bytearray
    closed : bool
    errno : int
    error_text : str

    def __init__(self, mode : int):
        """
        Initialize the endpoint

        Arguments:
            mode:
                Input type code (from asyncio.subprocess).  This can be DEVNULL
                or PIPE for stdin, should always be PIPE for stdout, and can be
                PIPE or STDOUT for stderr.
        """
        self.mode = mode
        self.data = None
        self.closed = mode != PIPE
        self.errno = None
        self.error_text = None

    def add_data(self, data : bytes) -> None:
        """
        Append data to the data buffer

        Arguments:
            data:
                Data to append.
        """
        if self.data:
            self.data.extend(data)
        else:
            self.data = bytearray(data)

    def close(self, error : Optional[Exception] = None) -> None:
        """
        Close the endpoint

        Arguments:
            exception:
                Reason for abnormal close.
        """
        self.closed = True

        if error:
            if isinstance(error, OSError):
                self.errno = error.errno
                self.error_text = error.strerror
            else:
                self.errno = EIO
                self.error_text = str(error)

class Command(asyncio.SubprocessProtocol, FiniteStateMachine, CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes

    # EVENTS
    E_STDOUT_READ : ClassVar[int] = 0
    E_MASTER_READ : ClassVar[int] = 1
    E_MASTER_WRITE : ClassVar[int] = 2
    E_CLOSED : ClassVar[int] = 3
    E_EXIT : ClassVar[int] = 4
    E_TIMEOUT : ClassVar[int] = 5
    E_STOP : ClassVar[int] = 6

    loop : asyncio.AbstractEventLoop
    timer : asyncio.TimerHandle
    event_wait : asyncio.Future
    next_events : set[int]

    transport : asyncio.SubprocessTransport
    protocol : asyncio.SubprocessProtocol
    stdin : IOManager
    stdout : IOManager
    stderr : IOManager

    def __init__(self, *args, **kwargs):
        """
        Initialize the command

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        asyncio.SubprocessProtocol.__init__(self)
        FiniteStateMachine.__init__(self, self.setup_fsm())
        CommandBase.__init__(self, *args, **kwargs)

        self.loop = asyncio.get_running_loop()
        self.timer = None
        self.event_wait = None
        self.next_events = set()

        self.transport = None
        self.protocol = None

        # We only need stdin if doing sudo password authentication on stdio.
        if self.use_ssh_password and self.use_sudo_password:
            mode = PIPE
        else:
            mode = DEVNULL
        self.stdin = IOManager(mode)

        self.stdout = IOManager(PIPE)

        # No need for stderr if redirecting to stdout.
        if self.redirect_stderr:
            mode = STDOUT
        else:
            mode = PIPE
        self.stderr = IOManager(mode)

    def add_event(self, event_id : int) -> None:
        """
        Add a new event to the event queue

        Arguments:
            event_id:
                Event ID to add to event queue.
        """
        self.next_events.add(event_id)

        if not self.event_wait.done():
            self.event_wait.set_result(self.next_events)

    def setup_master(self) -> None:
        """ Setup master endpoint """
        self.master = AIOEndpoint(
            self.pty.master,
            event_callback=self.add_event,
            read_data=Command.E_MASTER_READ,
            write_data=Command.E_MASTER_WRITE,
        )
        self.pty.master = None  # Take ownership
        self.master.register_read()

    def start_timer(self, timeout : float) -> None:
        """
        [Re]start a timer

        Arguments:
            Timeout, in seconds.
        """
        self.cancel_timer()

        if timeout is None:
            self.timer = None
        else:
            self.timer = self.loop.call_later(
                timeout, self.add_event, Command.E_TIMEOUT
            )

    def cancel_timer(self) -> None:
        """ Cancel any started timer """
        if self.timer:
            self.timer.cancel()
            self.timer = None

    def cancel(self, *_args, **_kwargs) -> None:
        """ Trigger a stop event (can be used as a signal handler) """
        self.add_event(Command.E_STOP)

    async def execute(self) -> None:
        """ Execute the command """
        self.event_wait = asyncio.Future()

        self.allocate_pty()
        self.setup_master()

        self.transport, self.protocol = await self.loop.subprocess_exec(
            lambda: self,
            *self.command,
            stdin=self.stdin.mode,
            stdout=self.stdout.mode,
            stderr=self.stderr.mode,
            pass_fds=[self.pty.slave],
            start_new_session=True,
            preexec_fn=self.set_terminal
        )

        self.run()  # Establish the initial state

        while self.q_now != Command.Q_DONE:
            await self.event_wait
            while self.next_events:
                event_id = self.next_events.pop()
                self.run(event_id)
            self.event_wait = asyncio.Future()

    def pipe_data_received(self, fd: int, data: bytes) -> None:
        """
        Handle input data

        Arguments:
            fd:
                1 for stdout or 2 for stderr.
            data:
                New input data.
        """
        if not data:
            return

        if fd == 1:
            self.stdout.add_data(data)
            if self.q_now == self.Q_SUDO_STDIO:
                # SUDO password authentication over stdio.
                self.add_event(self.E_STDOUT_READ)
        elif fd == 2:
            self.stderr.add_data(data)

    def pipe_connection_lost(
        self, fd: int, exc: Optional[Exception] = None
    ) -> None:
        """
        Mark I/O endpoint closed

        Arguments:
            fd:
                0 for stdin, 1 for stdout, or 2 for stderr.
            exc:
                Exception for closed due to an error.
        """
        if fd == 0:
            self.stdin.close(exc)
        elif fd == 1:
            self.stdout.close(exc)
        elif fd == 2:
            self.stderr.close(exc)
        else:
            return

        self.add_event(self.E_CLOSED)

    def process_exited(self) -> None:
        """ Trigger an exited event """
        self.add_event(self.E_EXIT)

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
                self.start_auth_timer, self.stop_timer,
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
                self.start_command_timer, None,
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

    def sudo_stdio(self, _state : int, _event : int, _data : Any) -> int:
        """ SUDO authentication action method (on master) """
        if not self.match_sudo_prompt(self.stdout.data):
            return None

        self.transport.get_pipe_transport(0).write(self.password)

        return self.Q_RUNNING

    def check_done(self, state : int, _event : int, _data : Any) -> int:
        """ Check for all stdio closed action method """
        self.status = self.transport.get_returncode()

        if (
            self.stdin.closed and self.stdout.closed and self.stderr.closed or
            state > self.Q_EXITED
        ):
            return self.Q_DONE

        return self.Q_EXITED

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child process termination begin method """
        self.transport.terminate()

    def kill_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child process kill begin method """
        self.transport.kill()

    def close(self) -> None:
        """ Release all resources """
        super().close()

        if self.transport:
            self.transport.close()
            self.transport = None

        self.protocol = None

if __name__ == '__main__':
    async def main() -> None:
        """ Execute the command """
        with Command.create_command() as acommand:
            for signo in [signal.SIGINT, signal.SIGTERM]:
                signal.signal(signo, acommand.cancel)

            try:
                await acommand.execute()
            except Exception as error:  # pylint: disable=broad-except
                sys.exit(error)

            print('STATUS:', acommand.status)
            print('REASON:', acommand.reason)

            if acommand.master and acommand.master.input_data:
                print('MASTER:')
                print(acommand.master.input_data.decode())

            if acommand.stdout and acommand.stdout.data:
                print('STDOUT:')
                print(acommand.stdout.data.decode())

            if acommand.stderr and acommand.stderr.data:
                print('STDERR:')
                print(acommand.stderr.data.decode())

        sys.exit(acommand.status)

    asyncio.run(main())
