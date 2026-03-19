"""
Timer Event Unit Tests
"""

import unittest

from selectors import DefaultSelector
from typing import ClassVar

from events.timer_event import TimerEvent

class TestTimerEvent(unittest.TestCase):
    """ Timer Event Unit Tests """
    EVENT_DATA : ClassVar[int] = 42

    def test_one(self):
        """ One timer """
        with DefaultSelector() as selectors:
            timer = None

            try:
                timer = TimerEvent(
                    1.0, selectors=selectors, event_data=self.EVENT_DATA
                )
                timer.start()
                events = selectors.select(timeout=5.0)
                self.assertEqual(len(events), 1)
                self.assertEqual(events[0][0].data, self.EVENT_DATA)
                self.assertTrue(timer.acknowledge())

                events = selectors.select(timeout=0)
                self.assertEqual(len(events), 0)
            finally:
                timer.close()

    def test_two(self):
        """ Two different timers """
        with DefaultSelector() as selectors:
            timer_1 = None
            timer_2 = None
            timer_1_fired = False
            timer_2_fired = False

            try:
                timer_1 = TimerEvent(
                    1.0, selectors=selectors, event_data=self.EVENT_DATA
                )
                timer_2 = TimerEvent(
                    2.0, selectors=selectors, event_data=self.EVENT_DATA + 1
                )
                timer_1.start()
                timer_2.start()

                while not timer_1_fired or not timer_2_fired:
                    events = selectors.select(timeout=5.0)
                    self.assertGreater(len(events), 0)
                    for event in events:
                        event_data = event[0].data
                        if event_data == self.EVENT_DATA:
                            self.assertTrue(timer_1.acknowledge())
                            timer_1_fired = True
                        elif event_data == self.EVENT_DATA + 1:
                            self.assertTrue(timer_2.acknowledge())
                            timer_2_fired = True

                events = selectors.select(timeout=0)
                self.assertEqual(len(events), 0)
            finally:
                timer_1.close()
                timer_2.close()

    def test_many(self):
        """ Many identical timers """
        ntimers = 10

        timers : dict[int, TimerEvent] = {}  # event_data: timer
        fired : set[int] = set()  # event_data

        with DefaultSelector() as selectors:
            try:
                for itimer in range(ntimers):
                    event_data = self.EVENT_DATA + itimer
                    timer = TimerEvent(
                        1.0, selectors=selectors, event_data=event_data
                    )
                    timer.start()
                    timers[event_data] = timer

                while len(fired) < ntimers:
                    events = selectors.select(timeout=5.0)
                    self.assertGreater(len(events), 0)
                    for event in events:
                        event_data = event[0].data
                        self.assertTrue(timers[event_data].acknowledge())
                        fired.add(event_data)

                events = selectors.select(timeout=0)
                self.assertEqual(len(events), 0)
            finally:
                for timer in timers.values():
                    timer.close()

if __name__ == '__main__':
    unittest.main()
