"""
This base class is used to manage events associated with I/O endpoints.  All
endpoints are implemented as non-blocking file descriptors, providing a reader
and a writer.  The reader and writer can be determined in one of three ways:

    1. Both file descriptors are explicitly specified.
    2. Only one file descriptor is explicitly specified, which is cloned to
       produce the other file descriptor.
    3. The reader and writer default to the two ends of a new pipe.

Note that option 2 is typically used to set up the master side of a pty and
option 3 is typically used to provide parent/child communication after a fork.

It is assumed that I/O endpoints will be used in some sort of event framework,
such as a select loop or asynchronous I/O.  Thus, this base class provides
abstract methods to "register" and "unregister" read and write events for the
endpoint file descriptors.  Since these events tend to be notifications of
readability or writeability, the derived events should call the endpoint read
or write methods when they fire.  Note that the derived register/unregister
methods must be robust enough to handle the cases when an existing event may or
may not be registered.  Also note that all read and written data must be of
type bytes.

The reader is implemented as its file descriptor, an input byte buffer, optional
read and EOF callbacks, and an optional opaque callback argument.  When the read
method is called, a low-level read method (os.read()) is called and any newly
read data is appended to the input buffer.  The input buffer will continue to
grow until the fetch_input() method is called to fetch all outstanding data and
reset the buffer to empty.  If there is new input data then the optional read
callback is called with the opaque argument.  If EOF is encountered (i.e.,
os_read() returns b''), then any read event is unregistered, the reader file
descriptor is closed (via os.close()), and the optional EOF callback is called
with the opaque callback argument.  Any read error is treated as EOF; however,
the corresponding error code and reason text are saved.  Note that EOF or an
error does not clear the input buffer.

The writer is implemented as its file descriptor, an output byte buffer, and an
optional write error callback.  The assumption is that write events are only
applicable if there is outstanding data in the output buffer to write.  Thus,
the write method will append any new output data to the output buffer and call a
low-level write method (os.write()) in an attempt to write all of the data.  If
all of the data is successfully written then any write event is unregistered and
the output buffer is set to empty.  Otherwise, the successfully written data is
removed from the output buffer and the output event is registered (just in case
it was not before).  If a write error is encountered then any write event is
unregistered, the writer file descriptor is closed (via os.close()), and the
optional write error callback is called with the opaque callback argument.  Note
that a write error does not clear the output buffer.

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
        *,
        reader : Optional[int] = None,
        writer : Optional[int] = None,
        read_size : Optional[int] = None,
        read_callback : Optional[Callback] = None,
        eof_callback : Optional[Callback] = None,
        write_error_callback : Optional[Callback] = None,
        callback_data : Optional[Any] = None
    ):
        """
        Initialize an endpoint

        Arguments:
            reader:
            writer:
                Reader and writer file descriptors.  See above for cases where
                none, one, or both are specified.
            read_size:
                Read size to use in all os.read() calls.
            read_callback:
                Callback for when new input data is read from the reader.
            eof_callback:
                Callback for when EOF (or a read error) occurs on the reader.
            write_error_callback:
                Callback for when a write error occurs.
            callback_data:
                Opaque data passed to read or EOF callback calls.
        """
        self.reader = reader
        self.read_size = read_size or self.DEFAULT_READ_SIZE
        self.input_data = None
        self.read_callback = read_callback
        self.eof_callback = eof_callback
        self.read_errno = None
        self.read_error_text = None

        self.writer = writer
        self.output_data = None
        self.write_error_callback = write_error_callback
        self.write_errno = None
        self.write_error_text = None

        self.callback_data = callback_data

        if self.reader is None and self.writer is None:
            self.reader, self.writer = os.pipe2(os.O_NONBLOCK)
        else:
            if self.reader is None and self.writer is not None:
                self.reader = os.dup(self.writer)
            elif self.reader is not None and self.writer is None:
                self.writer = os.dup(self.reader)
            os.set_blocking(self.reader, False)
            os.set_blocking(self.writer, False)

    def register_read(self) -> None:
        """ Register a read event (if not already registered) """

    def unregister_read(self) -> None:
        """ Unregister any active read event """

    def register_write(self) -> None:
        """ Register a write event (if not already registered) """

    def unregister_write(self) -> None:
        """ Unregister any active write event """

    def read(self) -> None:
        """ Read bytes and append to the current input data """
        if self.reader is None:
            self.write_only()
            return

        try:
            new_data = os.read(self.reader, self.read_size)
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
            if self.eof_callback:
                self.eof_callback(self.callback_data)
            self.write_only()
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
        if data:
            if self.output_data:
                self.output_data.extend(data)
            else:
                self.output_data = bytearray(data)

        if self.output_data:
            try:
                actual = os.write(self.writer, self.output_data)
            except BlockingIOError:
                actual = 0
            except OSError as error:
                self.write_errno = error.errno
                self.write_error_text = error.strerror
                if self.write_error_callback:
                    self.write_error_callback(self.callback_data)
                self.read_only()
                return
            except Exception as error:  # pylint: disable=broad-except
                self.write_errno = EIO
                self.write_error_text = str(error)
                if self.write_error_callback:
                    self.write_error_callback(self.callback_data)
                self.read_only()
                return
        else:
            self.output_data = None
            self.unregister_write()
            return

        if actual >= len(self.output_data):
            self.output_data = None
            self.unregister_write()
        else:
            self.output_data[:] = self.output_data[actual:]
            self.register_write()

    def read_only(self) -> None:
        """ Cancel any write event and close the writer """
        if self.writer:
            self.unregister_write()
            os.close(self.writer)
            self.writer = None

    def write_only(self) -> None:
        """ Cancel and read event and close the reader """
        if self.reader:
            self.unregister_read()
            os.close(self.reader)
            self.reader = None

    @property
    def is_reading(self) -> bool:
        """ Check for an open reader """
        return self.reader is not None

    @property
    def is_writing(self) -> bool:
        """ Check for an open writer """
        return self.writer is not None

    def fetch_input(self) -> bytes:
        """ Fetch outstanding input data and clear input buffer """
        data = bytes(self.input_data) if self.input_data else None
        self.input_data = None
        return data

    @property
    def read_error(self) -> tuple[int, str]:
        """ Get any read error """
        if self.read_errno is None:
            error = None
        else:
            error = (self.read_errno, self.read_error_text)
        return error

    @property
    def write_error(self) -> tuple[int, str]:
        """ Get any write error """
        if self.write_errno is None:
            error = None
        else:
            error = (self.write_errno, self.write_error_text)
        return error

    def close(self) -> None:
        """ Clean up the endpoint """
        self.read_only()
        self.write_only()

    def __enter__(self):
        """ Return self """
        return self

    def __exit__(self, *args, **kwargs):
        """ Close the reader and writer """
        self.close()
