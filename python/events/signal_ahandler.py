"""
Asyncio signals only support one handler per signal, which is problematic when
multiple tasks need to be alerted when a signal fires.  This class registers a
signal handler for a particular signal number and calls registered callbacks
when the signal occurs.  Callbacks accept a single opaque argument.

Callbacks should be simple, should not raise any exceptions, and should not
attempt to deregister themselves.  Instead, a callback can return a False value
to indicate that the callback should be deregistered upon return.
"""

import asyncio
from typing import Any, Callable, Optional

class SignalHandler:
    """ Dispatch Signal Delivery """
    Callback = Callable[[Any], bool]

    signo : int
    callbacks : dict[Callback: Any]
    loop : asyncio.AbstractEventLoop

    def __init__(self, signo : int):
        """
        Set the signal handler

        Arguments:
            signo:
                Target signal number to catch.
        """
        self.signo = signo
        self.callbacks = {}
        self.loop = asyncio.get_running_loop()
        self.loop.add_signal_handler(self.signo, self.handler)

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
        self.callbacks[callback] = callback_data

    def deregister_callback(self, callback : Callback) -> None:
        """
        Deregister a callback

        Arguments:
            callback:
                Callback to register.  Does nothing if the callback is not
                registered.
        """
        self.callbacks.pop(callback, None)

    def handler(self) -> None:
        """ Dispatch a caught signal """
        doomed_callback = []

        for callback, callback_data in self.callbacks.items():
            if not callback(callback_data):
                doomed_callback.append(callback)

        for callback in doomed_callback:
            self.callbacks.pop(callback, None)

    def close(self) -> None:
        """ Reset signal handler """
        self.loop.remove_signal_handler(self.signo)
