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

    def __init__(
        self, reader : Optional[int] = None,
        writer : Optional[int] = None,
        callback_data : Optional[Any] = None
    ):
        """ Initialize call indicators """
        self.read_called = 0
        self.eof_called = 0
        self.error_called = 0
        self.received_data = None
        self.read_registered = False
        self.write_registered = False

        super().__init__(
            reader=reader,
            writer=writer,
            read_callback=self.count_read,
            eof_callback=self.count_eof,
            write_error_callback=self.count_error,
            callback_data=callback_data
        )

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
        """ Create and use a pipe (default case) """
        # pylint: disable=too-many-statements
        with IOEndpointForTest(callback_data=self.CALLBACK_DATA) as endpoint:
            self.assertTrue(endpoint.is_reading)
            self.assertTrue(endpoint.is_writing)

            endpoint.register_read()
            endpoint.register_write()
            self.assertTrue(endpoint.read_registered)
            self.assertTrue(endpoint.write_registered)

            # Standard read/write.
            endpoint.received_data = None
            endpoint.write(self.TEST_DATA_1)
            endpoint.read()
            data = endpoint.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            self.assertTrue(endpoint.read_registered)
            self.assertFalse(endpoint.write_registered)
            self.assertIsNone(endpoint.input_data)
            self.assertIsNone(endpoint.output_data)
            self.assertEqual(endpoint.read_called, 1)
            self.assertEqual(endpoint.eof_called, 0)
            self.assertEqual(endpoint.error_called, 0)
            self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
            self.assertIsNone(endpoint.read_error)
            self.assertIsNone(endpoint.write_error)

            # Cumulative read/write.
            endpoint.received_data = None
            endpoint.write(self.TEST_DATA_2[:5])
            endpoint.read()
            endpoint.write(self.TEST_DATA_2[5:])
            endpoint.read()
            data = endpoint.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            self.assertTrue(endpoint.read_registered)
            self.assertFalse(endpoint.write_registered)
            self.assertIsNone(endpoint.input_data)
            self.assertIsNone(endpoint.output_data)
            # No buffering on a pipe so called twice!
            self.assertEqual(endpoint.read_called, 3)
            self.assertEqual(endpoint.eof_called, 0)
            self.assertEqual(endpoint.error_called, 0)
            self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
            self.assertIsNone(endpoint.read_error)
            self.assertIsNone(endpoint.write_error)

            # EOF.
            endpoint.received_data = None
            endpoint.read_only()
            endpoint.read()
            data = endpoint.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(endpoint.is_reading)
            self.assertFalse(endpoint.is_writing)
            self.assertFalse(endpoint.read_registered)
            self.assertFalse(endpoint.write_registered)
            self.assertIsNone(endpoint.input_data)
            self.assertIsNone(endpoint.output_data)
            self.assertEqual(endpoint.read_called, 3)
            self.assertEqual(endpoint.eof_called, 1)
            self.assertEqual(endpoint.error_called, 0)
            self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
            self.assertIsNone(endpoint.read_error)
            self.assertIsNone(endpoint.write_error)

        self.assertFalse(endpoint.is_reading)
        self.assertFalse(endpoint.is_writing)

    def test_pty_slave(self):
        """ Create a PTY and use the slave only """
        # pylint: disable=too-many-statements
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            # Use the slave so we can send ^D to simulate EOF.
            with IOEndpointForTest(
                reader=pty.slave, callback_data=self.CALLBACK_DATA
            ) as endpoint:
                # Grab ownership of the slave for the endpoint.
                pty.slave = None
                os.set_blocking(pty.master, False)

                self.assertTrue(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)

                endpoint.register_read()
                endpoint.register_write()
                self.assertTrue(endpoint.read_registered)
                self.assertTrue(endpoint.write_registered)

                # Read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_1)
                data = os.read(pty.master, 32)
                self.assertEqual(data, self.TEST_DATA_1)
                os.write(pty.master, self.TEST_DATA_2)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # Cumulative read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_2[:5])
                endpoint.write(self.TEST_DATA_2[5:])
                data = os.read(pty.master, 32)
                self.assertEqual(data, self.TEST_DATA_2)
                os.write(pty.master, self.TEST_DATA_1[:5])
                os.write(pty.master, self.TEST_DATA_1[5:])
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_1)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                # Buffering, so called only once!
                self.assertEqual(endpoint.read_called, 2)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # EOF
                endpoint.received_data = None
                os.write(pty.master, b"\x04")  # ^D
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertIsNone(data)

                self.assertFalse(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)
                self.assertFalse(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 2)
                self.assertEqual(endpoint.eof_called, 1)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

        self.assertFalse(endpoint.is_reading)
        self.assertFalse(endpoint.is_writing)

    def test_pty_both(self):
        """ Create a PTY and use the master and slave"""
        # pylint: disable=too-many-statements
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            # Use the master as the writer so we can send ^D to simulate EOF.
            with IOEndpointForTest(
                reader=pty.slave,
                writer=pty.master,
                callback_data=self.CALLBACK_DATA
            ) as endpoint:
                # Grab ownership for the endpoint.
                pty.master = None
                pty.slave = None

                self.assertTrue(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)

                endpoint.register_read()
                endpoint.register_write()
                self.assertTrue(endpoint.read_registered)
                self.assertTrue(endpoint.write_registered)

                # Read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_1)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_1)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # Cumulative read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_2[:5])
                endpoint.read()
                endpoint.write(self.TEST_DATA_2[5:])
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                # Buffering, so called only once!
                self.assertEqual(endpoint.read_called, 2)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # EOF
                endpoint.received_data = None
                endpoint.write(b"\x04")  # ^D
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertIsNone(data)

                self.assertFalse(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)
                self.assertFalse(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 2)
                self.assertEqual(endpoint.eof_called, 1)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

        self.assertFalse(endpoint.is_reading)
        self.assertFalse(endpoint.is_writing)

    def test_read_error(self):
        """ Force a read error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()

            with IOEndpointForTest(
                reader=pty.master, callback_data=self.CALLBACK_DATA
            ) as endpoint:
                # Grab ownership for the endpoint.
                pty.master = None
                os.set_blocking(pty.slave, False)

                self.assertTrue(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)

                endpoint.register_read()
                endpoint.register_write()
                self.assertTrue(endpoint.read_registered)
                self.assertTrue(endpoint.write_registered)

                # Read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_1)
                data = os.read(pty.slave, 32)
                self.assertEqual(data, self.TEST_DATA_1)
                os.write(pty.slave, self.TEST_DATA_2)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # Close the slave.
                endpoint.received_data = None
                pty.close()
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertIsNone(data)

                self.assertFalse(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)
                self.assertFalse(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 1)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                error = endpoint.read_error
                self.assertEqual(error[0], EIO)
                self.assertEqual(error[1], os.strerror(error[0]))
                self.assertIsNone(endpoint.write_error)

    def test_write_error(self):
        """ Force a write error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            # Use the slave because the master may buffer and not fail.
            with IOEndpointForTest(
                writer=pty.slave, callback_data=self.CALLBACK_DATA
            ) as endpoint:
                # Grab ownership for the endpoint.
                pty.slave = None
                os.set_blocking(pty.master, False)

                self.assertTrue(endpoint.is_reading)
                self.assertTrue(endpoint.is_writing)

                endpoint.register_read()
                endpoint.register_write()
                self.assertTrue(endpoint.read_registered)
                self.assertTrue(endpoint.write_registered)

                # Read/write.
                endpoint.received_data = None
                endpoint.write(self.TEST_DATA_1)
                data = os.read(pty.master, 32)
                self.assertEqual(data, self.TEST_DATA_1)
                os.write(pty.master, self.TEST_DATA_2)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertIsNone(endpoint.output_data)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 0)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                self.assertIsNone(endpoint.write_error)

                # Close the master.
                endpoint.received_data = None
                pty.close()
                endpoint.write(self.TEST_DATA_1)

                self.assertTrue(endpoint.is_reading)
                self.assertFalse(endpoint.is_writing)
                self.assertTrue(endpoint.read_registered)
                self.assertFalse(endpoint.write_registered)
                self.assertIsNone(endpoint.input_data)
                self.assertEqual(bytes(endpoint.output_data), self.TEST_DATA_1)
                self.assertEqual(endpoint.read_called, 1)
                self.assertEqual(endpoint.eof_called, 0)
                self.assertEqual(endpoint.error_called, 1)
                self.assertEqual(endpoint.received_data, self.CALLBACK_DATA)
                self.assertIsNone(endpoint.read_error)
                error = endpoint.write_error
                self.assertEqual(error[0], EIO)
                self.assertEqual(error[1], os.strerror(error[0]))

if __name__ == '__main__':
    unittest.main()
