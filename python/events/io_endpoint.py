"""
This base class is used to manage events associated with I/O endpoints.  All
endpoints are implemented as non-blocking file descriptors and all I/O is
performed using os.read(), os.write, and os.close().

It is assumed that I/O endpoints are used in some sort of an event loop
framework, such as a select loop or asynchronous I/O.  Thus, this base class
provides abstract methods to "register" and "unregister" read and write events
for the endpoint file descriptor per the target event loop.  Since these events
tend to be notifications of readability or writeability, the target event loop
should call the endpoint read or write methods when the corresponding events
occur.  Note that the derived register/unregister methods must be robust enough
to handle the cases when an existing event may or may not be registered.  Also
note that all read and written data must be of type bytes.

When the read() method is called, os.read() is called on the underlying file
descriptor and any newly read data is appended to the input buffer.  The input
buffer will continue to grow until the fetch_input() method is called with a
reset value of True.  If EOF or a read error is encountered then any read event
is unregistered, the file descriptor is closed, and the read error is recorded
in the errno and error_text attributes.  Note that EOF or an error does not
clear the input buffer.

The assumption is that write events are only applicable when there is
outstanding data in the output buffer to write.  Thus, the write() method will
append any new output data to the output buffer and call os.write() in an
attempt to write all of the data.  If all of the data is successfully written
then any write event is unregistered and the output buffer is set to empty.
Otherwise, the successfully written data is removed from the output buffer and
the output event is registered (just in case it was not before).  If a write
error is encountered then any write event is unregistered, the file descriptor
is closed, and the write error is recorded in the errno and error_text
attributes.  Note that an error does not clear the output buffer.

Note that the overall strategy is to not raise any exceptions; all errors can
be gleaned from the saved error codes and reason texts.
"""

from errno import EIO
import os
from typing import Any, ClassVar, Optional, Union

from events.io_buffer import IOBuffer

class IOEndpoint:
    """ Manage an I/O Endpoint """
    # pylint: disable=too-many-instance-attributes
    DEFAULT_READ_SIZE : ClassVar[int] = 1024*1024  # 1Mb

    fd : int
    no_close : bool
    read_size : int
    input_data : IOBuffer
    output_data : IOBuffer
    read_data : Any
    write_data : Any
    errno : int
    error_text : str

    def __init__(
        # pylint: disable=too-many-arguments
        self,
        fd : int,
        *,
        no_close : Optional[bool] = False,
        read_size : Optional[int] = DEFAULT_READ_SIZE,
        read_data : Optional[Any] = None,
        write_data : Optional[Any] = None
    ):
        """
        Initialize an endpoint

        Arguments:
            fd:
                Open file descriptor (>=0) for the target I/O endpoint.  Note
                that the descriptor type is not checked.  So, for example, if
                the descriptor represents the read end of a pipe then no write
                operations should be performed.  If there is no actual file
                descriptor involved (e.g., buffering only) then a value of
                None can be specified.
            no_close:
                If False then the I/O endpoint assumes ownership of the file
                descriptor and the close() method will close it.  Otherwise,
                the close() method does not close the file descriptor.
            read_size:
                Read size to use in all os.read() calls.
            read_data:
            write_data:
                Opaque data corresponding the read and write events.  These
                values are for use by the derived register/unregister methods
                and are usually used to represent event IDs.
        """
        self.fd = fd
        if self.fd is not None:
            os.set_blocking(self.fd, False)

        self.no_close = bool(no_close)
        self.read_size = read_size or self.DEFAULT_READ_SIZE
        self.input_data = IOBuffer()
        self.output_data = IOBuffer()
        self.read_data = read_data
        self.write_data = write_data
        self.errno = None
        self.error_text = None

    def register_read(self) -> None:
        """ Register a read event (if not already registered) """

    def unregister_read(self) -> None:
        """ Unregister any active read event """

    def register_write(self) -> None:
        """ Register a write event (if not already registered) """

    def unregister_write(self) -> None:
        """ Unregister any active write event """

    @property
    def is_open(self) -> bool:
        """ Check for an open file descriptor """
        return self.fd is not None

    def read(self) -> None:
        """ Read bytes and append to the current input data """
        if not self.is_open:
            return

        try:
            new_data = os.read(self.fd, self.read_size)
        except BlockingIOError:
            return
        # All other errors are treated as EOF.
        except OSError as error:
            self.errno = error.errno
            self.error_text = error.strerror
            new_data = b''
        except Exception as error:  # pylint: disable=broad-except
            self.errno = EIO
            self.error_text = str(error)
            new_data = b''

        if not new_data:
            self.close()
            return

        self.input_data.append(new_data)

    def write(self, data : Optional[bytes] = None) -> None:
        """
        Write outstanding and new data

        Arguments:
            data:
                Data to add to the pending output data.  An attempt will be
                made to write the old and any new data.
        """
        if not self.is_open:
            return

        if data:
            self.output_data.append(data)

        if len(self.output_data) <= 0:
            self.unregister_write()
            return

        try:
            actual = os.write(self.fd, self.output_data.fetch_bytes())
        except BlockingIOError:
            actual = 0
        except OSError as error:
            self.close()
            self.errno = error.errno
            self.error_text = error.strerror
            return
        except Exception as error:  # pylint: disable=broad-except
            self.close()
            self.errno = EIO
            self.error_text = str(error)
            return

        if actual >= len(self.output_data):
            self.output_data.reset()
            self.unregister_write()
        else:
            self.output_data.reset(count=actual)
            self.register_write()

    def fetch_input(
        self, text : Optional[bool] = False, reset : Optional[bool] = False
    ) -> Union[bytes, str]:
        """
        Fetch outstanding input data and clear input buffer

        Arguments:
            text:
                If True then return decoded text.
            reset:
                If True then any returned data is flushed from the buffer.

        Returns:
            Outstanding bytes or decoded text.
        """
        if text:
            data = self.input_data.fetch_text(reset=reset)
        else:
            data = self.input_data.fetch_bytes(reset=reset)

        return data

    def close(self) -> None:
        """ Close the endpoint """
        if not self.is_open:
            return

        self.unregister_read()
        self.unregister_write()

        if not self.no_close:
            os.close(self.fd)

        self.fd = None

    def __enter__(self):
        """ Return self """
        return self

    def __exit__(self, *args, **kwargs):
        """ Close the endpoint """
        self.close()
