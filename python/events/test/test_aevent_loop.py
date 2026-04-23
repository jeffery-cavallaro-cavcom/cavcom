"""
Asynchronous Event Loop Unit Tests
"""

import unittest

import asyncio
from typing import ClassVar

from events.aevent_loop import AEventLoop

class TestAEventLoop(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous Event Loop Unit Tests """
    TIMEOUT : ClassVar[float] = 5.0  # seconds

    @staticmethod
    async def add_events(loop : AEventLoop, events : list[int]) -> None:
        """ Add events to the event loop """
        for event in events:
            await asyncio.sleep(0.0)
            loop.add_event(event)

    async def test_none(self):
        """ No pending events """
        loop = AEventLoop()
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

    async def test_events(self):
        """ Handle multiple events """
        loop = AEventLoop()

        # one event
        event_task = asyncio.create_task(self.add_events(loop, [1]))
        event = await loop.wait_for_event(timeout=self.TIMEOUT)
        await event_task
        self.assertEqual(event, 1)
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

        # two more
        pending = [2, 3]
        event_task = asyncio.create_task(self.add_events(loop, pending))
        events = []
        for _ in range(2):
            events.append(await loop.wait_for_event(timeout=self.TIMEOUT))
        await event_task
        self.assertTrue(len(events), 2)
        self.assertEqual(set(events), set(pending))
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

        # Add three but get one
        pending = [4, 5, 6]
        event_task = asyncio.create_task(self.add_events(loop, pending))
        event = await loop.wait_for_event(timeout=self.TIMEOUT)
        await event_task
        self.assertIn(event, pending)
        pending.remove(event)

        # Add one more.
        pending.append(7)
        event_task = asyncio.create_task(self.add_events(loop, [7]))
        events = []
        for _ in range(3):
            events.append(await loop.wait_for_event(timeout=self.TIMEOUT))
        await event_task
        self.assertEqual(set(events), set(pending))
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

        # Many.
        pending = list(range(100))
        event_task = asyncio.create_task(self.add_events(loop, pending))
        events = []
        for _ in range(100):
            events.append(await loop.wait_for_event(timeout=self.TIMEOUT))
        await event_task
        self.assertEqual(set(events), set(pending))
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

    async def test_duplicate(self):
        """ Check for duplicate suppression """
        loop = AEventLoop()
        pending = [1, 2, 3, 2, 3, 3, 1, 1, 2]
        await asyncio.create_task(self.add_events(loop, pending))
        events = []
        for _ in range(3):
            events.append(await loop.wait_for_event(timeout=self.TIMEOUT))
        self.assertEqual(set(events), set(pending))
        event = await loop.wait_for_event(timeout=0.0)
        self.assertIsNone(event)

if __name__ == '__main__':
    unittest.main()
