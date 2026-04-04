"""
Select Endpoint Unit Tests
"""

import unittest

from errno import EIO
import os
from selectors import DefaultSelector
from typing import ClassVar

from events.select_endpoint import SelectEndpoint
from events.pty_manager import PTYManager

class TestSelectEndpoint(unittest.TestCase):
    """ Select Endpoint Unit Tests """
    TEST_SDATA_1 : ClassVar[bytes] = b"Hello, World!\n"
    TEST_SDATA_2 : ClassVar[bytes] = b"The answer is 42\n"

    E_READ_1 : ClassVar[int] = 1
    E_WRITE_1 : ClassVar[int] = 2
    E_READ_2 : ClassVar[int] = 3
    E_WRITE_2 : ClassVar[int] = 4

    def test_pipe(self):
        """ Create and use a pipe """
        reader_side, writer_side = os.pipe()

        with DefaultSelector() as select_loop:
            try:
                sreader = SelectEndpoint(
                    select_loop,
                    reader_side,
                    read_data=self.E_READ_1,
                    write_data=None
                )
                swriter = SelectEndpoint(
                    select_loop,
                    writer_side,
                    read_data=None,
                    write_data=self.E_WRITE_2
                )
                sreader.register_read()
                swriter.register_write()

                self.assertTrue(sreader.is_open)
                self.assertTrue(sreader.reading)
                self.assertFalse(sreader.writing)

                self.assertTrue(swriter.is_open)
                self.assertFalse(swriter.reading)
                self.assertTrue(swriter.writing)

                # A write event is expected.
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_WRITE_2)
                swriter.write()
                self.assertFalse(swriter.writing)

                # Read/write.
                swriter.write(self.TEST_SDATA_1)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_1)
                sreader.read()
                data = sreader.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_1)
                self.assertTrue(sreader.reading)
                self.assertFalse(swriter.writing)

                # EOF.
                swriter.close()
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_1)
                sreader.read()
                data = sreader.fetch_input()
                self.assertIsNone(data)
                self.assertFalse(sreader.reading)
                self.assertFalse(swriter.writing)
            finally:
                sreader.close()
                swriter.close()

    def test_pty(self):
        """ Allocate and use a PTY """
        # pylint: disable=too-many-statements
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with DefaultSelector() as select_loop:
                smaster = SelectEndpoint(
                    select_loop,
                    pty.master,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                )
                sslave = SelectEndpoint(
                    select_loop,
                    pty.slave,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                )

                smaster.register_read()
                smaster.register_write()
                sslave.register_read()

                self.assertTrue(smaster.is_open)
                self.assertTrue(smaster.reading)
                self.assertTrue(smaster.writing)

                self.assertTrue(sslave.is_open)
                self.assertTrue(sslave.reading)
                self.assertFalse(sslave.writing)

                # A master write event is expected.
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_WRITE_1)
                smaster.write()
                self.assertFalse(smaster.writing)

                # Read/write.
                smaster.write(self.TEST_SDATA_1)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_2)
                sslave.read()
                data = sslave.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_1)

                sslave.write(self.TEST_SDATA_2)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_1)
                smaster.read()
                data = smaster.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_2)

                # EOF
                smaster.write(b"\x04")  # ^D
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_2)
                sslave.read()
                data = sslave.fetch_input()
                self.assertIsNone(data)
                self.assertFalse(sslave.is_open)
                pty.slave = None

    def test_both(self):
        """ Read and write event on the same endpoint """
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with DefaultSelector() as select_loop:
                smaster = SelectEndpoint(
                    select_loop,
                    pty.master,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                )
                sslave = SelectEndpoint(
                    select_loop,
                    pty.slave,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                )

                smaster.register_read()
                smaster.register_write()

                self.assertTrue(smaster.is_open)
                self.assertTrue(smaster.reading)
                self.assertTrue(smaster.writing)

                sslave.write(self.TEST_SDATA_1)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                data = []
                for event in events:
                    data.extend(SelectEndpoint.get_events(*event))
                self.assertEqual(len(data), 2)
                self.assertIn(self.E_READ_1, data)
                self.assertIn(self.E_WRITE_1, data)

                smaster.write()
                smaster.read()

                data = smaster.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_1)

                self.assertTrue(smaster.reading)
                self.assertFalse(smaster.writing)
                self.assertFalse(sslave.reading)
                self.assertFalse(sslave.writing)

    def test_read_error(self):
        """ Force a read error """
        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with DefaultSelector() as select_loop:
                smaster = SelectEndpoint(
                    select_loop,
                    pty.master,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                )
                sslave = SelectEndpoint(
                    select_loop,
                    pty.slave,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                )

                smaster.register_read()
                sslave.register_read()

                # Read/write.
                smaster.write(self.TEST_SDATA_1)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_2)
                sslave.read()
                data = sslave.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_1)

                sslave.write(self.TEST_SDATA_2)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_1)
                smaster.read()
                data = smaster.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_2)

                # Close the slave.
                sslave.close()
                pty.slave = None

                smaster.read()
                data = smaster.fetch_input()
                self.assertIsNone(data)

                self.assertFalse(smaster.is_open)
                pty.master = None
                self.assertFalse(smaster.reading)
                self.assertFalse(smaster.writing)
                self.assertIsNone(smaster.input_data)
                self.assertIsNone(smaster.output_data)
                self.assertEqual(smaster.read_errno, EIO)
                self.assertEqual(
                    smaster.read_error_text, os.strerror(smaster.read_errno)
                )
                self.assertIsNone(smaster.write_errno)

    def test_write_error(self):
        """ Force a write error """

        with PTYManager() as pty:
            pty.disable_echo_crlf()
            pty.set_nonblocking()

            with DefaultSelector() as select_loop:
                smaster = SelectEndpoint(
                    select_loop,
                    pty.master,
                    read_data=self.E_READ_1,
                    write_data=self.E_WRITE_1
                )
                sslave = SelectEndpoint(
                    select_loop,
                    pty.slave,
                    read_data=self.E_READ_2,
                    write_data=self.E_WRITE_2
                )

                smaster.register_read()
                sslave.register_read()

                # Read/write.
                smaster.write(self.TEST_SDATA_1)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_2)
                sslave.read()
                data = sslave.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_1)

                sslave.write(self.TEST_SDATA_2)
                events = select_loop.select()
                self.assertEqual(len(events), 1)
                key, event = events[0]
                data = SelectEndpoint.get_events(key, event)
                self.assertEqual(len(data), 1)
                self.assertEqual(data[0], self.E_READ_1)
                smaster.read()
                data = smaster.fetch_input()
                self.assertEqual(data, self.TEST_SDATA_2)

                # Close the master.
                smaster.close()
                pty.master = None
                sslave.write(self.TEST_SDATA_1)

                self.assertFalse(sslave.is_open)
                pty.slave = None

                self.assertFalse(sslave.reading)
                self.assertFalse(sslave.writing)
                self.assertIsNone(sslave.input_data)
                self.assertEqual(sslave.output_data, self.TEST_SDATA_1)
                self.assertIsNone(sslave.read_errno)
                self.assertEqual(sslave.write_errno, EIO)
                self.assertEqual(
                    sslave.write_error_text, os.strerror(sslave.write_errno)
                )

if __name__ == '__main__':
    unittest.main()
