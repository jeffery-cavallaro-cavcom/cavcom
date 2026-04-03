""" I/O Endpoint Unit Tests """

import unittest

from errno import EIO
import os
from typing import Any, ClassVar, Optional

from events.io_endpoint import IOEndpoint
from events.pty_manager import PTYManager

class IOEndpointForTest(IOEndpoint):
    """ Record method calls """
    read_called : int
    eof_called : int
    error_called : int
    received_data : Any

    read_registered : bool
    write_registered : bool

    def __init__(self, fd : int, *, callback_data : Optional[Any] = None):
        """ Initialize call indicators """
        super().__init__(
            fd,
            read_callback=self.count_read,
            eof_callback=self.count_eof,
            write_error_callback=self.count_error,
            callback_data=callback_data
        )

        self.read_called = 0
        self.eof_called = 0
        self.error_called = 0
        self.received_data = None
        self.read_registered = False
        self.write_registered = False

    def count_read(self, data : Optional[Any] = None) -> None:
        """ Increment read count """
        self.read_called += 1
        self.received_data = data

    def count_eof(self, data : Optional[Any] = None) -> None:
        """ Increment EOF count """
        self.eof_called += 1
        self.received_data = data

    def count_error(self, data : Optional[Any] = None) -> None:
        """ Increment write error count """
        self.error_called += 1
        self.received_data = data

    def register_read(self) -> None:
        """ Indicate read events registered """
        self.read_registered = True

    def unregister_read(self) -> None:
        """ Indicate read events unregistered """
        self.read_registered = False

    def register_write(self) -> None:
        """ Indicate write events registered """
        self.write_registered = True

    def unregister_write(self) -> None:
        """ Indicate write events unregistered """
        self.write_registered = False

class TestIOEndpoint(unittest.TestCase):
    """ I/O Endpoint Unit Tests """
    TEST_DATA_1 : ClassVar[bytes] = b"Hello, World!\n"
    TEST_DATA_2 : ClassVar[bytes] = b"The answer is 42\n"
    CALLBACK_DATA : ClassVar[int] = 42

    def test_pipe(self):
        """ Create and use a pipe """
        # pylint: disable=too-many-statements
        reader_side, writer_side = os.pipe()

        try:
            reader = IOEndpointForTest(
                reader_side, callback_data=self.CALLBACK_DATA
            )
            writer = IOEndpointForTest(
                writer_side, callback_data=self.CALLBACK_DATA
            )

            self.assertTrue(reader.is_open)
            self.assertTrue(writer.is_open)

            reader.register_read()
            writer.register_write()
            self.assertTrue(reader.read_registered)
            self.assertTrue(writer.write_registered)

            # Standard read/write.
            reader.received_data = None
            writer.write(self.TEST_DATA_1)
            reader.read()
            data = reader.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            self.assertTrue(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.input_data)
            self.assertIsNone(writer.output_data)
            self.assertEqual(reader.read_called, 1)
            self.assertEqual(reader.eof_called, 0)
            self.assertEqual(reader.error_called, 0)
            self.assertEqual(reader.received_data, self.CALLBACK_DATA)
            self.assertIsNone(reader.read_errno)
            self.assertIsNone(writer.write_errno)

            # Cumulative read/write.
            reader.received_data = None
            writer.write(self.TEST_DATA_2[:5])
            reader.read()
            writer.write(self.TEST_DATA_2[5:])
            reader.read()
            data = reader.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            self.assertTrue(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.input_data)
            self.assertIsNone(writer.output_data)
            # No buffering on a pipe so called twice!
            self.assertEqual(reader.read_called, 3)
            self.assertEqual(reader.eof_called, 0)
            self.assertEqual(writer.error_called, 0)
            self.assertEqual(reader.received_data, self.CALLBACK_DATA)
            self.assertIsNone(reader.read_errno)
            self.assertIsNone(writer.write_errno)

            # EOF.
            reader.received_data = None
            writer.close()
            reader.read()
            data = reader.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(reader.is_open)
            self.assertFalse(writer.is_open)
            self.assertFalse(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.input_data)
            self.assertIsNone(writer.output_data)
            self.assertEqual(reader.read_called, 3)
            self.assertEqual(reader.eof_called, 1)
            self.assertEqual(reader.error_called, 0)
            self.assertEqual(reader.received_data, self.CALLBACK_DATA)
            self.assertIsNone(reader.read_errno)
            self.assertIsNone(writer.write_errno)
        finally:
            reader.close()
            writer.close()

        self.assertFalse(reader.is_open)
        self.assertFalse(writer.is_open)

    def test_pty(self):
        """ Allocate and use a PTY """
        # pylint: disable=too-many-statements
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = IOEndpointForTest(
                pty.master, callback_data=self.CALLBACK_DATA
            )
            slave = IOEndpointForTest(
                pty.slave, callback_data=self.CALLBACK_DATA
            )

            self.assertTrue(master.is_open)
            self.assertTrue(slave.is_open)

            master.register_read()
            master.register_write()
            slave.register_read()
            slave.register_write()
            self.assertTrue(master.read_registered)
            self.assertTrue(master.write_registered)
            self.assertTrue(slave.read_registered)
            self.assertTrue(slave.write_registered)

            # Read/write.
            master.received_data = None
            slave.received_data = None
            master.write(self.TEST_DATA_1)
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            self.assertTrue(master.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_called, 0)
            self.assertEqual(master.eof_called, 0)
            self.assertEqual(master.error_called, 0)
            self.assertIsNone(master.received_data)
            self.assertIsNone(master.read_errno)
            self.assertIsNone(master.write_errno)

            self.assertTrue(slave.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(slave.input_data)
            self.assertIsNone(slave.output_data)
            self.assertEqual(slave.read_called, 1)
            self.assertEqual(slave.eof_called, 0)
            self.assertEqual(slave.error_called, 0)
            self.assertEqual(slave.received_data, self.CALLBACK_DATA)
            self.assertIsNone(slave.read_errno)
            self.assertIsNone(slave.write_errno)

            master.received_data = None
            slave.received_data = None
            slave.write(self.TEST_DATA_2)
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            self.assertTrue(master.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_called, 1)
            self.assertEqual(master.eof_called, 0)
            self.assertEqual(master.error_called, 0)
            self.assertEqual(master.received_data, self.CALLBACK_DATA)
            self.assertIsNone(master.read_errno)
            self.assertIsNone(master.write_errno)

            self.assertTrue(slave.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(slave.input_data)
            self.assertIsNone(slave.output_data)
            self.assertEqual(slave.read_called, 1)
            self.assertEqual(slave.eof_called, 0)
            self.assertEqual(slave.error_called, 0)
            self.assertIsNone(slave.received_data)
            self.assertIsNone(slave.read_errno)
            self.assertIsNone(slave.write_errno)

            # Cumulative read/write.
            master.received_data = None
            slave.received_data = None
            master.write(self.TEST_DATA_2[:5])
            slave.read()
            master.write(self.TEST_DATA_2[5:])
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            self.assertTrue(master.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_called, 1)
            self.assertEqual(master.eof_called, 0)
            self.assertEqual(master.error_called, 0)
            self.assertIsNone(master.received_data)
            self.assertIsNone(master.read_errno)
            self.assertIsNone(master.write_errno)

            self.assertTrue(slave.read_registered)
            self.assertFalse(slave.write_registered)
            self.assertIsNone(slave.input_data)
            self.assertIsNone(slave.output_data)
            # Buffering, so called only once!
            self.assertEqual(slave.read_called, 2)
            self.assertEqual(slave.eof_called, 0)
            self.assertEqual(slave.error_called, 0)
            self.assertEqual(slave.received_data, self.CALLBACK_DATA)
            self.assertIsNone(slave.read_errno)
            self.assertIsNone(slave.write_errno)

            # EOF
            master.received_data = None
            slave.received_data = None
            master.write(b"\x04")  # ^D
            slave.read()
            data = slave.fetch_input()
            self.assertIsNone(data)

            self.assertTrue(master.is_open)
            self.assertTrue(master.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_called, 1)
            self.assertEqual(master.eof_called, 0)
            self.assertEqual(master.error_called, 0)
            self.assertIsNone(master.received_data)
            self.assertIsNone(master.read_errno)
            self.assertIsNone(master.write_errno)

            self.assertFalse(slave.is_open)
            pty.slave = None
            self.assertFalse(slave.read_registered)
            self.assertFalse(slave.write_registered)
            self.assertIsNone(slave.input_data)
            self.assertIsNone(slave.output_data)
            self.assertEqual(slave.read_called, 2)
            self.assertEqual(slave.eof_called, 1)
            self.assertEqual(slave.error_called, 0)
            self.assertEqual(slave.received_data, self.CALLBACK_DATA)
            self.assertIsNone(slave.read_errno)
            self.assertIsNone(slave.write_errno)

            master.close()
            pty.master = None

    def test_read_error(self):
        """ Force a read error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = IOEndpointForTest(
                pty.master, callback_data=self.CALLBACK_DATA
            )
            slave = IOEndpointForTest(
                pty.slave, callback_data=self.CALLBACK_DATA
            )

            master.register_read()
            slave.register_read()

            # Read/write.
            master.write(self.TEST_DATA_1)
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            slave.write(self.TEST_DATA_2)
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            # Close the slave.
            slave.close()
            pty.slave = None

            master.read()
            data = master.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(master.is_open)
            pty.master = None
            self.assertFalse(master.read_registered)
            self.assertFalse(master.write_registered)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_called, 1)
            self.assertEqual(master.eof_called, 1)
            self.assertEqual(master.error_called, 0)
            self.assertEqual(master.received_data, self.CALLBACK_DATA)
            self.assertEqual(master.read_errno, EIO)
            self.assertEqual(
                master.read_error_text, os.strerror(master.read_errno)
            )
            self.assertIsNone(master.write_errno)

    def test_write_error(self):
        """ Force a write error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = IOEndpointForTest(
                pty.master, callback_data=self.CALLBACK_DATA
            )
            slave = IOEndpointForTest(
                pty.slave, callback_data=self.CALLBACK_DATA
            )

            master.register_read()
            slave.register_read()

            # Read/write.
            master.write(self.TEST_DATA_1)
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            slave.write(self.TEST_DATA_2)
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            # Close the master.
            master.close()
            pty.master = None
            slave.write(self.TEST_DATA_1)

            self.assertFalse(slave.is_open)
            pty.slave = None
            self.assertFalse(slave.read_registered)
            self.assertFalse(slave.write_registered)
            self.assertIsNone(slave.input_data)
            self.assertEqual(bytes(slave.output_data), self.TEST_DATA_1)
            self.assertEqual(slave.read_called, 1)
            self.assertEqual(slave.eof_called, 0)
            self.assertEqual(slave.error_called, 1)
            self.assertEqual(slave.received_data, self.CALLBACK_DATA)
            self.assertIsNone(slave.read_errno)
            self.assertEqual(slave.write_errno, EIO)
            self.assertEqual(
                slave.write_error_text, os.strerror(slave.write_errno)
            )

if __name__ == '__main__':
    unittest.main()
