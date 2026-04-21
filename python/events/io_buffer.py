"""
This class is used to accumulate raw data from I/O operations.  Bytes data is
appended to a buffer until a request is made to fetch and reset the buffer.
Fetched data can be bytes or decoded to text.  If a unicode decode error occurs
at the of the buffer then it is assumed that a unicode sequence is currently
incomplete and will be completed later, so only the valid text up to the error
position is returned.  A unicode decode error anywhere else is assumed to be a
real error.
"""

from typing import Optional

class IOBuffer:
    """ Accumulate Raw Data """
    buffer : bytearray

    def __init__(self):
        """ Create and empty buffer """
        self.buffer = bytearray()

    def append(self, data : bytes) -> None:
        """
        Append data to the buffer

        Arguments:
            data:
                New data to append.
        """
        self.buffer.extend(data)

    def reset(self, count : Optional[int] = None) -> None:
        """
        Reset the buffer

        Arguments:
            count:
                If specified then a maximum of this many bytes are removed from
                the start of the buffer.  Otherwise, the buffer is reset to
                empty.
        """
        if count is None:
            self.buffer = bytearray()
        else:
            self.buffer = self.buffer[count:]

    def __len__(self) -> int:
        """ Get number of bytes in buffer """
        return len(self.buffer)

    def fetch_bytes(self, reset : Optional[bool] = False) -> bytes:
        """
        Extract data from the buffer

        Arguments:
            reset:
                Remove the extracted data from the buffer.

        Returns:
            Extracted bytes or None if the buffer is empty.
        """
        if not self.buffer:
            return None

        data = bytes(self.buffer)
        if reset:
            self.reset()

        return data

    def fetch_text(self, reset : Optional[bool] = False) -> str:
        """
        Extract data from the buffer

        Arguments:
            reset:
                Remove the extracted text from the buffer.

        Returns:
            Extracted decoded text.  If a decode error occurs at the end of the
            buffer then the valid text up to the error is returned and the
            incomplete sequence is left in the buffer.
        """
        if not self.buffer:
            return None

        try:
            text = self.buffer.decode()
            if reset:
                self.reset()
        except UnicodeDecodeError as error:
            if error.end < len(self.buffer):
                raise
            text = self.buffer[:error.start].decode()
            if reset:
                self.buffer = self.buffer[error.start:]

        return text or None
