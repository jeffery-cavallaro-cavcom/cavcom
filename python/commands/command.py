"""
This class extends the base command class to provide a synchronous command
execution class.  The event loop is based on a select loop.  Signal and timer
events use the "write to a descriptor" trick to introduced signal and timeout
events into the select event loop.
"""

from selectors import DefaultSelector
from signal import signal, SIGINT, SIGTERM
from subprocess import Popen, PIPE, DEVNULL, STDOUT
import sys
from typing import Any

from events.timer_event import TimerEvent
from events.event_handler import EventHandler
from events.io_endpoint import IOEndpoint
from events.select_endpoint import SelectEndpoint
from commands.command_base import CommandBase

# pylint: disable=duplicate-code

class Command(CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes
    event_loop : DefaultSelector
    timer : TimerEvent
    stop : EventHandler
    child : Popen

    def __init__(self, *args, **kwargs):
        """
        Initialize the command and select loop

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        self.event_loop = DefaultSelector()
        self.timer = None
        self.stop = None
        self.child = None

        super().__init__(*args, **kwargs)

    def setup_master(self) -> None:
        """ Setup master endpoint """
        self.master = SelectEndpoint(
            self.event_loop,
            self.pty.master,
            no_close=True,
            read_data=self.E_MASTER_READ,
            write_data=self.E_MASTER_WRITE
        )
        self.master.register_read()

    def start_timer(self, timeout : float) -> None:
        """ Start timer """
        self.cancel_timer()

        if timeout is None:
            self.timer = None
        else:
            self.timer = TimerEvent(
                timeout, self.event_loop, event_data=self.E_TIMEOUT
            )
            self.timer.start()

    def cancel_timer(self) -> None:
        """ Cancel timer """
        if self.timer:
            self.timer.close()
            self.timer = None

    def setup_stop(self) -> None:
        """ Set up stop event """
        if not self.stop:
            self.stop = EventHandler(self.event_loop, event_data=self.E_STOP)

    def disable_stop(self) -> None:
        """ Disable stop event """
        if self.stop:
            self.stop.close()
            self.stop = None

    def cancel(self, *_args, **_kwargs) -> None:
        """ Trigger a stop event (can be used as a signal handler) """
        if self.q_now < self.Q_RUNNING:
            self.stop.trigger()
        else:
            self.child.terminate()

        if not self.reason:
            self.reason = 'Command canceled'

    def execute(self) -> None:
        """ Execute the command """
        # We only need stdin if doing sudo password authentication on stdio.
        if self.use_sudo_password and self.remote:
            stdin_mode = PIPE
        else:
            stdin_mode = DEVNULL

        # No need for stderr if redirecting to stdout.
        if self.redirect_stderr:
            stderr_mode = STDOUT
        else:
            stderr_mode = PIPE

        # pylint: disable=subprocess-popen-preexec-fn
        # pylint: disable=consider-using-with
        self.child = Popen(
            self.command,
            stdin=stdin_mode,
            stdout=PIPE,
            stderr=stderr_mode,
            pass_fds=[self.pty.slave] if self.pty else [],
            start_new_session=True,
            preexec_fn=self.set_terminal if self.pty else None
        )

        self.stdout = SelectEndpoint(
            self.event_loop,
            self.child.stdout.fileno(),
            no_close=True,
            read_data=self.E_STDOUT_READ
        )

        if not self.redirect_stderr:
            self.stderr = IOEndpoint(self.child.stderr.fileno(), no_close=True)

        self.setup_stop()

        self.run()  # Establish initial state

        self.run_event_loop()

    def run_event_loop(self) -> None:
        """ Run the event loop until done """
        while self.q_now != Command.Q_DONE:
            events = self.event_loop.select()
            for key, which in events:
                if isinstance(key.data, tuple):
                    event_ids = SelectEndpoint.get_events(key, which)
                else:
                    event_ids = [key.data]
                for event_id in event_ids:
                    if event_id == Command.E_TIMEOUT:
                        if self.timer:
                            self.timer.acknowledge()
                    elif event_id == Command.E_STOP:
                        if self.stop:
                            self.stop.acknowledge()
                    self.run(event_id)

    def start_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Start SUDO authentication on stdio """
        self.stdout.register_read()
        self.start_auth_timer(state, event, data)

    def end_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Stop SUDO authentication on stdio """
        self.stop_timer(state, event, data)
        self.stdout.unregister_read()

    def sudo_stdio(self, _state : int, _event : int, _data : Any) -> int:
        """ Attempt SUDO authentication (on stdio) """
        self.stdout.read()
        if self.stdout.errno:
            self.status = self.status or self.stdout.errno
            self.reason = self.reason or self.stdout.error_text
            return self.Q_TERM

        text = self.stdout.fetch_input(text=True)
        if not self.sudo_prompt.search(text):
            return None

        return self.Q_RUNNING

    def wait_for_exit(self, state : int, event : int, data : Any) -> int:
        """ Wait for child exit """
        password = None

        if self.use_ssh_password and self.use_sudo_password:
            password = self.password
        else:
            password = None

        try:
            stdout, stderr = self.child.communicate(
                input=password, timeout=self.command_timeout
            )
        except TimeoutError:
            return self.run_timeout(state, event, data)
        except Exception as error:  # pylint: disable=broad-except
            self.status = self.status or 1
            self.reason = self.reason or str(error)
            return self.Q_TERM

        self.collect_output(stdout, stderr)
        self.status = self.child.returncode

        return self.Q_DONE

    def collect_output(self, stdout : bytes, stderr : bytes) -> None:
        """ Collect output from stdout and/or stderr """
        if stdout:
            self.stdout.input_data.append(stdout)

        if stderr and self.stderr:
            self.stderr.input_data.append(stderr)

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Start child command process termination (derived override) """
        self.child.terminate()

        try:
            stdout, stderr = self.child.communicate(
                input=None, timeout=self.term_timeout
            )
        except:  # pylint: disable=bare-except
            return self.Q_KILL

        self.collect_output(stdout, stderr)
        self.status = self.child.returncode

        return self.Q_DONE

    def kill_child(self, state : int, event : int, data : Any) -> int:
        """ Kill child command process """
        self.child.kill()

        try:
            stdout, stderr = self.child.communicate(
                input=None, timeout=self.kill_timeout
            )
        except:  # pylint: disable=bare-except
            self.kill_failed(state, event, data)
            return self.Q_DONE

        self.collect_output(stdout, stderr)
        self.status = self.child.returncode

        return self.Q_DONE

    def close(self) -> None:
        """ Tear down event loop and close endpoints """
        self.disable_stop()

        super().close()

        if self.event_loop:
            self.event_loop.close()

if __name__ == '__main__':
    def main() -> None:
        """ Execute the command """
        with Command.create_command() as command:
            for signo in [SIGINT, SIGTERM]:
                signal(signo, command.cancel)

            try:
                command.execute()
            except Exception as error:  # pylint: disable=broad-except
                sys.exit(error)

            print('STATUS:', command.status)
            print('REASON:', command.reason)

            if command.master:
                text = command.master.fetch_input(text=True)
                if text:
                    print('MASTER:')
                    print(text)

            if command.stdout:
                text = command.stdout.fetch_input(text=True)
                if text:
                    print('STDOUT:')
                    print(text)

            if command.stderr:
                text = command.stderr.fetch_input(text=True)
                if text:
                    print('STDERR:')
                    print(text)

        sys.exit(command.status)

    main()
