"""
String type for argument parsers.  Provides whitespace stripping and blank
value suppression checks.
"""

from argparse import ArgumentTypeError
from typing import Optional

class String:
    """ String Type Argument """
    # pylint: disable=too-few-public-methods
    no_strip : bool
    allow_blank : bool

    def __init__(
        self,
        no_strip : Optional[bool] = False,
        allow_blank : Optional[bool] = False
    ):
        """
        Set string options

        Arguments:
            no_strip:
                If True then do not strip leading/trailing whitespace.
            allow_blank:
                Allow a blank ('') value.  This check is applied after any
                necessary whitespace stripping.
        """
        self.no_strip = bool(no_strip)
        self.allow_blank = bool(allow_blank)

    def __call__(self, value : str) -> str:
        """ Strip and suppress blanks as configured """
        if not isinstance(value, str):
            raise ArgumentTypeError('must be a string value')

        if not self.no_strip:
            value = value.strip()

        if not self.allow_blank and not value:
            raise ArgumentTypeError('may not be a blank string')

        return value
