"""
This class extends the event handler class for select loop signal events.  The
event trigger() method is registered with a signal handler dispatcher.
"""

from selectors import DefaultSelector
from threading import Timer
from typing import Any, Optional

from events.event_handler import EventHandler

class TimerEvent(EventHandler):
    """ Signal Events """
    duration : float
    timer : Timer

    def __init__(
        self,
        duration : float,
        selectors : DefaultSelector,
        event_data : Optional[Any] = None
    ):
        """
        Create and register a new timer event

        Arguments:
            duration:
                Timeout duration, in seconds.  A value <= 0 will fire
                immediately.  A value of None will never fire.
            selectors:
                Target select loop to which the event is registered.
            data:
                Opaque event data that is included in the triggered event's
                SelectKey.  This is normally an event ID.
        """
        super().__init__(selectors, event_data=event_data)
        self.duration = duration
        self.timer = None

    def start(self) -> None:
        """ Start the timer """
        self.cancel()
        self.timer = Timer(self.duration, self.trigger)
        self.timer.start()

    def cancel(self) -> None:
        """ Cancel the timer """
        if self.timer:
            self.timer.cancel()
            self.timer = None

    def close(self) -> None:
        """ Unregister the timer event """
        self.cancel()
        super().close()
