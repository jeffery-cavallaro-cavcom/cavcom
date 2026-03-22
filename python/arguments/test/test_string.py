""" String Type Unit Tests """

import unittest

from argparse import ArgumentTypeError
from typing import ClassVar

from arguments.string import String

class TestInteger(unittest.TestCase):
    """ Integer Type Unit Tests """
    VALUE : ClassVar[int] = 'hello'
    TYPE_ERROR : ClassVar[str] = 'must be a string value'
    BLANK_ERROR : ClassVar[str] = 'may not be a blank string'

    def test_valid_strip_no_blank(self):
        """ Valid value with strip and no blank """
        values = [
            self.VALUE,
            ' ' + self.VALUE,
            self.VALUE + '\n',
            "   \t " + self.VALUE + "   \r\n"
        ]

        for value in values:
            result = String()(value)
            self.assertEqual(result, self.VALUE)

    def test_invalid_strip_no_blank(self):
        """ Invalid value with strip and no blank """
        values = {
            None: self.TYPE_ERROR,
            '': self.BLANK_ERROR,
            '  \t  \r\n': self.BLANK_ERROR,
            42: self.TYPE_ERROR
        }

        for value, result in values.items():
            with self.assertRaises(ArgumentTypeError) as error:
                String()(value)

            self.assertEqual(str(error.exception), result)

    def test_valid_no_strip_no_blank(self):
        """ Valid value with no strip and no blank """
        values = [
            self.VALUE,
            ' ' + self.VALUE,
            self.VALUE + '\n',
            "   \t " + self.VALUE + "   \r\n"
        ]

        for value in values:
            result = String(no_strip=True)(value)
            self.assertEqual(result, value)

    def test_invalid_no_strip_no_blank(self):
        """ Invalid value with strip and no blank """
        values = {
            None: self.TYPE_ERROR,
            '': self.BLANK_ERROR,
            42: self.TYPE_ERROR
        }

        for value, result in values.items():
            with self.assertRaises(ArgumentTypeError) as error:
                String(no_strip=True)(value)

            self.assertEqual(str(error.exception), result)

    def test_valid_strip_blank(self):
        """ Valid value with strip and allow blank """
        values = {
            '': '',
            ' ': '',
            '  \t  \n': '',
            self.VALUE: self.VALUE,
            ' ' + self.VALUE: self.VALUE,
            self.VALUE + '\n': self.VALUE,
            "   \t " + self.VALUE + "   \r\n": self.VALUE
        }

        for value, expect in values.items():
            result = String(allow_blank=True)(value)
            self.assertEqual(result, expect)

    def test_invalid_strip_blank(self):
        """ Invalid value with strip and allow blank """
        values = [None, 42]

        for value in values:
            with self.assertRaises(ArgumentTypeError) as error:
                String(allow_blank=True)(value)

            self.assertEqual(str(error.exception), self.TYPE_ERROR)

    def test_valid_no_strip_blank(self):
        """ Valid value with no strip and allow blank """
        values = [
            '',
            ' ',
            '  \t  \r\n',
            self.VALUE,
            ' ' + self.VALUE,
            self.VALUE + '\n',
            "   \t " + self.VALUE + "   \r\n"
        ]

        for value in values:
            result = String(no_strip=True, allow_blank=True)(value)
            self.assertEqual(result, value)

    def test_invalid_no_strip_blank(self):
        """ Invalid value with no strip and allow blank """
        values = [None, 42]

        for value in values:
            with self.assertRaises(ArgumentTypeError) as error:
                String(no_strip=True, allow_blank=True)(value)

            self.assertEqual(str(error.exception), self.TYPE_ERROR)

if __name__ == '__main__':
    unittest.main()
