"""
Select Endpoint Unit Tests
"""

import unittest

from selectors import DefaultSelector, EVENT_READ, EVENT_WRITE
from typing import ClassVar

from events.select_endpoint import SelectEndpoint

class TestSelectEndpoint(unittest.TestCase):
    """ Select Endpoint Unit Tests """
    TEST_DATA : ClassVar[bytes] = b"Hello, World!\n"

    E_READ : ClassVar[int] = 1
    E_WRITE : ClassVar[int] = 2

    def test_pipe(self):
        """ Create and use a pipe """
        with DefaultSelector() as select_loop:
            with SelectEndpoint(
                select_loop, read_data=self.E_READ, write_data=self.E_WRITE
            ) as endpoint:
                endpoint.register_read()
                endpoint.register_write()

                # A write event is expected.
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                self.assertEqual(key.data, self.E_WRITE)
                self.assertEqual(event, EVENT_WRITE)
                endpoint.write()
                self.assertIsNone(endpoint.writer_key)

                # Read/write.
                endpoint.write(self.TEST_DATA)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                self.assertEqual(key.data, self.E_READ)
                self.assertEqual(event, EVENT_READ)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertEqual(data, self.TEST_DATA)
                self.assertIsNotNone(endpoint.reader_key)
                self.assertIsNone(endpoint.writer_key)

                # EOF.
                endpoint.read_only()
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                self.assertEqual(key.data, self.E_READ)
                self.assertEqual(event, EVENT_READ)
                endpoint.read()
                data = endpoint.fetch_input()
                self.assertIsNone(data)
                self.assertIsNone(endpoint.reader_key)
                self.assertIsNone(endpoint.writer_key)

if __name__ == '__main__':
    unittest.main()
