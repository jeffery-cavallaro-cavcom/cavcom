"""
Signal Handler Unit Tests

The goal is to deliver one or more signal events to one or more threads via
event file descriptors.  To avoid race conditions, the main thread and event
threads are synchronized by a "ready" state maintained in the event threads.
Upon startup, an event threads sets its ready state, indicating to the main
thread that it is ready to accept events.  It also sets its ready state after
it has processed a signal event.  An exit signal handler is used to terminate
all threads.
"""

import unittest

import os
from selectors import DefaultSelector, EVENT_READ
from signal import SIGUSR1, SIGUSR2, raise_signal
from threading import Thread, Lock
from time import sleep
from typing import ClassVar, Optional

from osx.signal_handler import SignalHandler

class CallbackThread(Thread):
    """ Count Callback Calls """
    count : int
    event_fd : int
    exit_fd : int
    deregister : bool
    ready : bool
    ready_lock : Lock

    def __init__(self, deregister : Optional[bool] = False):
        """ Initialize count """
        super().__init__()
        self.count = 0
        self.event_fd = None
        self.exit_fd = None
        self.deregister = bool(deregister)
        self.ready = False
        self.ready_lock = Lock()

    def run(self) -> None:
        """ Wait for events """
        self.event_fd = None
        self.exit_fd = None

        try:
            self.event_fd = os.eventfd(0, flags=os.EFD_NONBLOCK)
            self.exit_fd = os.eventfd(0, flags=os.EFD_NONBLOCK)

            with DefaultSelector() as selector:
                selector.register(self.event_fd, EVENT_READ)
                selector.register(self.exit_fd, EVENT_READ)

                self.set_ready()

                more = True

                while more:
                    events = selector.select()
                    for event in events:
                        if event[0].fd == self.event_fd:
                            increment = os.eventfd_read(self.event_fd)
                            self.count += increment
                            self.set_ready()

                        elif event[0].fd == self.exit_fd:
                            os.eventfd_read(self.exit_fd)
                            more = False
        finally:
            self.close()

    def set_ready(self) -> None:
        """ Indicate ready or incremented """
        with self.ready_lock:
            self.ready = True

    def is_ready(self) -> bool:
        """ Check for ready condition """
        with self.ready_lock:
            ready = self.ready
            self.ready = False
            return ready

    def trigger(self, increment : int) -> bool:
        """
        Trigger event

        Arguments:
            increment:
                Amount to increment the counter.  Must be > 0.

        Returns:
            True if the callback is to be auto-deregistered.
        """
        os.eventfd_write(self.event_fd, increment)
        return not self.deregister

    def terminate(self, _data) -> bool:
        """ Terminate thread """
        os.eventfd_write(self.exit_fd, 1)
        return True

    def close(self) -> None:
        """ Close all event descriptors """
        if self.event_fd is not None:
            os.close(self.event_fd)
            self.event_fd = None

        if self.exit_fd is not None:
            os.close(self.exit_fd)
            self.exit_fd = None

class TestSignalHandler(unittest.TestCase):
    """ Signal Handler Unit Tests """
    EVENTNO : ClassVar[int] = SIGUSR1
    EXITNO : ClassVar[int] = SIGUSR2

    dispatcher : SignalHandler
    terminator : SignalHandler

    def __init__(self, *args, **kwargs):
        """ Initialize dispatcher """
        super().__init__(*args, **kwargs)
        self.dispatcher = None
        self.terminator = None

    def setUp(self):
        """ Register the dispatcher """
        super().setUp()
        self.dispatcher = SignalHandler(self.EVENTNO)
        self.terminator = SignalHandler(self.EXITNO)

    def tearDown(self):
        """ Deregister the dispatcher """
        if self.dispatcher:
            self.dispatcher.close()
            self.dispatcher = None

        if self.terminator:
            self.terminator.close()
            self.terminator = None

        super().tearDown()

    def start_thread(
        self,
        increment : Optional[int] = 1,
        deregister : Optional[bool] = False
    ) -> CallbackThread:
        """
        Create and start a callback thread

        Arguments:
            increment:
                Increment value.
            deregister:
                Auto-deregister event after first occurrence.

        Returns:
            Created and started callback thread.
        """
        thread = CallbackThread(deregister=deregister)
        self.dispatcher.register_callback(thread.trigger, increment)
        self.terminator.register_callback(thread.terminate)
        thread.start()

        return thread

    @staticmethod
    def check_thread(thread : CallbackThread) -> None:
        """
        Check for ready thread

        Arguments:
            thread:
                Callback thread to check for ready condition.
        """
        while not thread.is_ready():
            sleep(0)

    def test_none(self):
        """ No registered callbacks """
        raise_signal(self.EVENTNO)
        raise_signal(self.EXITNO)

    def test_one(self):
        """ One registered callback """
        thread = self.start_thread(increment=5)
        self.check_thread(thread)
        self.assertEqual(thread.count, 0)

        raise_signal(self.EVENTNO)
        self.check_thread(thread)
        self.assertEqual(thread.count, 5)

        raise_signal(self.EXITNO)
        thread.join()
        self.assertEqual(thread.count, 5)

    def test_many(self):
        """ Many registered callbacks """
        nthreads = 10
        nsignals = 5

        threads = []

        for ithread in range(nthreads):
            thread = self.start_thread(increment=ithread + 1)
            self.check_thread(thread)
            threads.append(thread)

        for _ in range(nsignals):
            raise_signal(self.EVENTNO)
            for thread in threads:
                self.check_thread(thread)

        raise_signal(self.EXITNO)

        for thread in threads:
            thread.join()

        for ithread, thread in enumerate(threads):
            self.assertEqual(thread.count, 5*(ithread + 1))

    def test_deregister(self):
        """ Explicit deregistration """
        thread_1 = self.start_thread()
        thread_2 = self.start_thread()
        self.check_thread(thread_1)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 0)
        self.assertEqual(thread_2.count, 0)

        raise_signal(self.EVENTNO)
        self.check_thread(thread_1)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 1)

        self.dispatcher.deregister_callback(thread_1.trigger)
        raise_signal(self.EVENTNO)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 2)

        raise_signal(self.EXITNO)
        thread_1.join()
        thread_2.join()
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 2)

    def test_auto_deregister(self):
        """ Automatic deregistration """
        thread_1 = self.start_thread(deregister=True)
        thread_2 = self.start_thread()
        self.check_thread(thread_1)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 0)
        self.assertEqual(thread_2.count, 0)

        raise_signal(self.EVENTNO)
        self.check_thread(thread_1)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 1)

        raise_signal(self.EVENTNO)
        self.check_thread(thread_2)
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 2)

        raise_signal(self.EXITNO)
        thread_1.join()
        thread_2.join()
        self.assertEqual(thread_1.count, 1)
        self.assertEqual(thread_2.count, 2)

if __name__ == '__main__':
    unittest.main()
