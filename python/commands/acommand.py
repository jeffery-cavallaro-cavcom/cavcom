"""
This class extends the base command class to provide an asynchronous command
execution class.  The event loop is based on a Future whose value is a set of
current events.
"""

import asyncio
from asyncio.subprocess import DEVNULL, PIPE, STDOUT
from errno import EIO
from signal import signal, SIGINT, SIGTERM
import sys
from typing import Any, Optional

from events.aevent_loop import AEventLoop
from events.io_endpoint import IOEndpoint
from events.aio_endpoint import AIOEndpoint
from commands.command_base import CommandBase

# pylint: disable=duplicate-code

class Command(asyncio.SubprocessProtocol, CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes
    event_loop : AEventLoop
    timer : asyncio.TimerHandle
    transport : asyncio.SubprocessTransport
    protocol : asyncio.SubprocessProtocol

    def __init__(self, *args, **kwargs):
        """
        Initialize the command

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        self.event_loop = AEventLoop()
        self.timer = None
        self.transport = None
        self.protocol = None

        asyncio.SubprocessProtocol.__init__(self)
        CommandBase.__init__(self, *args, **kwargs)

    def setup_master(self) -> None:
        """ Setup master endpoint """
        self.master = AIOEndpoint(
            self.pty.master,
            no_close=True,
            event_callback=self.event_loop.add_event,
            read_data=self.E_MASTER_READ,
            write_data=self.E_MASTER_WRITE
        )
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
            self.timer = self.event_loop.loop.call_later(
                timeout, self.event_loop.add_event, self.E_TIMEOUT
            )

    def cancel_timer(self) -> None:
        """ Cancel any started timer """
        if self.timer:
            self.timer.cancel()
            self.timer = None

    def cancel(self, *_args, **_kwargs) -> None:
        """ Trigger a stop event (can be used as a signal handler) """
        self.event_loop.add_event(self.E_STOP)

        if not self.reason:
            self.reason = 'Command canceled'

    async def execute(self) -> None:
        """ Execute the command """
        # We only need stdin if doing sudo password authentication on stdio.
        sudo_stdio = self.use_sudo_password and self.remote
        if sudo_stdio:
            stdin_mode = PIPE
            self.stdin = IOEndpoint(None, no_close=True)
        else:
            stdin_mode = DEVNULL

        self.stdout = IOEndpoint(None, no_close=True)

        # No need for stderr if redirecting to stdout.
        if self.redirect_stderr:
            stderr_mode = STDOUT
        else:
            stderr_mode = PIPE
            self.stderr = IOEndpoint(None, no_close=True)

        mechanisms = await self.event_loop.loop.subprocess_exec(
            lambda: self,
            *self.command,
            stdin=stdin_mode,
            stdout=PIPE,
            stderr=stderr_mode,
            pass_fds=[self.pty.slave] if self.pty else [],
            start_new_session=True,
            preexec_fn=self.set_terminal if self.pty else None
        )
        self.transport, self.protocol = mechanisms

        self.run()  # Establish the initial state

        while self.q_now != Command.Q_DONE:
            event_id = await self.event_loop.wait_for_event()
            self.run(event_id)

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
            self.stdout.input_data.append(data)
            if self.q_now == self.Q_SUDO_STDIO:
                # SUDO password authentication over stdio.
                self.event_loop.add_event(self.E_STDOUT_READ)
        elif fd == 2 and self.stderr:
            self.stderr.input_data.append(data)

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
            endpoint = self.stdin
        elif fd == 1:
            endpoint = self.stdout
        elif fd == 2:
            endpoint = self.stderr
        else:
            endpoint = None

        if not endpoint:
            return

        endpoint.close()

        if exc:
            if isinstance(exc, IOError):
                endpoint.errno = exc.errno
                endpoint.error_text = exc.strerror
            else:
                endpoint.errno = EIO
                endpoint.error_text = str(exc)

        self.event_loop.add_event(self.E_CLOSED)

    def process_exited(self) -> None:
        """ Trigger an exited event """
        self.event_loop.add_event(self.E_EXIT)

    def check_done(self, state : int, _event : int, _data : Any) -> int:
        """ Check for all stdio closed action method """
        self.status = self.transport.get_returncode()

        all_closed  = (
            (not self.stdin or self.stdin.is_closed) and
            (not self.stdout or self.stdout.is_closed) and
            (not self.stderr or self.stderr.is_closed)
        )

        if all_closed or state > self.Q_EXITED:
            return self.Q_DONE

        return self.Q_EXITED

    def sudo_stdio(self, _state : int, _event : int, _data : Any) -> int:
        """ Attempt SUDO authentication (on stdio) """
        text = self.stdout.fetch_input(text=True)
        if not self.sudo_prompt.search(text):
            return None

        self.transport.get_pipe_transport(0).write(self.password)

        return self.Q_RUNNING

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child command process termination """
        self.transport.terminate()

    def kill_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Kill child command process """
        self.transport.kill()

    def close(self) -> None:
        """ Release all resources """
        super().close()

        if self.transport:
            self.transport.close()
            self.transport = None

if __name__ == '__main__':
    async def main() -> None:
        """ Execute the command """
        with Command.create_command() as acommand:
            for signo in [SIGINT, SIGTERM]:
                signal(signo, acommand.cancel)

            try:
                await acommand.execute()
            except Exception as error:  # pylint: disable=broad-except
                sys.exit(error)

            print('STATUS:', acommand.status)
            print('REASON:', acommand.reason)

            if acommand.master:
                text = acommand.master.fetch_input(text=True)
                if text:
                    print('MASTER:')
                    print(text)

            if acommand.stdout:
                text = acommand.stdout.fetch_input(text=True)
                if text:
                    print('STDOUT:')
                    print(text)

            if acommand.stderr:
                text = acommand.stderr.fetch_input(text=True)
                if text:
                    print('STDERR:')
                    print(text)

        sys.exit(acommand.status)

    asyncio.run(main())
