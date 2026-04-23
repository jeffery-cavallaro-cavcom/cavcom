"""
This class extends the I/O endpoint base class to add support for asynchronous
I/O events.  Events are implemented using an event callback and the opaque read
and write data values.  Registering a read or write event adds the event to the
currently running asyncio event loop.  When the event fires, the event callback
is called with the appropriate opaque read or write data value.  Note that the
event loop must be allowed to run in order for this to work; the intended
implementation is that the event callback will set a future on which the caller
is awaiting.
"""

import asyncio
from typing import Any, Callable, Optional

from events.io_endpoint import IOEndpoint

class AIOEndpoint(IOEndpoint):
    """ Manage an Asynchronous I/O Endpoint """
    Callback = Callable[[Any], None]

    loop : asyncio.AbstractEventLoop
    event_callback : Callback
    read_active : bool
    write_active : bool

    def __init__(
        self,
        fd : int,
        *,
        event_callback : Optional[Callback] = None,
        **kwargs
    ):
        """
        Initialize an AIO endpoint

        Arguments:
            fd:
                Open file descriptor (>=0) for the target I/O endpoint.
            event_callback:
                Callback to handle read and write events.
            kwargs:
                Other keyword arguments for IOEndpoint().
        """
        super().__init__(fd, **kwargs)

        self.loop = asyncio.get_running_loop()
        self.event_callback = event_callback
        self.read_active = False
        self.write_active = False

    def register_read(self) -> None:
        """ Register for read events """
        if self.is_open and not self.read_active and self.event_callback:
            self.loop.add_reader(self.fd, self.event_callback, self.read_data)
            self.read_active = True

    def unregister_read(self) -> None:
        """ Unregister read events """
        if self.read_active:
            self.loop.remove_reader(self.fd)
            self.read_active = False

    def register_write(self) -> None:
        """ Register for write events """
        if self.is_open and not self.write_active and self.event_callback:
            self.loop.add_writer(self.fd, self.event_callback, self.write_data)
            self.write_active = True

    def unregister_write(self) -> None:
        """ Unregister write events """
        if self.write_active:
            self.loop.remove_writer(self.fd)
            self.write_active = False
