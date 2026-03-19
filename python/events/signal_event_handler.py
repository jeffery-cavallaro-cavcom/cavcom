"""
This class extends the event handler class for select loop signal events.  The
event trigger() method is registered with a signal handler dispatcher.
"""

from selectors import DefaultSelector
from typing import Any, Optional

from events.signal_handler import SignalHandler
from events.event_handler import EventHandler

class SignalEventHandler(EventHandler):
    """ Signal Events """
    dispatcher : SignalHandler

    def __init__(
        self,
        dispatcher : SignalHandler,
        selectors : DefaultSelector,
        event_data : Optional[Any] = None
    ):
        """
        Create and register a new signal event handler

        Arguments:
            dispatcher:
                Process-global signal dispatcher singleton.  Note that the
                target signal number is inherent to this instance.
            selectors:
                Target select loop to which the event is registered.
            data:
                Opaque event data that is included in the triggered event's
                SelectKey.  This is normally an event ID.
        """
        super().__init__(selectors, data=event_data)
        self.dispatcher = dispatcher
        self.dispatcher.register_callback(self.trigger_signal, None)

    def trigger_signal(self, _data) -> None:
        """ Match call signature """
        super().trigger()
        return True  # never auto-deregister

    def close(self) -> None:
        """ Unregister signal event """
        self.dispatcher.deregister_callback(self.trigger_signal)
        super().close()
