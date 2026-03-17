"""
Asynchronous Signal Handler Unit Tests

The goal is to deliver one or more signal events to one or more tasks, each
awaiting on its own Future.  The signal handler sets the Future value to
indicate that the signal event has occurred, causing the task to increment a
counter.  To avoid race conditions, the main thread synchronizes with the tasks
by checking for the expected count value after each signal event.  An exit
signal handler is used to terminate all tasks.
"""

import unittest

import asyncio
from signal import SIGUSR1, SIGUSR2, raise_signal
from typing import ClassVar, Optional

from osx.signal_ahandler import SignalHandler

class CallbackTask:
    """ Count Callback Calls """
    E_EVENT : ClassVar[int] = 42
    E_EXIT : ClassVar[int] = 100

    task_id : int
    count : int
    next_event : asyncio.Future
    deregister : bool

    def __init__(self, deregister : Optional[bool] = False):
        """ Initialize count """
        self.count = 0
        self.next_event = None
        self.deregister = bool(deregister)

    async def run(self) -> None:
        """ Wait for events """
        more = True

        while more:
            self.next_event = asyncio.Future()
            await self.next_event
            event_id = self.next_event.result()

            if event_id == self.E_EVENT:
                self.count += 1
            elif event_id == self.E_EXIT:
                more = False

    def trigger(self, event_id : int) -> bool:
        """
        Trigger event

        Arguments:
            event_id:
                Next event ID (E_xxxx).

        Returns:
            True if the callback is to be auto-deregistered.
        """
        self.next_event.set_result(event_id)
        return not self.deregister

class TestSignalHandler(unittest.IsolatedAsyncioTestCase):
    """ Signal Handler Unit Tests """
    EVENTNO : ClassVar[int] = SIGUSR1
    EXITNO : ClassVar[int] = SIGUSR2

    adispatcher : SignalHandler
    aterminator : SignalHandler

    def __init__(self, *args, **kwargs):
        """ Initialize dispatcher """
        super().__init__(*args, **kwargs)
        self.adispatcher = None
        self.aterminator = None

    async def asyncSetUp(self):
        """ Register the dispatcher """
        super().setUp()
        self.adispatcher = SignalHandler(self.EVENTNO)
        self.aterminator = SignalHandler(self.EXITNO)

    async def asyncTearDown(self):
        """ Deregister the dispatcher """
        if self.adispatcher:
            self.adispatcher.close()
            self.adispatcher = None

        if self.aterminator:
            self.aterminator.close()
            self.aterminator = None

        super().tearDown()

    def start_task(
        self, deregister : Optional[bool] = False
    ) -> tuple[asyncio.Task, CallbackTask]:
        """
        Create and start a callback task

        Arguments:
            deregister:
                Auto-deregister event after first occurrence.

        Returns:
            Tuple containing the new task and the corresponding CallbackTask
            instance.
        """
        callback = CallbackTask(deregister=deregister)
        self.adispatcher.register_callback(
            callback.trigger, CallbackTask.E_EVENT
        )
        self.aterminator.register_callback(
            callback.trigger, CallbackTask.E_EXIT
        )
        task = asyncio.create_task(callback.run())

        return (task, callback)

    @staticmethod
    async def check_task(callback : CallbackTask, count : int) -> None:
        """
        Synchronize with task based on count

        Arguments:
            callback:
                Callback instance for target task.
            count:
                Expected count before continuing.
        """
        while callback.count != count:
            await asyncio.sleep(0)

    async def test_none(self):
        """ No registered callbacks """
        raise_signal(self.EVENTNO)
        raise_signal(self.EXITNO)

    async def test_one(self):
        """ One registered callback """
        task, callback = self.start_task()
        self.assertEqual(callback.count, 0)

        raise_signal(self.EVENTNO)
        await self.check_task(callback, 1)
        self.assertEqual(callback.count, 1)

        raise_signal(self.EXITNO)
        await task
        self.assertEqual(callback.count, 1)

    async def test_many(self):
        """ Many registered callbacks """
        ntasks = 10
        nsignals = 5

        tasks = {}

        for _ in range(ntasks):
            task, callback = self.start_task()
            tasks[task] = callback

        for isig in range(nsignals):
            raise_signal(self.EVENTNO)
            for task in tasks.values():
                await self.check_task(task, isig + 1)

        raise_signal(self.EXITNO)

        for task in tasks:
            await task

        for task in tasks.values():
            self.assertEqual(task.count, nsignals)

    async def test_deregister(self):
        """ Explicit deregistration """
        task_1, callback_1 = self.start_task()
        task_2, callback_2 = self.start_task()
        self.assertEqual(callback_1.count, 0)
        self.assertEqual(callback_2.count, 0)

        raise_signal(self.EVENTNO)
        await self.check_task(callback_1, 1)
        await self.check_task(callback_2, 1)
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 1)

        self.adispatcher.deregister_callback(callback_1.trigger)
        raise_signal(self.EVENTNO)
        await self.check_task(callback_2, 2)
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 2)

        raise_signal(self.EXITNO)
        await task_1
        await task_2
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 2)

    async def test_auto_deregister(self):
        """ Automatic deregistration """
        task_1, callback_1 = self.start_task(deregister=True)
        task_2, callback_2 = self.start_task()
        self.assertEqual(callback_1.count, 0)
        self.assertEqual(callback_2.count, 0)

        raise_signal(self.EVENTNO)
        await self.check_task(callback_1, 1)
        await self.check_task(callback_2, 1)
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 1)

        raise_signal(self.EVENTNO)
        await self.check_task(callback_2, 2)
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 2)

        raise_signal(self.EXITNO)
        await task_1
        await task_2
        self.assertEqual(callback_1.count, 1)
        self.assertEqual(callback_2.count, 2)

if __name__ == '__main__':
    unittest.main()
