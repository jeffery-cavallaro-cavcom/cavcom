"""
Event Handler Unit Tests
"""

import unittest

from selectors import DefaultSelector, EVENT_READ
from typing import ClassVar

from events.event_handler import EventHandler

class TestEventHandler(unittest.TestCase):
    """ Event Handler Unit Tests """
    EVENT_DATA : ClassVar[int] = 42

    selectors: DefaultSelector
    event_handler : EventHandler

    def __init__(self, *args, **kwargs):
        """ Define test variables """
        super().__init__(*args, **kwargs)
        self.selectors = None
        self.event_handler = None

    def setUp(self):
        """ Set up the event loop """
        self.selectors = DefaultSelector()
        self.event_handler = EventHandler(
            self.selectors, event_data=self.EVENT_DATA
        )

    def tearDown(self):
        """ Close the event loop """
        if self.event_handler:
            self.event_handler.close()
            self.event_handler = None

        if self.selectors:
            self.selectors.close()
            self.selectors = None

    def test_events(self):
        """ Trigger events """
        nevents = 5

        for _ in range(nevents):
            self.event_handler.trigger()
            events = self.selectors.select(timeout=5)
            self.assertEqual(len(events), 1)
            event = events[0]
            self.assertEqual(event[0].data, self.EVENT_DATA)
            self.assertEqual(event[1], EVENT_READ)
            was_pending = self.event_handler.acknowledge()
            self.assertTrue(was_pending)

    def test_no_pending(self):
        """ Check for untriggered event """
        was_pending = self.event_handler.acknowledge()
        self.assertFalse(was_pending)

    def test_smash(self):
        """ Multiple triggers, one event """
        for _ in range(5):
            self.event_handler.trigger()

        events = self.selectors.select(timeout=5)
        self.assertEqual(len(events), 1)
        event = events[0]
        self.assertEqual(event[0].data, self.EVENT_DATA)
        self.assertEqual(event[1], EVENT_READ)
        was_pending = self.event_handler.acknowledge()
        self.assertTrue(was_pending)

        events = self.selectors.select(timeout=0)
        self.assertEqual(len(events), 0)

if __name__ == '__main__':
    unittest.main()
