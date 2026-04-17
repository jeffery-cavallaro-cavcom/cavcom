"""
This class extends the event handler class for select loop signal events.
"""

from selectors import DefaultSelector
from signal import signal, Handlers
from types import FrameType
from typing import Any, Callable, Optional, Union

from events.event_handler import EventHandler

class SignalEvent(EventHandler):
    """ Signal Events """
    Callback = Callable[[int, FrameType], None]

    signo : int
    old_handler : Union[Callback, Handlers]

    def __init__(
        self,
        signo : int,
        selectors : DefaultSelector,
        event_data : Optional[Any] = None
    ):
        """
        Create and register a new signal event

        Arguments:
            signo:
                Target signal number.
            selectors:
                Target select loop to which the event is registered.
            event_data:
                Opaque event data that is included in the triggered event's
                SelectKey.  This is normally an event ID.
        """
        super().__init__(selectors, event_data=event_data)

        self.signo = signo
        self.old_handler = signal(self.signo, self.trigger_signal)

    def trigger_signal(self, _signo : int, _frame : FrameType) -> None:
        """ Match call signature """
        self.trigger()
        return True  # never auto-deregister

    def close(self) -> None:
        """ Unregister signal event """
        signal(self.signo, self.old_handler)
        super().close()
