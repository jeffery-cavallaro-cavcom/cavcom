"""
This class implements an asynchronous timer event based on the asyncio
call_later() method and the corresponding TimerHandle.
"""

import asyncio
from typing import Any, Callable, Optional

class TimerEvent:
    """ Signal Events """
    Callback = Callable[[Any], None]

    duration : float
    event_callback : Callback
    event_data : Any
    loop : asyncio.AbstractEventLoop
    timer_handle : asyncio.TimerHandle

    def __init__(
        self,
        duration : float,
        event_callback : Callback,
        event_data : Optional[Any] = None
    ):
        """
        Create and register a new timer event

        Arguments:
            duration:
                Timeout duration, in seconds.  A value <= 0 will fire
                immediately.  A value of None will never fire.
            event_callback:
                Callback to be called when the timer expires.
            event_data:
                Opaque event data that is passed to the callback.  This is
                normally an event ID.
        """
        self.duration = duration
        self.event_callback = event_callback
        self.event_data = event_data
        self.loop = asyncio.get_running_loop()
        self.timer_handle = None

    def start(self) -> None:
        """ Start the timer """
        self.cancel()
        self.timer_handle = self.loop.call_later(
            self.duration, self.event_callback, self.event_data
        )

    def cancel(self) -> None:
        """ Cancel the timer """
        if self.timer_handle:
            self.timer_handle.cancel()
            self.timer_handle = None

    def close(self) -> None:
        """ Unregister the timer event """
        self.cancel()
