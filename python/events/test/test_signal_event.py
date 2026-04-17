"""
Signal Event Unit Tests
"""

import unittest

from selectors import DefaultSelector
from signal import SIGUSR1, SIGUSR2, raise_signal
from typing import ClassVar

from events.signal_event import SignalEvent

class TestSignalEvent(unittest.TestCase):
    """ Signal Event Handler Unit Tests """
    EVENT_SIGNAL : ClassVar[int] = SIGUSR1
    EXIT_SIGNAL : ClassVar[int] = SIGUSR2

    E_EVENT : ClassVar[int] = 42
    E_EXIT : ClassVar[int] = 100

    def test_signal(self):
        """ Check for signal event """
        selectors = None
        event_handler = None
        exit_handler = None

        try:
            selectors = DefaultSelector()
            event_handler = SignalEvent(
                self.EVENT_SIGNAL, selectors, event_data=self.E_EVENT
            )
            exit_handler = SignalEvent(
                self.EXIT_SIGNAL, selectors, event_data=self.E_EXIT
            )

            events = selectors.select(timeout=0)
            self.assertEqual(len(events), 0)

            raise_signal(self.EVENT_SIGNAL)
            events = selectors.select(timeout=5)
            self.assertEqual(len(events), 1)
            event_id = events[0][0].data
            self.assertEqual(event_id, self.E_EVENT)
            event_handler.acknowledge()

            events = selectors.select(timeout=0)
            self.assertEqual(len(events), 0)

            raise_signal(self.EVENT_SIGNAL)
            events = selectors.select(timeout=5)
            self.assertEqual(len(events), 1)
            event_id = events[0][0].data
            self.assertEqual(event_id, self.E_EVENT)
            event_handler.acknowledge()

            raise_signal(self.EXIT_SIGNAL)
            events = selectors.select(timeout=5)
            self.assertEqual(len(events), 1)
            event_id = events[0][0].data
            self.assertEqual(event_id, self.E_EXIT)
            exit_handler.acknowledge()

            events = selectors.select(timeout=0)
            self.assertEqual(len(events), 0)
        finally:
            if event_handler:
                event_handler.close()

            if exit_handler:
                exit_handler.close()

            if selectors:
                selectors.close()

if __name__ == '__main__':
    unittest.main()
