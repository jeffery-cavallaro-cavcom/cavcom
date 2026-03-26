"""
This class extends the base command class to provide a synchronous command
execution class.  The event loop is based on a select loop.  Signal and timer
events use the "write to a descriptor" trick to introduced signal and timeout
events into the select event loop.
"""

from selectors import DefaultSelector
from signal import SIGCHLD
import sys
from typing import ClassVar

from events.select_endpoint import SelectEndpoint
from events.signal_event import SignalEvent
from events.timer_event import TimerEvent
from events.signal_handler import SignalHandler

from commands.command_base import CommandBase

class Command(CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes
    ChildSignalDispatch : ClassVar[SignalHandler] = SignalHandler(SIGCHLD)

    select_loop : DefaultSelector
    sigchld : SignalEvent
    timer : TimerEvent

    def __init__(self, *args, **kwargs):
        """
        Initialize the command and select loop

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        super().__init__(*args, **kwargs)

        self.select_loop = DefaultSelector()
        self.sigchld = None
        self.timer = None

    def setup_endpoints(self) -> None:
        """ Setup all endpoints """
        self.stdin = SelectEndpoint(
            self.select_loop, write_data=Command.E_STDIN_WRITE
        )

        self.stdout = SelectEndpoint(
            self.select_loop, read_data=Command.E_STDOUT_READ,
        )

        if self.redirect_stderr:
            self.stderr = None
        else:
            self.stderr = SelectEndpoint(
                self.select_loop, read_data=Command.E_STDERR_READ
            )

    def setup_master(self, master: int) -> None:
        """ Setup master endpoint """
        self.master = SelectEndpoint(
            self.select_loop,
            read_data=Command.E_MASTER_READ,
            write_data=Command.E_MASTER_WRITE,
            reader=master
        )

    def enable_sigchld(self) -> None:
        """ Enable SIGCHLD events """
        if not self.sigchld:
            self.sigchld = SignalEvent(
                self.ChildSignalDispatch,
                self.select_loop,
                event_data=Command.E_SIGCHLD
            )

    def disable_sigchld(self) -> None:
        """ Disable SIGCHLD events """
        if self.sigchld:
            self.sigchld.close()
            self.sigchld = None

    def start_timer(self, timeout : float) -> None:
        """ Start timer """
        self.cancel_timer()

        if timeout is None:
            self.timer = None
        else:
            self.timer = TimerEvent(
                timeout, self.select_loop, event_data=Command.E_TIMEOUT
            )
            self.timer.start()

    def cancel_timer(self) -> None:
        """ Cancel timer """
        if self.timer:
            self.timer.close()
            self.timer = None

    def execute(self) -> None:
        """ Execute the command """
        self.fork_and_exec()

        self.run()  # Establish initial state

        while self.q_now != Command.Q_DONE:
            events = self.select_loop.select()
            for key, _ in events:
                event_id = key.data

                if event_id == Command.E_SIGCHLD:
                    if self.sigchld:
                        self.sigchld.acknowledge()
                elif event_id == Command.E_TIMEOUT:
                    if self.timer:
                        self.timer.acknowledge()

                self.run(event_id)

    def close(self) -> None:
        """ Tear down event loop and close endpoints """
        super().close()

        if self.select_loop:
            self.select_loop.close()
            self.select_loop = None

if __name__ == '__main__':
    def main() -> None:
        """ Execute the command """
        with Command.create_command() as command:
            try:
                command.execute()
            except Exception as error:  # pylint: disable=broad-except
                sys.exit(error)

            print('STATUS:', command.status)
            print('REASON:', command.reason)

            if command.master and command.master.input_data:
                print('MASTER:')
                print(command.master.input_data.decode())

            if command.stdout and command.stdout.input_data:
                print('STDOUT:')
                print(command.stdout.input_data.decode())

            if command.stderr and command.stderr.input_data:
                print('STDERR:')
                print(command.stderr.input_data.decode())

        sys.exit(command.status)

    main()
