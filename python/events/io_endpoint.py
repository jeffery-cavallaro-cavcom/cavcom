"""
This base class is used to manage events associated with I/O endpoints.  All
endpoints are implemented as non-blocking file descriptors and all I/O is
performed using os.read(), os.write, and os.close().

It is assumed that I/O endpoints will be used in some sort of event framework,
such as a select loop or asynchronous I/O.  Thus, this base class provides
abstract methods to "register" and "unregister" read and write events for the
endpoint file descriptors.  Since these events tend to be notifications of
readability or writeability, the derived events should call the endpoint read
or write methods when they fire.  Note that the derived register/unregister
methods must be robust enough to handle the cases when an existing event may or
may not be registered.  Also note that all read and written data must be of
type bytes.

When the read() method is called, os.read() is called on the underlying file
descriptor and any newly read data is appended to the input buffer.  The input
buffer will continue to grow until the fetch_input() method is called, which
returns all oustanding input data and resets the input buffer to empty.  If
there is new input data then the optional read callback is called with the
opaque callback argument.  If EOF is encountered then any read event is
unregistered, the file descriptor is closed, and the optional EOF callback is
called with the opaque callback argument.  All read errors (except
BlockingIOError) are treated as EOF; however, the corresponding error code and
reason text are saved.  Note that EOF or an error does not clear the input
buffer.

The assumption is that write events are only applicable when there is
outstanding data in the output buffer to write.  Thus, the write() method will
append any new output data to the output buffer and call os.write() in an
attempt to write all of the data.  If all of the data is successfully written
then any write event is unregistered and the output buffer is set to empty.
Otherwise, the successfully written data is removed from the output buffer and
the output event is registered (just in case it was not before).  If a write
error is encountered then any write event is unregistered, the file descriptor
is closed, and the optional write error callback is called with the opaque
callback argument.  Note that a write error does not clear the output buffer.

Note that the overall strategy is to not raise any exceptions; all errors can
be gleaned from the saved error codes and reason texts.
"""

from errno import EIO
import os
from typing import Any, Callable, ClassVar, Optional

class IOEndpoint:
    """ Manage an I/O Endpoint """
    # pylint: disable=too-many-instance-attributes
    DEFAULT_READ_SIZE : ClassVar[int] = 1024*1024  # 1Mb

    Callback = Callable[[Any], None]

    reader : int
    read_size : int
    input_data : bytearray
    read_callback : Callback
    eof_callback : Callback
    read_errno : int
    read_error_text : str

    writer : int
    output_data : bytearray
    write_error_callback : Callback
    write_errno : int
    write_error_text : str

    callback_data : Any

    def __init__(
        # pylint: disable=too-many-arguments
        self,
        fd : int,
        *,
        read_size : Optional[int] = DEFAULT_READ_SIZE,
        read_callback : Optional[Callback] = None,
        eof_callback : Optional[Callback] = None,
        write_error_callback : Optional[Callback] = None,
        callback_data : Optional[Any] = None
    ):
        """
        Initialize an endpoint

        Arguments:
            fd:
                Open file descriptor (>=0) for the target I/O endpoint.  Note
                that the descriptor type is not checked.  So, for example, if
                the descriptor represents the read end of a pipe then no write
                operations should be performed.  The descriptor is set to
                non-blocking mode and the new I/O endpoint instance assumes
                ownership of the descriptor.
            read_size:
                Read size to use in all os.read() calls.
            read_callback:
                Callback to call when new input data is read.
            eof_callback:
                Callback to call when EOF (or a read error) occurs.
            write_error_callback:
                Callback to call when a write error occurs.
            callback_data:
                Opaque data passed to read, EOF, and write error callback calls.
        """
        self.fd = fd
        os.set_blocking(self.fd, False)

        self.read_size = read_size or self.DEFAULT_READ_SIZE
        self.input_data = None
        self.read_callback = read_callback
        self.eof_callback = eof_callback
        self.read_errno = None
        self.read_error_text = None

        self.output_data = None
        self.write_error_callback = write_error_callback
        self.write_errno = None
        self.write_error_text = None

        self.callback_data = callback_data

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
        return self.fd is not None and self.fd >= 0

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
            self.read_errno = error.errno
            self.read_error_text = error.strerror
            new_data = b''
        except Exception as error:  # pylint: disable=broad-except
            self.read_errno = EIO
            self.read_error_text = str(error)
            new_data = b''

        if not new_data:
            self.close()
            if self.eof_callback:
                self.eof_callback(self.callback_data)
            return

        if self.input_data:
            self.input_data.extend(new_data)
        else:
            self.input_data = bytearray(new_data)

        if self.read_callback:
            self.read_callback(self.callback_data)

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
            if self.output_data:
                self.output_data.extend(data)
            else:
                self.output_data = bytearray(data)

        if not self.output_data:
            self.output_data = None
            self.unregister_write()
            return

        try:
            actual = os.write(self.fd, self.output_data)
        except BlockingIOError:
            actual = 0
        except OSError as error:
            self.close()
            self.write_errno = error.errno
            self.write_error_text = error.strerror
            if self.write_error_callback:
                self.write_error_callback(self.callback_data)
            return
        except Exception as error:  # pylint: disable=broad-except
            self.close()
            self.write_errno = EIO
            self.write_error_text = str(error)
            if self.write_error_callback:
                self.write_error_callback(self.callback_data)
            return

        if actual >= len(self.output_data):
            self.output_data = None
            self.unregister_write()
        else:
            self.output_data[:] = self.output_data[actual:]
            self.register_write()

    def fetch_input(self) -> bytes:
        """ Fetch outstanding input data and clear input buffer """
        data = bytes(self.input_data) if self.input_data else None
        self.input_data = None
        return data

    def close(self) -> None:
        """ Clean up the endpoint """
        if self.is_open:
            self.unregister_read()
            self.unregister_write()
            os.close(self.fd)
            self.fd = None

    def __enter__(self):
        """ Return self """
        return self

    def __exit__(self, *args, **kwargs):
        """ Close the endpoint """
        self.close()
