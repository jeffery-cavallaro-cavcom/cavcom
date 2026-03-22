"""
Integer type for argument parsers.  Provides minimum and maximum value checks.
"""

from argparse import ArgumentTypeError
from typing import Optional

class Integer:
    """ Integer Type Argument """
    min_value : int
    max_value : int

    def __init__(
        self,
        min_value : Optional[int] = None,
        max_value : Optional[int] = None
    ) :
        """
        Set allowed value range

        Arguments:
            min_value:
                Minimum allowed value, or no minimum if None.
            max_value:
                Maximum allowed value, or no maximum if None.
        """
        self.min_value = min_value
        self.max_value = max_value

    def __call__(self, value : str) -> int:
        """ Convert and check value """
        try:
            value = int(value)
        except (ValueError, TypeError):
            value = None

        if value is None:
            raise ArgumentTypeError('must be a decimal integer value')

        if (
            self.min_value is not None and value < self.min_value or
            self.max_value is not None and value > self.max_value
        ):
            raise ArgumentTypeError(f"must be {self.range_text()}")

        return value

    def range_text(self) -> str:
        """ Adjust range error text to range """
        if self.min_value is None and self.max_value is not None:
            result = f"<= {self.max_value}"
        elif self.min_value is not None and self.max_value is None:
            result = f">= {self.min_value}"
        else:
            result = f"between {self.min_value} and {self.max_value}"

        return result
