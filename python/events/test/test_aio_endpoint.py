"""
Asynchronous I/O Endpoint Unit Tests
"""

import unittest

import asyncio
from typing import ClassVar

from events.aio_endpoint import AIOEndpoint

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

    async def next_event(self) -> int:
        """ Wait for the next event """
        if not self.event_future:
            self.event_future = asyncio.Future()
        await self.event_future
        event_id = self.event_future.result()
        self.event_future = None
        return event_id

class TestAIOEndpoint(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous I/O Endpoint Unit Tests """
    TEST_DATA : ClassVar[bytes] = b"Hello, World!\n"

    E_READ : ClassVar[int] = 1
    E_WRITE : ClassVar[int] = 2

    async def test_default_pipe(self):
        """ Create and use a new pipe """
        event_handler = EventHandlerForTest()

        with AIOEndpoint(
            event_callback=event_handler.event_callback,
            read_data=self.E_READ,
            write_data=self.E_WRITE
        ) as endpoint:
            endpoint.register_read()
            endpoint.register_write()

            # A write event is expected.
            event_id = await event_handler.next_event()
            self.assertEqual(event_id, self.E_WRITE)
            endpoint.write()
            self.assertFalse(endpoint.write_active)

            # Read/write.
            endpoint.write(self.TEST_DATA)
            event_id = await event_handler.next_event()
            self.assertEqual(event_id, self.E_READ)
            endpoint.read()
            data = endpoint.fetch_input()
            self.assertEqual(data, self.TEST_DATA)
            self.assertTrue(endpoint.read_active)
            self.assertFalse(endpoint.write_active)

            # EOF.
            endpoint.read_only()
            event_id = await event_handler.next_event()
            self.assertEqual(event_id, self.E_READ)
            endpoint.read()
            data = endpoint.fetch_input()
            self.assertIsNone(data)
            self.assertFalse(endpoint.read_active)
            self.assertFalse(endpoint.write_active)

if __name__ == '__main__':
    unittest.main()
