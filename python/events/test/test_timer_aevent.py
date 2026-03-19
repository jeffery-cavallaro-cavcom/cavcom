"""
Asynchronous Timer Event Unit Tests
"""

import unittest

import asyncio
from typing import ClassVar

from events.timer_aevent import TimerEvent

class CallbackTask:
    """ Count Callback Calls """
    E_EVENT : ClassVar[int] = 42

    task_id : int
    count : int
    next_event : asyncio.Future

    def __init__(self):
        """ Initialize count """
        self.count = 0
        self.next_event = None

    async def run(self, event_id : int) -> None:
        """
        Wait for event

        Arguments:
            event_id:
                Expected event ID.
        """
        self.next_event = asyncio.Future()
        await self.next_event
        next_event_id = self.next_event.result()

        if event_id == next_event_id:
            self.count += 1

    def trigger(self, event_id : int) -> None:
        """
        Trigger event

        Arguments:
            event_id:
                Next event ID (E_xxxx).
        """
        self.next_event.set_result(event_id)

class TestTimerEvent(unittest.IsolatedAsyncioTestCase):
    """ Timer Event Unit Tests """
    EVENT_ID : ClassVar[int] = 42

    async def test_one(self):
        """ One timer """
        timer = None

        try:
            callback = CallbackTask()
            task = asyncio.create_task(callback.run(self.EVENT_ID))
            self.assertEqual(callback.count, 0)

            timer = TimerEvent(
                1.0, callback.trigger, event_data=self.EVENT_ID
            )
            timer.start()

            await task
            self.assertEqual(callback.count, 1)
        finally:
            timer.close()

    async def test_two(self):
        """ Two different timers """
        tasks : dict[asyncio.Task, tuple[CallbackTask, TimerEvent]] = {}

        try:
            for itask in range(2):
                event_id = self.EVENT_ID + itask
                callback = CallbackTask()
                task = asyncio.create_task(callback.run(event_id))
                self.assertEqual(callback.count, 0)
                timer = TimerEvent(
                    itask + 1, callback.trigger, event_data=event_id
                )
                tasks[task] = (callback, timer)
                timer.start()

            for task, callback in tasks.items():
                await task
                self.assertEqual(callback[0].count, 1)
        finally:
            for callback in tasks.values():
                callback[1].close()

    async def test_many(self):
        """ Many identical timers """
        tasks : dict[asyncio.Task, tuple[CallbackTask, TimerEvent]] = {}

        try:
            for itask in range(10):
                event_id = self.EVENT_ID + itask
                callback = CallbackTask()
                task = asyncio.create_task(callback.run(event_id))
                self.assertEqual(callback.count, 0)
                timer = TimerEvent(1, callback.trigger, event_data=event_id)
                tasks[task] = (callback, timer)
                timer.start()

            for task, callback in tasks.items():
                await task
                self.assertEqual(callback[0].count, 1)
        finally:
            for callback in tasks.values():
                callback[1].close()

if __name__ == '__main__':
    unittest.main()
