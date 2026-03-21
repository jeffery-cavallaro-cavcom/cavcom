"""
PTY Manager Unit Tests
"""

import unittest

import os

from events.pty_manager import PTYManager

class TestPTYManager(unittest.TestCase):
    """ PTY Manager Unit Tests """
    def test_read_write(self):
        """ Read and write with echo and CRLF-mapping """
        m2s_message = b'MASTER_TO_SLAVE'
        s2m_message = b'SLAVE_TO_MASTER'

        with PTYManager() as pty:
            self.assertIsNotNone(pty.master)
            self.assertIsNotNone(pty.slave)

            # Master to slave:
            message = m2s_message + b"\n"
            echo = m2s_message + b"\r\n"

            self.assertEqual(os.write(pty.master, message), len(message))
            data = os.read(pty.master, 32)  # echo
            self.assertEqual(data, echo)

            data = os.read(pty.slave, 32)
            self.assertEqual(data, message)

            # Slave to master:
            message = s2m_message + b"\n"
            result = s2m_message + b"\r\n"
            self.assertEqual(os.write(pty.slave, message), len(message))
            data = os.read(pty.master, 32)
            self.assertEqual(data, result)

        self.assertIsNone(pty.master)
        self.assertIsNone(pty.slave)

    def test_read_write_no_echo_crlf(self):
        """ Read and write with no echo and no CRLF-mapping """
        m2s_message = b'MASTER_TO_SLAVE\n'
        s2m_message = b'SLAVE_TO_MASTER\n'

        with PTYManager() as pty:
            self.assertIsNotNone(pty.master)
            self.assertIsNotNone(pty.slave)
            pty.disable_echo_crlf()

            # Master to slave:
            self.assertEqual(
                os.write(pty.master, m2s_message), len(m2s_message)
            )
            data = os.read(pty.slave, 32)
            self.assertEqual(data, m2s_message)

            # Slave to master:
            self.assertEqual(
                os.write(pty.slave, s2m_message), len(s2m_message)
            )
            data = os.read(pty.master, 32)
            self.assertEqual(data, s2m_message)

        self.assertIsNone(pty.master)
        self.assertIsNone(pty.slave)

if __name__ == '__main__':
    unittest.main()
