"""
This class implements an asynchronous event loop based on a Future and a list
of pending event IDs.  Note that duplicate pending events are folded into a
single event and event delivery order is not guaranteed.
"""

import asyncio
from typing import Any, Optional

class AEventLoop:
    """ Asynchronous Event Loop """
    loop : asyncio.AbstractEventLoop
    event_wait : asyncio.Future
    next_events : set[Any]

    def __init__(self):
        """ Initialize and empty event loop """
        self.loop = asyncio.get_running_loop()
        self.event_wait = self.loop.create_future()
        self.next_events = set()

    def add_event(self, event_data : Any) -> None:
        """
        Add a pending event

        Arguments:
            event_data:
                Data corresponding to the new pending event.  This should not be
                None, since it will look like a timeout, but this is not
                enforced.
        """
        self.next_events.add(event_data)

        if not self.event_wait.done():
            self.event_wait.set_result(self.next_events)

    async def wait_for_event(self, timeout : Optional[float] = None) -> Any:
        """
        Wait for and return the next event

        Arguments:
            timeout:
                Event wait timeout (seconds).

        Returns:
            Data corresponding to next event, or None for timeout.
        """
        while True:
            if self.next_events:
                return self.next_events.pop()
            if self.event_wait.done():
                self.event_wait = self.loop.create_future()
            try:
                await asyncio.wait_for(self.event_wait, timeout=timeout)
            except TimeoutError:
                return None
