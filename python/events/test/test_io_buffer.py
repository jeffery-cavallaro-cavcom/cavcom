"""
I/O Buffer Unit Tests
"""

import unittest

from typing import ClassVar

from events.io_buffer import IOBuffer

class TestIOBuffer(unittest.TestCase):
    """ I/O Buffer Unit Tests """
    TEXT : ClassVar[str] = "\u00A1Hasta Ma\u00F1ana!"
    BYTES : ClassVar[bytes] = TEXT.encode()

    def test_bytes(self):
        """ Append and fetch bytes """
        buffer = IOBuffer()
        self.assertIsNone(buffer.fetch_bytes())

        part = self.BYTES[:5]
        buffer.append(part)
        self.assertEqual(buffer.fetch_bytes(), part)
        self.assertEqual(buffer.fetch_bytes(), part)

        buffer.append(self.BYTES[5:])
        self.assertEqual(buffer.fetch_bytes(), self.BYTES)
        self.assertEqual(buffer.fetch_bytes(reset=True), self.BYTES)
        self.assertIsNone(buffer.fetch_bytes())

        buffer.append(self.BYTES)
        self.assertEqual(buffer.fetch_bytes(reset=True), self.BYTES)
        self.assertIsNone(buffer.fetch_bytes())

    def test_text(self):
        """ Append and fetch as text """
        buffer = IOBuffer()
        self.assertIsNone(buffer.fetch_bytes())

        part = self.BYTES[:5]
        buffer.append(part)
        self.assertEqual(buffer.fetch_text(), self.TEXT[:4])
        self.assertEqual(buffer.fetch_text(), self.TEXT[:4])

        buffer.append(self.BYTES[5:])
        self.assertEqual(buffer.fetch_text(), self.TEXT)
        self.assertEqual(buffer.fetch_text(reset=True), self.TEXT)
        self.assertIsNone(buffer.fetch_text())

        buffer.append(self.BYTES)
        self.assertEqual(buffer.fetch_text(reset=True), self.TEXT)
        self.assertIsNone(buffer.fetch_text())

    def test_decode(self):
        """ Handle a decode error """
        buffer = IOBuffer()
        self.assertIsNone(buffer.fetch_bytes())

        buffer.append(self.BYTES[0:1])
        self.assertIsNone(buffer.fetch_text())
        buffer.append(self.BYTES[1:11])
        self.assertEqual(buffer.fetch_text(), self.TEXT[:9])
        self.assertEqual(buffer.fetch_text(reset=True), self.TEXT[:9])
        self.assertIsNone(buffer.fetch_text())

        buffer.append(self.BYTES[11:])
        self.assertEqual(buffer.fetch_text(reset=True), self.TEXT[9:])
        self.assertIsNone(buffer.fetch_bytes())

if __name__ == '__main__':
    unittest.main()
