"""
This class provides a simple and thread-safe method for a main test thread to
synchronize with other test threads.  This is normally needed when the main
test thread does something (e.g., trigger events) that causes the test threads
to change state, but the main test thread cannot continue until it is sure that
the other test threads' state changes are complete.  When a main thread needs to
wait for another test thread to change state, it calls the is_ready() method.
When the test thread is ready, it calls the set_ready() method.
"""

from threading import Lock

class ThreadSync:
    """ Synchronize Threads """
    ready : bool
    lock : Lock

    def __init__(self):
        """ Initialize the ready state and lock """
        self.ready = False
        self.lock = Lock()

    def set_ready(self) -> None:
        """ Indicate ready or incremented """
        with self.lock:
            self.ready = True

    def is_ready(self) -> bool:
        """ Check for ready condition """
        with self.lock:
            ready = self.ready
            self.ready = False
            return ready
