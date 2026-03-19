"""
Signals only support one handler per signal, which is problematic when multiple
threads need to be alerted when a signal fires.  This class registers a signal
handler for a particular signal number and calls registered callbacks when the
signal occurs.  Callbacks accept a single opaque argument.  Note that per the
python signal package documentation, handlers always run in the main thread, so
something like the file descriptor write trick must be used to communicate
directly with another thread.

Instances of this class are thread-safe upon initialization and are meant to be
global singletons.  The various callbacks are called under lock, so they should
be simple and not attempt to deregister themselves, or a deadlock will occur.
Instead, a callback can return a False value to indicate that the callback
should be deregistered upon return.  Also, per the python signal package
documentation, the callbacks should not raise any exceptions.
"""

from signal import signal, Handlers
from types import FrameType
from threading import Lock
from typing import Any, Callable, Optional, Union

class SignalHandler:
    """ Dispatch Signal Delivery """
    Callback = Callable[[Any], bool]

    signo : int
    callbacks : dict[Callback: Any]
    lock : Lock
    old_handler : Union[Callback, Handlers]

    def __init__(self, signo : int):
        """
        Set the signal handler

        Arguments:
            signo:
                Target signal number to catch.
        """
        self.signo = signo
        self.callbacks = {}
        self.lock = Lock()
        self.old_handler = signal(self.signo, self.handler)

    def register_callback(
        self, callback : Callback, callback_data : Optional[Any] = None
    ) -> None:
        """
        Register a callback

        Arguments:
            callback:
                Callback to call when the target signal fires.  If the callback
                is already registered then it is replaced, useful for when the
                callback argument changes.
            callback_data:
                Opaque argument passed to callback.

        """
        with self.lock:
            self.callbacks[callback] = callback_data

    def deregister_callback(self, callback : Callback) -> None:
        """
        Deregister a callback

        Arguments:
            callback:
                Callback to register.  Does nothing if the callback is not
                registered.
        """
        with self.lock:
            self.callbacks.pop(callback, None)

    def handler(
        self, _signo : int, _frame : Optional[FrameType] = None
    ) -> None:
        """
        Dispatch a caught signal

        Arguments:
            signo:
                Caught signal number.
            frame:
                Optional interrupted call frame information.
        """
        doomed = []

        with self.lock:
            for callback, callback_data in self.callbacks.items():
                if not callback(callback_data):
                    doomed.append(callback)

            for callback in doomed:
                self.callbacks.pop(callback, None)

    def close(self) -> None:
        """ Reset the signal handler """
        signal(self.signo, self.old_handler)
