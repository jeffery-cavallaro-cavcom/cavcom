"""
This class extends the base command class to provide a synchronous command
execution class.  The event loop is based on a select loop.  Signal and timer
events use the "write to a descriptor" trick to introduced signal and timeout
events into the select event loop.
"""

import os
from selectors import DefaultSelector, SelectorKey, EVENT_READ
import signal
from subprocess import Popen, DEVNULL, PIPE, STDOUT
import sys
from typing import Any, ClassVar

from events.select_endpoint import SelectEndpoint
from events.event_handler import EventHandler
from events.timer_event import TimerEvent
from events.fsm import FiniteStateMachine, State

from commands.command_base import CommandBase

class Command(FiniteStateMachine, CommandBase):
    """ Execute a command """
    # pylint: disable=too-many-instance-attributes

    # EVENTS
    E_STDOUT_READ : ClassVar[int] = 0
    E_MASTER_READ : ClassVar[int] = 1
    E_MASTER_WRITE : ClassVar[int] = 2
    E_TIMEOUT : ClassVar[int] = 3
    E_STOP : ClassVar[int] = 4

    select_loop : DefaultSelector
    timer : TimerEvent
    stop : EventHandler
    child : Popen
    stdout_blocking : bool
    stdout_key : SelectorKey
    stdout : bytearray
    stderr : bytearray

    def __init__(self, *args, **kwargs):
        """
        Initialize the command and select loop

        Arguments:
            args:
                Additional positional arguments for BaseCommand().
            kwargs:
                Additional keyword arguments for BaseCommand().
        """
        FiniteStateMachine.__init__(self, self.setup_fsm())
        CommandBase.__init__(self, *args, **kwargs)

        self.select_loop = DefaultSelector()
        self.timer = None
        self.stop = None
        self.child = None
        self.stdout_blocking = None
        self.stdout_key = None
        self.stdout = None
        self.stderr = None

    def setup_master(self) -> None:
        """ Setup master endpoint """
        self.master = SelectEndpoint(
            self.select_loop,
            self.pty.master,
            read_data=Command.E_MASTER_READ,
            write_data=Command.E_MASTER_WRITE
        )
        self.pty.master = None  # Take ownership
        self.master.register_read()

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

    def setup_stop(self) -> None:
        """ Set up stop event """
        if not self.stop:
            self.stop = EventHandler(
                self.select_loop, event_data=Command.E_STOP
            )

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

        self.reason = 'Command canceled'

    def execute(self) -> None:
        """ Execute the command """
        self.allocate_pty()
        self.setup_master()

        # We only need stdin if doing sudo password authentication on stdio.
        if self.use_ssh_password and self.use_sudo_password:
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
            pass_fds=[self.pty.slave],
            start_new_session=True,
            preexec_fn=self.set_terminal
        )

        self.setup_stop()

        self.run()  # Establish initial state

        while self.q_now != Command.Q_DONE:
            events = self.select_loop.select()
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
                    self.sudo_timeout,  # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_SUDO_STDIO
            State(
                self.start_sudo_stdio, self.end_sudo_stdio,
                [
                    self.sudo_stdio,    # E_STDOUT_READ
                    self.read_master,   # E_MASTER_READ
                    self.write_master,  # E_MASTER_WRITE
                    self.sudo_timeout,  # E_TIMEOUT
                    self.stop_command   # E_STOP
                ]
            ),

            # Q_RUNNING
            State(
                self.wait_for_exit, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            ),

            # Q_EXITED (unused)
            State(
                None, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            ),

            # Q_TERM
            State(
                self.term_child, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            ),

            # Q_KILL
            State(
                self.kill_child, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            ),

            # Q_DONE
            State(
                self.close_all, None,
                [
                    None,  # E_STDOUT_READ
                    None,  # E_MASTER_READ
                    None,  # E_MASTER_WRITE
                    None,  # E_TIMEOUT
                    None   # E_STOP
                ]
            )
        ]

    def start_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Start SUDO password on stdio """
        self.stdout_blocking = os.get_blocking(self.child.stdout.fileno())
        os.set_blocking(self.child.stdout.fileno(), False)

        self.stdout_key = self.select_loop.register(
            self.child.stdout, EVENT_READ, self.E_STDOUT_READ
        )

        self.start_auth_timer(state, event, data)

    def end_sudo_stdio(self, state : int, event : int, data : Any) -> int:
        """ Start SUDO password on stdio """
        self.stop_timer(state, event, data)

        if self.stdout_key:
            self.select_loop.unregister(self.child.stdout)
            self.stdout_key = None

        os.set_blocking(self.child.stdout.fileno(), self.stdout_blocking)

    def sudo_stdio(self, _state : int, _event : int, _data : Any) -> int:
        """ SUDO authentication action method (on stdio) """
        try:
            data = self.child.stdout.read()
        except BlockingIOError:
            return None
        except:  # pylint: disable=bare-except
            self.reason = 'Read error'
            return self.Q_TERM

        self.collect_output(data, None)

        if not self.match_sudo_prompt(self.stdout):
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
            self.reason = str(error)
            return self.Q_TERM

        self.collect_output(stdout, stderr)
        self.status = self.child.returncode

        return self.Q_DONE

    def term_child(self, _state : int, _event : int, _data : Any) -> int:
        """ Terminate the child process """
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
        """ Kill the child process """
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

    def collect_output(self, stdout : bytes, stderr : bytes) -> None:
        """ Collect output """
        if stdout:
            if self.stdout:
                self.stdout.extend(stdout)
            else:
                self.stdout = bytearray(stdout)

        if stderr:
            if self.stderr:
                self.stderr.extend(stderr)
            else:
                self.stderr = bytearray(stderr)

    def close(self) -> None:
        """ Tear down event loop and close endpoints """
        self.disable_stop()

        super().close()

        if self.select_loop:
            self.select_loop.close()
            self.select_loop = None

if __name__ == '__main__':
    def main() -> None:
        """ Execute the command """
        with Command.create_command() as command:
            for signo in [signal.SIGINT, signal.SIGTERM]:
                signal.signal(signo, command.cancel)

            try:
                command.execute()
            except Exception as error:  # pylint: disable=broad-except
                raise
                sys.exit(error)

            print('STATUS:', command.status)
            print('REASON:', command.reason)

            if command.master and command.master.input_data:
                print('MASTER:')
                print(command.master.input_data.decode())

            if command.stdout and command.stdout:
                print('STDOUT:')
                print(command.stdout.decode())

            if command.stderr and command.stderr:
                print('STDERR:')
                print(command.stderr.decode())

        sys.exit(command.status)

    main()
