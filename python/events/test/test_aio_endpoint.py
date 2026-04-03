"""
Asynchronous I/O Endpoint Unit Tests
"""

import unittest

import asyncio
from errno import EIO
import os
from typing import ClassVar

from events.aio_endpoint import AIOEndpoint
from events.pty_manager import PTYManager

class EventHandlerForTest:
    """ Awaitable Event Handler """
    event_future : asyncio.Future

    def __init__(self):
        """ Initialize the context """
        self.event_future = None

    def event_callback(self, event_id : int) -> None:
        """ Set the future value """
        if self.event_future:
            self.event_future.set_result(event_id)

    async def next_event(self, expected : int) -> bool:
        """ Wait for the next expected event """
        while True:
            if not self.event_future:
                self.event_future = asyncio.Future()
            try:
                await asyncio.wait_for(self.event_future, timeout=5)
            except:  # pylint: disable=bare-except
                return False
            event_id = self.event_future.result()
            self.event_future = None
            if event_id == expected:
                break

        return True

class TestAIOEndpoint(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous I/O Endpoint Unit Tests """
    TEST_DATA_1 : ClassVar[bytes] = b"Hello, World!\n"
    TEST_DATA_2 : ClassVar[bytes] = b"The answer is 42\n"

    E_READ_1 : ClassVar[int] = 1
    E_WRITE_1 : ClassVar[int] = 2
    E_READ_2 : ClassVar[int] = 3
    E_WRITE_2 : ClassVar[int] = 4

    async def test_pipe(self):
        """ Create and use a new pipe """
        event_handler = EventHandlerForTest()

        reader_side, writer_side = os.pipe()

        try:
            reader = AIOEndpoint(
                reader_side,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1
            )
            writer = AIOEndpoint(
                writer_side,
                event_callback=event_handler.event_callback,
                write_data=self.E_WRITE_1
            )
            reader.register_read()
            writer.register_write()

            self.assertTrue(reader.is_open)
            self.assertTrue(reader.read_active)
            self.assertFalse(reader.write_active)

            self.assertTrue(writer.is_open)
            self.assertFalse(writer.read_active)
            self.assertTrue(writer.write_active)

            # A write event is expected.
            self.assertTrue(await event_handler.next_event(self.E_WRITE_1))
            writer.write()
            self.assertFalse(writer.write_active)

            # Read/write.
            writer.write(self.TEST_DATA_1)
            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            reader.read()
            data = reader.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)
            self.assertFalse(writer.write_active)
            self.assertTrue(reader.read_active)

            # EOF.
            writer.close()
            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            reader.read()
            data = reader.fetch_input()
            self.assertIsNone(data)
            self.assertFalse(reader.read_active)
            self.assertFalse(reader.is_open)
            self.assertFalse(writer.write_active)
        finally:
            reader.close()
            writer.close()

    async def test_pty(self):
        """ Allocate and use a PTY """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            slave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            master.register_read()
            master.register_write()
            slave.register_read()

            self.assertTrue(master.is_open)
            self.assertTrue(master.read_active)
            self.assertTrue(master.write_active)

            self.assertTrue(slave.is_open)
            self.assertTrue(slave.read_active)
            self.assertFalse(slave.write_active)

            # A master write event is expected.
            self.assertTrue(await event_handler.next_event(self.E_WRITE_1))
            master.write()
            self.assertFalse(master.write_active)

            # Read/write.
            master.write(self.TEST_DATA_1)
            self.assertTrue(await event_handler.next_event(self.E_READ_2))
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            slave.write(self.TEST_DATA_2)
            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            # EOF
            master.write(b"\x04")  # ^D
            self.assertTrue(await event_handler.next_event(self.E_READ_2))
            slave.read()
            data = slave.fetch_input()
            self.assertIsNone(data)
            self.assertFalse(slave.is_open)
            pty.slave = None

    async def test_read_error(self):
        """ Force a read error """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            slave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            master.register_read()
            slave.register_read()

            # Read/write.
            master.write(self.TEST_DATA_1)
            self.assertTrue(await event_handler.next_event(self.E_READ_2))
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            slave.write(self.TEST_DATA_2)
            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            # Close the slave.
            slave.close()
            pty.slave = None

            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            master.read()
            data = master.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(master.is_open)
            pty.master = None
            self.assertFalse(master.read_active)
            self.assertFalse(master.write_active)
            self.assertIsNone(master.input_data)
            self.assertIsNone(master.output_data)
            self.assertEqual(master.read_errno, EIO)
            self.assertEqual(
                master.read_error_text, os.strerror(master.read_errno)
            )
            self.assertIsNone(master.write_errno)

    async def test_write_error(self):
        """ Force a write error """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()

            master = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            slave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            master.register_read()
            slave.register_read()

            master.write(self.TEST_DATA_1)
            self.assertTrue(await event_handler.next_event(self.E_READ_2))
            slave.read()
            data = slave.fetch_input()
            self.assertEqual(data, self.TEST_DATA_1)

            slave.write(self.TEST_DATA_2)
            self.assertTrue(await event_handler.next_event(self.E_READ_1))
            master.read()
            data = master.fetch_input()
            self.assertEqual(data, self.TEST_DATA_2)

            # Close the master.
            master.close()
            pty.master = None
            slave.write(self.TEST_DATA_1)

            self.assertFalse(slave.is_open)
            pty.slave = None
            self.assertFalse(slave.read_active)
            self.assertFalse(slave.write_active)
            self.assertIsNone(slave.input_data)
            self.assertEqual(slave.output_data, self.TEST_DATA_1)
            self.assertIsNone(slave.read_errno)
            self.assertEqual(slave.write_errno, EIO)
            self.assertEqual(
                slave.write_error_text, os.strerror(slave.write_errno)
            )

if __name__ == '__main__':
    unittest.main()
