"""
This class extends the I/O endpoint class to add support for asynchronous I/O
events.  Events are implemented using an event callback and opaque read and
write event callback data values.  Registering a read or write event adds the
event to the currently running asyncio event loop.  When the event fires, the
event callback is called with the appropriate callback data value.  Thus, the
data values can be used to identify specific events in the event loop.  Note
that the event loop must be allowed to run in order for this to work.  Thus,
the intended implementation is that the event callback will set a future on
which the caller is awaiting to the callback data value.
"""

import asyncio
from typing import Any, Optional

from events.io_endpoint import IOEndpoint

class AIOEndpoint(IOEndpoint):
    """ Manage a Selectable I/O Endpoint """
    event_callback : IOEndpoint.Callback
    read_data : Any
    write_data : Any
    read_active : bool
    write_active : bool
    loop : asyncio.AbstractEventLoop

    def __init__(
        self,
        event_callback : IOEndpoint.Callback,
        read_data : Optional[Any] = None,
        write_data : Optional[Any] = None,
        **kwargs
    ):
        """
        Initialize an AIO endpoint

        Arguments:
            event_callback:
                Callback to handle read or write event.
            read_data:
                Opaque callback data for a read event.
            write_data:
                Opaque callback data for a write event.
            kwargs:
                Other keyword arguments for IOEndpoint().
        """
        super().__init__(**kwargs)

        self.event_callback = event_callback
        self.read_data = read_data
        self.write_data = write_data
        self.read_active = False
        self.write_active = False
        self.loop = asyncio.get_running_loop()

    def register_read(self) -> None:
        """ Register for read events """
        if self.reader and not self.read_active:
            self.loop.add_reader(
                self.reader, self.event_callback, self.read_data
            )
            self.read_active = True

    def unregister_read(self) -> None:
        """ Unregister read events """
        if self.reader and self.read_active:
            self.loop.remove_reader(self.reader)
            self.read_active = False

    def register_write(self) -> None:
        """ Register for write events """
        if self.writer and not self.write_active:
            self.loop.add_writer(
                self.writer, self.event_callback, self.write_data
            )
            self.write_active = True

    def unregister_write(self) -> None:
        """ Unregister write events """
        if self.writer and self.write_active:
            self.loop.remove_writer(self.writer)
            self.write_active = False
