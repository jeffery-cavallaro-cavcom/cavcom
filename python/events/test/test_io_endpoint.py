"""
I/O Endpoint Unit Tests
"""

import unittest

from errno import EIO
import os
from typing import ClassVar

from events.io_endpoint import IOEndpoint
from events.pty_manager import PTYManager

class IOEndpointForTest(IOEndpoint):
    """ Monitor event register """
    read_registered : bool
    write_registered : bool

    def __init__(self, fd : int, **kwargs):
        """ Initialize register indicators """
        super().__init__(fd, **kwargs)

        self.read_registered = False
        self.write_registered = False

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

    def test_none(self):
        """ No actual file descriptor (buffer only) """
        with IOEndpoint(None) as endpoint:
            self.assertFalse(endpoint.is_open)
            endpoint.input_data.append(self.TEST_DATA_1)

        self.assertFalse(endpoint.is_open)
        self.assertEqual(
            endpoint.fetch_input(text=True, reset=True),
            self.TEST_DATA_1.decode()
        )
        self.assertIsNone(endpoint.fetch_input())

    def test_pipe(self):
        """ Create and use a pipe """
        # pylint: disable=too-many-statements
        reader_side, writer_side = os.pipe()

        with (
            IOEndpointForTest(reader_side) as reader,
            IOEndpointForTest(writer_side) as writer
        ):
            self.assertTrue(reader.is_open)
            self.assertTrue(writer.is_open)

            reader.register_read()
            writer.register_write()
            self.assertTrue(reader.read_registered)
            self.assertTrue(writer.write_registered)

            # Standard read/write.
            writer.write(self.TEST_DATA_1)
            reader.read()
            data = reader.fetch_input(text=True)
            self.assertEqual(data, self.TEST_DATA_1.decode())
            data = reader.fetch_input(reset=True)
            self.assertEqual(data, self.TEST_DATA_1)

            self.assertTrue(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.fetch_input())
            self.assertEqual(len(writer.output_data), 0)
            self.assertIsNone(reader.errno)
            self.assertIsNone(writer.errno)

            # Cumulative read/write.
            writer.write(self.TEST_DATA_2[:5])
            reader.read()
            writer.write(self.TEST_DATA_2[5:])
            reader.read()
            data = reader.fetch_input(reset=True)
            self.assertEqual(data, self.TEST_DATA_2)

            self.assertTrue(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.fetch_input())
            self.assertEqual(len(writer.output_data), 0)
            self.assertIsNone(reader.errno)
            self.assertIsNone(writer.errno)

            # EOF.
            writer.close()
            reader.read()
            data = reader.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(reader.is_open)
            self.assertFalse(writer.is_open)
            self.assertFalse(reader.read_registered)
            self.assertFalse(writer.write_registered)
            self.assertIsNone(reader.fetch_input())
            self.assertEqual(len(writer.output_data), 0)
            self.assertIsNone(reader.errno)
            self.assertIsNone(writer.errno)

        self.assertFalse(reader.is_open)
        self.assertFalse(writer.is_open)

    def test_pty(self):
        """ Allocate and use a PTY """
        # pylint: disable=too-many-statements
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with (
                IOEndpointForTest(pty.master, no_close=True) as master,
                IOEndpointForTest(pty.slave, no_close=True) as slave
            ):
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
                master.write(self.TEST_DATA_1)
                slave.read()
                data = slave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_1)

                self.assertTrue(master.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(master.fetch_input())
                self.assertEqual(len(master.output_data), 0)
                self.assertIsNone(master.errno)

                self.assertTrue(slave.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(slave.fetch_input())
                self.assertEqual(len(slave.output_data), 0)
                self.assertIsNone(slave.errno)
                self.assertIsNone(slave.errno)

                slave.write(self.TEST_DATA_2)
                master.read()
                data = master.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(master.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(master.fetch_input())
                self.assertEqual(len(master.output_data), 0)
                self.assertIsNone(master.errno)
                self.assertIsNone(master.errno)

                self.assertTrue(slave.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(slave.fetch_input())
                self.assertEqual(len(slave.output_data), 0)
                self.assertIsNone(slave.errno)

                # Cumulative read/write.
                master.write(self.TEST_DATA_2[:5])
                slave.read()
                master.write(self.TEST_DATA_2[5:])
                slave.read()
                data = slave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_2)

                self.assertTrue(master.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(master.fetch_input())
                self.assertEqual(len(master.output_data), 0)
                self.assertIsNone(master.errno)

                self.assertTrue(slave.read_registered)
                self.assertFalse(slave.write_registered)
                self.assertIsNone(slave.fetch_input())
                self.assertEqual(len(slave.output_data), 0)
                self.assertIsNone(slave.errno)
                self.assertIsNone(slave.errno)

                # EOF
                master.write(b"\x04")  # ^D
                slave.read()
                data = slave.fetch_input(reset=True)
                self.assertIsNone(data)

                self.assertTrue(master.is_open)
                self.assertTrue(master.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(master.fetch_input())
                self.assertEqual(len(master.output_data), 0)
                self.assertIsNone(master.errno)

                self.assertFalse(slave.is_open)
                self.assertFalse(slave.read_registered)
                self.assertFalse(slave.write_registered)
                self.assertIsNone(slave.fetch_input())
                self.assertEqual(len(slave.output_data), 0)
                self.assertIsNone(slave.errno)

            self.assertFalse(master.is_open)
            self.assertFalse(slave.is_open)
            self.assertIsNotNone(pty.master)
            self.assertIsNotNone(pty.slave)

        self.assertIsNone(pty.master)
        self.assertIsNone(pty.slave)

    def test_read_error(self):
        """ Force a read error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with (
                IOEndpointForTest(pty.master, no_close=True) as master,
                IOEndpointForTest(pty.slave) as slave
            ):
                pty.slave = None  # Grab ownership

                master.register_read()
                slave.register_read()

                # Read/write.
                master.write(self.TEST_DATA_1)
                slave.read()
                data = slave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_1)

                slave.write(self.TEST_DATA_2)
                master.read()
                data = master.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_2)

                # Close the slave to force a master read error.
                slave.close()

                master.read()
                data = master.fetch_input(reset=True)
                self.assertIsNone(data)

                self.assertFalse(master.is_open)
                self.assertFalse(master.read_registered)
                self.assertFalse(master.write_registered)
                self.assertIsNone(master.fetch_input())
                self.assertEqual(len(master.output_data), 0)
                self.assertEqual(master.errno, EIO)
                self.assertEqual(master.error_text, os.strerror(master.errno))

            self.assertFalse(master.is_open)
            self.assertFalse(slave.is_open)

        self.assertIsNone(pty.master)
        self.assertIsNone(pty.slave)

    def test_write_error(self):
        """ Force a write error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with (
                IOEndpointForTest(pty.master) as master,
                IOEndpointForTest(pty.slave, no_close=True) as slave
            ):
                pty.master = None  # Grab ownership

                master.register_read()
                slave.register_read()

                # Read/write.
                master.write(self.TEST_DATA_1)
                slave.read()
                data = slave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_1)

                slave.write(self.TEST_DATA_2)
                master.read()
                data = master.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_DATA_2)

                # Close the master to force a slave write error.
                master.close()
                slave.write(self.TEST_DATA_1)

                self.assertFalse(slave.is_open)
                self.assertFalse(slave.read_registered)
                self.assertFalse(slave.write_registered)
                self.assertIsNone(slave.fetch_input())
                self.assertEqual(len(slave.output_data), len(self.TEST_DATA_1))
                self.assertEqual(slave.errno, EIO)
                self.assertEqual(slave.error_text, os.strerror(slave.errno))

            self.assertFalse(master.is_open)
            self.assertFalse(slave.is_open)

        self.assertIsNone(pty.master)
        self.assertIsNone(pty.slave)

if __name__ == '__main__':
    unittest.main()
