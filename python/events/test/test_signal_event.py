"""
Signal Event Unit Tests

The goal is to deliver one or more signal events to one or more threads.  To
avoid race conditions, the main thread and event threads are synchronized on
startup and event delivery using a ThreadSync instance.  An exit signal handler
is used to terminate all threads.
"""

import unittest

from selectors import DefaultSelector
from signal import SIGUSR1, SIGUSR2, raise_signal
from threading import Thread
from time import sleep
from typing import ClassVar

from events.signal_handler import SignalHandler
from events.signal_event import SignalEvent

from events.test.thread_sync import ThreadSync

EVENT_HANDLER : SignalHandler = None
EXIT_HANDLER : SignalHandler = None

class CallbackThread(Thread):
    """ Count Callback Calls """
    E_EVENT : ClassVar[int] = 42
    E_EXIT : ClassVar[int] = 100

    count : int
    sync : ThreadSync

    def __init__(self):
        """ Initialize count """
        super().__init__()
        self.count = 0
        self.sync = ThreadSync()

    def run(self) -> None:
        """ Wait for events """
        event_handler = None
        exit_handler = None
        selectors = None

        try:
            selectors = DefaultSelector()
            event_handler = SignalEvent(
                EVENT_HANDLER, selectors, event_data=self.E_EVENT
            )
            exit_handler = SignalEvent(
                EXIT_HANDLER, selectors, event_data=self.E_EXIT
            )

            self.sync.set_ready()

            more = True

            while more:
                events = selectors.select()
                for next_event in events:
                    event_id = next_event[0].data
                    if event_id == self.E_EVENT:
                        event_handler.acknowledge()
                        self.count += 1
                        self.sync.set_ready()
                    elif event_id == self.E_EXIT:
                        exit_handler.acknowledge()
                        more = False
        finally:
            if event_handler:
                event_handler.close()
                event_handler = None

            if exit_handler:
                exit_handler.close()
                exit_handler = None

            if selectors:
                selectors.close()
                selectors = None

class TestSignalEvent(unittest.TestCase):
    """ Signal Event Handler Unit Tests """
    EVENT_SIGNAL : ClassVar[int] = SIGUSR1
    EXIT_SIGNAL : ClassVar[int] = SIGUSR2

    def setUp(self):
        """ Register the dispatcher """
        super().setUp()

        # pylint: disable=global-statement
        global EVENT_HANDLER
        global EXIT_HANDLER

        EVENT_HANDLER = SignalHandler(self.EVENT_SIGNAL)
        EXIT_HANDLER = SignalHandler(self.EXIT_SIGNAL)

    def tearDown(self):
        """ Deregister the dispatcher """
        # pylint: disable=global-statement
        global EVENT_HANDLER
        global EXIT_HANDLER

        if EVENT_HANDLER:
            EVENT_HANDLER.close()
            EVENT_HANDLER = None

        if EXIT_HANDLER:
            EXIT_HANDLER.close()
            EXIT_HANDLER = None

        super().tearDown()

    @staticmethod
    def check_thread(thread : CallbackThread) -> None:
        """
        Check for ready thread

        Arguments:
            thread:
                Callback thread to check for ready condition.
        """
        while not thread.sync.is_ready():
            sleep(0)

    def test_none(self):
        """ No registered callbacks """
        raise_signal(self.EVENT_SIGNAL)
        raise_signal(self.EXIT_SIGNAL)

    def test_one(self):
        """ One registered callback """
        thread = CallbackThread()
        thread.start()
        self.check_thread(thread)
        self.assertEqual(thread.count, 0)

        raise_signal(self.EVENT_SIGNAL)
        self.check_thread(thread)
        self.assertEqual(thread.count, 1)

        raise_signal(self.EXIT_SIGNAL)
        thread.join()
        self.assertEqual(thread.count, 1)

    def test_many(self):
        """ Many registered callbacks """
        thread_count = 10
        signal_count = 5

        threads = []

        for _ in range(thread_count):
            thread = CallbackThread()
            thread.start()
            self.check_thread(thread)
            threads.append(thread)

        for _ in range(signal_count):
            raise_signal(self.EVENT_SIGNAL)
            for thread in threads:
                self.check_thread(thread)

        raise_signal(self.EXIT_SIGNAL)

        for thread in threads:
            thread.join()

        for thread in threads:
            self.assertEqual(thread.count, signal_count)

if __name__ == '__main__':
    unittest.main()
