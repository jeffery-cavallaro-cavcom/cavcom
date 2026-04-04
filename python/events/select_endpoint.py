"""
This class extends the I/O endpoint class to add support for select loop read
and write events.  Since each file descriptor can only be registered once with
a select loop, the code handles updating the registration depending on whether a
read event, write event, both, or neither is desired.  Opaque read and write
data values are supported and both are returned as a tuple in a fired event
select key; a get_events() method is provided to determine which data value(s)
to use based on the read/write indicator for a fired event.  These opaque data
values can be used to identify specific events in the event loop.
"""

from selectors import BaseSelector, SelectorKey, EVENT_READ, EVENT_WRITE
from typing import Any, Optional

from events.io_endpoint import IOEndpoint

class SelectEndpoint(IOEndpoint):
    """ Manage a Selectable I/O Endpoint """
    selectors : BaseSelector
    event_data = tuple[Any, Any]
    reading : bool
    writing : bool
    select_key : SelectorKey

    def __init__(
        self,
        selectors : BaseSelector,
        fd : int,
        *,
        read_data : Optional[Any] = None,
        write_data : Optional[Any] = None,
        **kwargs
    ):
        """
        Initialize a selectable endpoint

        Arguments:
            selectors:
                Selectors used in target select loop.
            fd:
                Open file descriptor (>=0) for the target I/O endpoint.
            read_data:
                Opaque data to return with read events.
            write_data:
                Opaque data to return with write events.
            kwargs:
                Other keyword arguments for IOEndpoint().
        """
        super().__init__(fd, **kwargs)

        self.selectors = selectors
        self.event_data = (read_data, write_data)
        self.reading = False
        self.writing = False
        self.select_key = None

    def register_read(self) -> None:
        """ Register for read events """
        if not self.reading:
            self.reading = True
            self.update_events()

    def unregister_read(self) -> None:
        """ Unregister read events """
        if self.reading:
            self.reading = False
            self.update_events()

    def register_write(self) -> None:
        """ Register for write events """
        if not self.writing:
            self.writing = True
            self.update_events()

    def unregister_write(self) -> None:
        """ Unregister write events """
        if self.writing:
            self.writing = False
            self.update_events()

    @staticmethod
    def get_events(select_key : SelectorKey, events : int) -> list[Any]:
        """
        Get next events

        Arguments;
            select_key:
                Select key for fired event(s).
            events:
                Occurring event mask.
        
        Returns:
            List of corresponding event data for occurring events, with
            precedence for the write event.
        """
        fired = []

        if events & EVENT_WRITE:
            fired.append(select_key.data[1])

        if events & EVENT_READ:
            fired.append(select_key.data[0])

        return fired

    def update_events(self) -> None:
        """ Update the desired registered events """
        events = 0

        if self.reading:
            events |= EVENT_READ
        if self.writing:
            events |= EVENT_WRITE

        if not events and self.select_key:
            self.selectors.unregister(self.fd)
            self.select_key = None
        elif self.select_key:
            self.select_key = self.selectors.modify(
                self.fd, events, self.event_data
            )
        else:
            self.select_key = self.selectors.register(
                self.fd, events, self.event_data
            )
