"""
This class is used to inject non-file I/O events (e.g., signals and timers) into
select loops using eventfd(2) event file descriptors.  The event generator calls
the trigger() method when an event occurs.  The select loop must call the
acknowledge() method once it picks up an event.  The close() method must be
called when an event handler instance is no longer needed in order to prevent
file descriptor leakage.  Note that semaphore behavior is not enabled.  Thus,
multiple triggers result in a single acknowledged event.
"""

import os
from selectors import DefaultSelector, EVENT_READ
from typing import Any, Optional

class EventHandler:
    """ Non-file I/O Events """
    selectors : DefaultSelector
    event_data : Any
    event_fd : int

    def __init__(
        self, selectors : DefaultSelector, data : Optional[Any] = None
    ):
        """
        Create and register a new event handler

        Arguments:
            selectors:
                Target select loop to which the event is registered.
            data:
                Opaque event data that is included in the triggered event's
                SelectKey.  This is normally an event ID.
        """
        self.selectors = selectors
        self.event_data = data
        self.event_fd = os.eventfd(0, os.EFD_NONBLOCK)
        self.selectors.register(self.event_fd, EVENT_READ, data=self.event_data)

    def trigger(self) -> None:
        """ Trigger an event """
        os.eventfd_write(self.event_fd, 1)

    def acknowledge(self) -> bool:
        """
        Acknowledge an event

        Returns:
            True if there really was a pending event.
        """
        try:
            os.eventfd_read(self.event_fd)
            pending = True
        except BlockingIOError:
            pending = False

        return pending

    def close(self) -> None:
        """ Unregister and close the event """
        self.selectors.unregister(self.event_fd)
        os.close(self.event_fd)
