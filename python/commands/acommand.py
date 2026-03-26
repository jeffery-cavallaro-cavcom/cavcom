"""
This class extends the base command class to provide an asynchronous command
execution class.  The event loop is based on a Future whose value is a list of
current events.
"""

import asyncio
from signal import SIGCHLD
import sys
from typing import ClassVar

from events.aio_endpoint import AIOEndpoint
from events.signal_ahandler import SignalHandler

from commands.command_base import CommandBase

class Command(CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes
    ChildSignalDispatcher : ClassVar[SignalHandler] = None

    loop : asyncio.AbstractEventLoop
    sigchld : bool
    timer : asyncio.TimerHandle
    event_wait : asyncio.Future
    next_events : set[int]

    def __init__(self, *args, **kwargs):
        """
        Initialize the command

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        super().__init__(*args, **kwargs)

        if not Command.ChildSignalDispatcher:
            Command.ChildSignalDispatcher = SignalHandler(SIGCHLD)

        self.loop = asyncio.get_running_loop()
        self.sigchld = False
        self.timer = None
        self.event_wait = None
        self.next_events = set()

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

    def setup_endpoints(self) -> None:
        """ Setup all endpoints """
        self.stdin = AIOEndpoint(
            self.add_event, write_data=Command.E_STDIN_WRITE
        )

        self.stdout = AIOEndpoint(
            self.add_event, read_data=Command.E_STDOUT_READ
        )

        if self.redirect_stderr:
            self.stderr = None
        else:
            self.stderr = AIOEndpoint(
                self.add_event, read_data=Command.E_STDERR_READ
            )

    def setup_master(self, master: int) -> None:
        """ Setup master endpoint """
        self.master = AIOEndpoint(
            self.add_event,
            read_data=Command.E_MASTER_READ,
            write_data=Command.E_MASTER_WRITE,
            reader=master
        )

    def enable_sigchld(self) -> None:
        """ Enable SIGCHLD events """
        if not self.sigchld:
            self.ChildSignalDispatcher.register_callback(
                self.add_event, Command.E_SIGCHLD
            )
            self.sigchld = True

    def disable_sigchld(self) -> None:
        """ Disable SIGCHLD events """
        if self.sigchld:
            self.ChildSignalDispatcher.deregister_callback(self.add_event)
            self.sigchld = False

    def start_timer(self, timeout : int) -> None:
        """ Start timer """
        self.cancel_timer()

        if timeout is None:
            self.timer = None
        else:
            self.timer = self.loop.call_later(
                timeout, self.add_event, Command.E_TIMEOUT
            )

    def cancel_timer(self) -> None:
        """ Cancel timer """
        if self.timer:
            self.timer.cancel()
            self.timer = None

    async def execute(self) -> None:
        """ Execute the command """
        self.event_wait = asyncio.Future()

        self.fork_and_exec()

        self.run()  # Establish the initial state

        while self.q_now != Command.Q_DONE:
            await self.event_wait
            while self.next_events:
                event_id = self.next_events.pop()
                self.run(event_id)
            self.event_wait = asyncio.Future()

    def close(self) -> None:
        """ Tear down event loop and close endpoints """
        super().close()

        if self.event_wait and not self.event_wait.done():
            self.event_wait.cancel()
            self.event_wait = None

if __name__ == '__main__':
    async def main() -> None:
        """ Execute the command """
        with Command.create_command() as acommand:
            try:
                await acommand.execute()
            except Exception as error:  # pylint: disable=broad-except
                sys.exit(error)

            print('STATUS:', acommand.status)
            print('STATE:', acommand.reason)

            if acommand.master and acommand.master.input_data:
                print('MASTER:')
                print(acommand.master.input_data.decode())

            if acommand.stdout and acommand.stdout.input_data:
                print('STDOUT:')
                print(acommand.stdout.input_data.decode())

            if acommand.stderr and acommand.stderr.input_data:
                print('STDERR:')
                print(acommand.stderr.input_data.decode())

        sys.exit(acommand.status)

    asyncio.run(main())
