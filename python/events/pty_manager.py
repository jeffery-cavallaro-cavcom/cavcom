"""
This class is used to allocate an available pseudo-terminal (pty) and disable
echo and CRLF-mapping modes on the slave.  This makes the master and slave
suitable for an I/O endpoint.
"""

import os
import pty
import termios

class PTYManager:
    """ PTY Manager """
    master : int
    slave : int

    def __init__(self):
        """ Allocate a PTY """
        self.master, self.slave = pty.openpty()

    def disable_echo_crlf(self) -> None:
        """ Disable echo and CRLF-mapping modes on the slave """
        attrs = termios.tcgetattr(self.slave)
        attrs[1] &= ~termios.ONLCR
        attrs[3] &= ~termios.ECHO
        termios.tcsetattr(self.slave, termios.TCSANOW, attrs)

    def close(self) -> None:
        """ Close the master and the slave """
        if self.master:
            os.close(self.master)
            self.master = None

        if self.slave:
            os.close(self.slave)
            self.slave = None

    def __enter__(self):
        """ Return self """
        return self

    def __exit__(self, *args, **kwargs):
        """ Close the master and slave """
        self.close()
