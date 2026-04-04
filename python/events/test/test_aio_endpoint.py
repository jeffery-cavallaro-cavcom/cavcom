"""
Asynchronous I/O Endpoint Unit Tests
"""

import unittest

import asyncio
from errno import EIO
import os
from typing import ClassVar, Optional

from events.aio_endpoint import AIOEndpoint
from events.pty_manager import PTYManager

class EventHandlerForTest:
    """ Awaitable Event Handler """
    pending : set[int]
    event_future : asyncio.Future

    def __init__(self):
        """ Initialize the context """
        self.pending = set()
        self.event_future = asyncio.Future()

    def event_callback(self, event_id : int) -> None:
        """
        Set the future value

        Arguments:
            event_id:
                Event ID for set future value.
        """
        self.pending.add(event_id)

        if not self.event_future.done():
            self.event_future.set_result(self.pending)

    async def next_event(self, expected : Optional[int] = None) -> int:
        """
        Wait for the next expected event

        Arguments:
            expected:
                If None then wait for the next event.  Otherwise, wait for the
                specified event.

        Returns:
            Next event, or None if the expected event did not occur.
        """
        while True:
            if not self.pending:
                if self.event_future.done():
                    self.event_future = asyncio.Future()
                try:
                    await asyncio.wait_for(self.event_future, timeout=5)
                except:  # pylint: disable=bare-except
                    event_id = None
                    break

            if not self.pending:
                continue

            event_id = self.pending.pop()
            if expected is None or event_id == expected:
                break

        return event_id

class TestAIOEndpoint(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous I/O Endpoint Unit Tests """
    TEST_ADATA_1 : ClassVar[bytes] = b"Hello, World!\n"
    TEST_ADATA_2 : ClassVar[bytes] = b"The answer is 42\n"

    E_READ_1 : ClassVar[int] = 1
    E_WRITE_1 : ClassVar[int] = 2
    E_READ_2 : ClassVar[int] = 3
    E_WRITE_2 : ClassVar[int] = 4

    async def test_pipe(self):
        """ Create and use a new pipe """
        event_handler = EventHandlerForTest()

        reader_side, writer_side = os.pipe()

        try:
            areader = AIOEndpoint(
                reader_side,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1
            )
            awriter = AIOEndpoint(
                writer_side,
                event_callback=event_handler.event_callback,
                write_data=self.E_WRITE_1
            )
            areader.register_read()
            awriter.register_write()

            self.assertTrue(areader.is_open)
            self.assertTrue(areader.read_active)
            self.assertFalse(areader.write_active)

            self.assertTrue(awriter.is_open)
            self.assertFalse(awriter.read_active)
            self.assertTrue(awriter.write_active)

            # A write event is expected.
            event_id = await event_handler.next_event(self.E_WRITE_1)
            self.assertEqual(event_id, self.E_WRITE_1)
            awriter.write()
            self.assertFalse(awriter.write_active)

            # Read/write.
            awriter.write(self.TEST_ADATA_1)
            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            areader.read()
            data = areader.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_1)
            self.assertFalse(awriter.write_active)
            self.assertTrue(areader.read_active)

            # EOF.
            awriter.close()
            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            areader.read()
            data = areader.fetch_input()
            self.assertIsNone(data)
            self.assertFalse(areader.read_active)
            self.assertFalse(areader.is_open)
            self.assertFalse(awriter.write_active)
        finally:
            areader.close()
            awriter.close()

    async def test_pty(self):
        """ Allocate and use a PTY """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            amaster = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            aslave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            amaster.register_read()
            amaster.register_write()
            aslave.register_read()

            self.assertTrue(amaster.is_open)
            self.assertTrue(amaster.read_active)
            self.assertTrue(amaster.write_active)

            self.assertTrue(aslave.is_open)
            self.assertTrue(aslave.read_active)
            self.assertFalse(aslave.write_active)

            # A master write event is expected.
            event_id = await event_handler.next_event(self.E_WRITE_1)
            self.assertEqual(event_id, self.E_WRITE_1)
            amaster.write()
            self.assertFalse(amaster.write_active)

            # Read/write.
            amaster.write(self.TEST_ADATA_1)
            event_id = await event_handler.next_event(self.E_READ_2)
            self.assertEqual(event_id, self.E_READ_2)
            aslave.read()
            data = aslave.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_1)

            aslave.write(self.TEST_ADATA_2)
            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            amaster.read()
            data = amaster.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_2)

            # EOF
            amaster.write(b"\x04")  # ^D
            event_id = await event_handler.next_event(self.E_READ_2)
            self.assertEqual(event_id, self.E_READ_2)
            aslave.read()
            data = aslave.fetch_input()
            self.assertIsNone(data)
            self.assertFalse(aslave.is_open)
            pty.slave = None

    async def test_both(self):
        """ Read and write event on the same endpoint """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            amaster = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            aslave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            amaster.register_read()
            amaster.register_write()

            self.assertTrue(amaster.is_open)
            self.assertTrue(amaster.read_active)
            self.assertTrue(amaster.write_active)

            aslave.write(self.TEST_ADATA_1)
            events = set()
            while len(events) < 2:
                events.add(await event_handler.next_event())
            self.assertEqual(len(events), 2)
            self.assertIn(self.E_READ_1, events)
            self.assertIn(self.E_WRITE_1, events)

            amaster.write()
            amaster.read()
            data = amaster.fetch_input()

            self.assertEqual(data, self.TEST_ADATA_1)
            self.assertTrue(amaster.read_active)
            self.assertFalse(amaster.write_active)

    async def test_read_error(self):
        """ Force a read error """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            amaster = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            aslave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            amaster.register_read()
            aslave.register_read()

            # Read/write.
            amaster.write(self.TEST_ADATA_1)
            event_id = await event_handler.next_event(self.E_READ_2)
            self.assertEqual(event_id, self.E_READ_2)
            aslave.read()
            data = aslave.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_1)

            aslave.write(self.TEST_ADATA_2)
            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            amaster.read()
            data = amaster.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_2)

            # Close the slave.
            aslave.close()
            pty.slave = None

            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            amaster.read()
            data = amaster.fetch_input()
            self.assertIsNone(data)

            self.assertFalse(amaster.is_open)
            pty.master = None
            self.assertFalse(amaster.read_active)
            self.assertFalse(amaster.write_active)
            self.assertIsNone(amaster.input_data)
            self.assertIsNone(amaster.output_data)
            self.assertEqual(amaster.read_errno, EIO)
            self.assertEqual(
                amaster.read_error_text, os.strerror(amaster.read_errno)
            )
            self.assertIsNone(amaster.write_errno)

    async def test_write_error(self):
        """ Force a write error """
        event_handler = EventHandlerForTest()

        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            amaster = AIOEndpoint(
                pty.master,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_1,
                write_data=self.E_WRITE_1
            )
            aslave = AIOEndpoint(
                pty.slave,
                event_callback=event_handler.event_callback,
                read_data=self.E_READ_2,
                write_data=self.E_WRITE_2
            )

            amaster.register_read()
            aslave.register_read()

            # Read/write.
            amaster.write(self.TEST_ADATA_1)
            event_id = await event_handler.next_event(self.E_READ_2)
            self.assertEqual(event_id, self.E_READ_2)
            aslave.read()
            data = aslave.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_1)

            aslave.write(self.TEST_ADATA_2)
            event_id = await event_handler.next_event(self.E_READ_1)
            self.assertEqual(event_id, self.E_READ_1)
            amaster.read()
            data = amaster.fetch_input()
            self.assertEqual(data, self.TEST_ADATA_2)

            # Close the master.
            amaster.close()
            pty.master = None
            aslave.write(self.TEST_ADATA_1)

            self.assertFalse(aslave.is_open)
            pty.slave = None
            self.assertFalse(aslave.read_active)
            self.assertFalse(aslave.write_active)
            self.assertIsNone(aslave.input_data)
            self.assertEqual(aslave.output_data, self.TEST_ADATA_1)
            self.assertIsNone(aslave.read_errno)
            self.assertEqual(aslave.write_errno, EIO)
            self.assertEqual(
                aslave.write_error_text, os.strerror(aslave.write_errno)
            )

if __name__ == '__main__':
    unittest.main()
