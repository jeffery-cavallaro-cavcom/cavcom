"""
This class extends the I/O endpoint class to add support for select loop read
and write events.  It supports opaque read and write event data values that are
included in the fired event select keys.  These values can be used to identify
specific events in the event loop.
"""

from selectors import BaseSelector, SelectorKey, EVENT_READ, EVENT_WRITE
from typing import Any, Optional

from events.io_endpoint import IOEndpoint

class SelectEndpoint(IOEndpoint):
    """ Manage a Selectable I/O Endpoint """
    selectors : BaseSelector
    reader_key : SelectorKey
    writer_key : SelectorKey
    read_data : Any
    write_data : Any

    def __init__(
        self,
        selectors : BaseSelector,
        read_data : Optional[Any] = None,
        write_data : Optional[Any] = None,
        **kwargs
    ):
        """
        Initialize a selectable endpoint

        Arguments:
            selectors:
                Selectors used in target select loop.
            read_data:
                Opaque data to return with read events.
            write_data:
                Opaque data to return with write events.
            kwargs:
                Other keyword arguments for IOEndpoint().
        """
        super().__init__(**kwargs)

        self.selectors = selectors
        self.reader_key = None
        self.writer_key = None
        self.read_data = read_data
        self.write_data = write_data

    def register_read(self) -> None:
        """ Register for read events """
        if self.reader and not self.reader_key:
            self.reader_key = self.selectors.register(
                self.reader, EVENT_READ, self.read_data
            )

    def unregister_read(self) -> None:
        """ Unregister read events """
        if self.reader_key:
            self.selectors.unregister(self.reader_key.fileobj)
            self.reader_key = None

    def register_write(self) -> None:
        """ Register for write events """
        if self.writer and not self.writer_key:
            self.writer_key = self.selectors.register(
                self.writer, EVENT_WRITE, self.write_data
            )

    def unregister_write(self) -> None:
        """ Unregister write events """
        if self.writer_key:
            self.selectors.unregister(self.writer_key.fileobj)
            self.writer_key = None
