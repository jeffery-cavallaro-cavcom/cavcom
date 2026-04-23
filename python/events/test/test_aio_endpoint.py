"""
Asynchronous I/O Endpoint Unit Tests
"""

import unittest

import asyncio
from errno import EIO
import os
from typing import Any, ClassVar

from events.aio_endpoint import AIOEndpoint
from events.pty_manager import PTYManager
from events.aevent_loop import AEventLoop

class TestAIOEndpoint(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous I/O Endpoint Unit Tests """
    TEST_ADATA_1 : ClassVar[bytes] = b"Hello, World!\n"
    TEST_ADATA_2 : ClassVar[bytes] = b"The answer is 42\n"

    E_READ_1 : ClassVar[int] = 1
    E_WRITE_1 : ClassVar[int] = 2
    E_READ_2 : ClassVar[int] = 3
    E_WRITE_2 : ClassVar[int] = 4

    TIMEOUT : ClassVar[float] = 5.0  # seconds

    async def get_event(self, loop : AEventLoop, event : Any) -> None:
        """ Wait for a specific event """
        while True:
            if await loop.wait_for_event(timeout=self.TIMEOUT) == event:
                return
            await asyncio.sleep(0.0)

    async def test_pipe(self):
        """ Create and use a new pipe """
        event_loop = AEventLoop()

        reader_side, writer_side = os.pipe()

        with (
            AIOEndpoint(
                reader_side,
                event_callback=event_loop.add_event,
                read_data=self.E_READ_1
            ) as areader,
            AIOEndpoint(
                writer_side,
                event_callback=event_loop.add_event,
                write_data=self.E_WRITE_1
            ) as awriter
        ):
            areader.register_read()
            awriter.register_write()

            self.assertTrue(areader.is_open)
            self.assertTrue(areader.read_active)
            self.assertFalse(areader.write_active)

            self.assertTrue(awriter.is_open)
            self.assertFalse(awriter.read_active)
            self.assertTrue(awriter.write_active)

            # A write event is expected.
            await self.get_event(event_loop, self.E_WRITE_1)
            awriter.write()
            self.assertFalse(awriter.write_active)

            # Read/write.
            awriter.write(self.TEST_ADATA_1)
            await self.get_event(event_loop, self.E_READ_1)
            areader.read()
            data = areader.fetch_input(reset=True)
            self.assertEqual(data, self.TEST_ADATA_1)
            self.assertTrue(areader.read_active)
            self.assertFalse(awriter.write_active)

            # EOF.
            awriter.close()
            self.assertFalse(awriter.is_open)
            self.assertFalse(awriter.write_active)
            await self.get_event(event_loop, self.E_READ_1)
            areader.read()
            data = areader.fetch_input(reset=True)
            self.assertIsNone(data)
            self.assertFalse(areader.is_open)
            self.assertFalse(areader.read_active)

        self.assertFalse(areader.is_open)
        self.assertFalse(awriter.is_open)

    async def test_pty(self):
        """ Allocate and use a PTY """
        event_loop = AEventLoop()

        with PTYManager() as apty:
            apty.disable_echo_crlf()
            apty.set_nonblocking()

            with (
                AIOEndpoint(
                    apty.master,
                    event_callback=event_loop.add_event,
                    no_close=True,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                ) as amaster,
                AIOEndpoint(
                    apty.slave,
                    no_close=True,
                    event_callback=event_loop.add_event,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                ) as aslave
            ):
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
                await self.get_event(event_loop, self.E_WRITE_1)
                amaster.write()
                self.assertFalse(amaster.write_active)

                # Read/write.
                amaster.write(self.TEST_ADATA_1)
                await self.get_event(event_loop, self.E_READ_2)
                aslave.read()
                data = aslave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_1)

                aslave.write(self.TEST_ADATA_2)
                await self.get_event(event_loop, self.E_READ_1)
                amaster.read()
                data = amaster.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_2)

                # EOF
                amaster.write(b"\x04")  # ^D
                await self.get_event(event_loop, self.E_READ_2)
                aslave.read()
                data = aslave.fetch_input(reset=True)
                self.assertIsNone(data)
                self.assertFalse(aslave.is_open)

            self.assertFalse(amaster.is_open)
            self.assertFalse(aslave.is_open)

        self.assertIsNone(apty.master)
        self.assertIsNone(apty.slave)

    async def test_both(self):
        """ Read and write event on the same endpoint """
        event_loop = AEventLoop()

        with PTYManager() as apty:
            apty.disable_echo_crlf()
            apty.set_nonblocking()

            with (
                AIOEndpoint(
                    apty.master,
                    event_callback=event_loop.add_event,
                    no_close=True,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                ) as amaster,
                AIOEndpoint(
                    apty.slave,
                    no_close=True,
                    event_callback=event_loop.add_event,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                ) as aslave
            ):
                amaster.register_read()
                amaster.register_write()

                self.assertTrue(amaster.is_open)
                self.assertTrue(amaster.read_active)
                self.assertTrue(amaster.write_active)

                aslave.write(self.TEST_ADATA_1)
                events = set()
                while len(events) < 2:
                    events.add(
                        await event_loop.wait_for_event(timeout=self.TIMEOUT)
                    )
                self.assertIn(self.E_READ_1, events)
                self.assertIn(self.E_WRITE_1, events)

                amaster.write()
                amaster.read()
                data = amaster.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_1)
                self.assertTrue(amaster.read_active)
                self.assertFalse(amaster.write_active)

            self.assertFalse(amaster.is_open)
            self.assertFalse(aslave.is_open)

        self.assertIsNone(apty.master)
        self.assertIsNone(apty.slave)

    async def test_read_error(self):
        """ Force a read error """
        event_loop = AEventLoop()

        with PTYManager() as apty:
            apty.disable_echo_crlf()
            apty.set_nonblocking()

            with (
                AIOEndpoint(
                    apty.master,
                    event_callback=event_loop.add_event,
                    no_close=True,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                ) as amaster,
                AIOEndpoint(
                    apty.slave,
                    event_callback=event_loop.add_event,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                ) as aslave
            ):
                apty.slave = None  # Grab ownership

                amaster.register_read()
                aslave.register_read()

                # Read/write.
                amaster.write(self.TEST_ADATA_1)
                await self.get_event(event_loop, self.E_READ_2)
                aslave.read()
                data = aslave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_1)

                aslave.write(self.TEST_ADATA_2)
                await self.get_event(event_loop, self.E_READ_1)
                amaster.read()
                data = amaster.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_2)

                # Close the slave to force a read error on the master.
                aslave.close()
                await self.get_event(event_loop, self.E_READ_1)
                amaster.read()
                data = amaster.fetch_input(reset=True)
                self.assertIsNone(data)
                self.assertFalse(amaster.is_open)
                self.assertFalse(amaster.read_active)
                self.assertFalse(amaster.write_active)
                self.assertIsNone(amaster.fetch_input())
                self.assertEqual(len(amaster.output_data), 0)
                self.assertEqual(amaster.errno, EIO)
                self.assertEqual(amaster.error_text, os.strerror(amaster.errno))

            self.assertFalse(amaster.is_open)
            self.assertFalse(aslave.is_open)

        self.assertIsNone(apty.master)
        self.assertIsNone(apty.slave)

    async def test_write_error(self):
        """ Force a write error """
        event_loop = AEventLoop()

        with PTYManager() as apty:
            apty.disable_echo_crlf()
            apty.set_nonblocking()

            with (
                AIOEndpoint(
                    apty.master,
                    event_callback=event_loop.add_event,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                ) as amaster,
                AIOEndpoint(
                    apty.slave,
                    no_close=True,
                    event_callback=event_loop.add_event,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                ) as aslave
            ):
                apty.master = None  # Grab ownership

                amaster.register_read()
                aslave.register_read()

                # Read/write.
                amaster.write(self.TEST_ADATA_1)
                await self.get_event(event_loop, self.E_READ_2)
                aslave.read()
                data = aslave.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_1)

                aslave.write(self.TEST_ADATA_2)
                await self.get_event(event_loop, self.E_READ_1)
                amaster.read()
                data = amaster.fetch_input(reset=True)
                self.assertEqual(data, self.TEST_ADATA_2)

                # Close the master to force a write error on the slave.
                amaster.close()
                aslave.write(self.TEST_ADATA_1)
                self.assertFalse(aslave.is_open)
                self.assertFalse(aslave.read_active)
                self.assertFalse(aslave.write_active)
                self.assertIsNone(aslave.fetch_input())
                self.assertEqual(len(aslave.output_data), len(self.TEST_ADATA_1))
                self.assertEqual(aslave.errno, EIO)
                self.assertEqual(aslave.error_text, os.strerror(aslave.errno))

            self.assertFalse(amaster.is_open)
            self.assertFalse(aslave.is_open)

        self.assertIsNone(apty.master)
        self.assertIsNone(apty.slave)

if __name__ == '__main__':
    unittest.main()
