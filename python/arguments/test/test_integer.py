""" Integer Type Unit Tests """

import unittest

from argparse import ArgumentTypeError
from typing import ClassVar

from arguments.integer import Integer

class TestInteger(unittest.TestCase):
    """ Integer Type Unit Tests """
    VALUE : ClassVar[int] = 42

    def test_valid_no_checks(self):
        """ Valid value with no range checks """
        value = Integer()(str(self.VALUE))
        self.assertEqual(value, self.VALUE)

    def test_invalid(self):
        """ Invalid value """
        values = [None, '', '    ', 'xxx']

        for value in values:
            with self.assertRaises(ArgumentTypeError) as error:
                Integer()(value)

            self.assertEqual(
                str(error.exception), 'must be a decimal integer value'
            )

    def test_valid_checks(self):
        """ Valid value with range checks """
        str_value = str(self.VALUE)

        value = Integer(min_value=0)(str_value)
        self.assertEqual(value, self.VALUE)

        value = Integer(max_value=100)(str_value)
        self.assertEqual(value, self.VALUE)

        value = Integer(min_value=0, max_value=100)(str_value)
        self.assertEqual(value, self.VALUE)

    def test_fail_checks(self):
        """ Valid value that fails range checks """
        str_value = str(self.VALUE)

        with self.assertRaises(ArgumentTypeError) as error:
            Integer(min_value=43)(str_value)

        self.assertEqual(str(error.exception), 'must be >= 43')

        with self.assertRaises(ArgumentTypeError) as error:
            Integer(max_value=41)(str_value)

        self.assertEqual(str(error.exception), 'must be <= 41')

        with self.assertRaises(ArgumentTypeError) as error:
            Integer(min_value=0, max_value=41)(str_value)

        self.assertEqual(str(error.exception), 'must be between 0 and 41')

if __name__ == '__main__':
    unittest.main()
