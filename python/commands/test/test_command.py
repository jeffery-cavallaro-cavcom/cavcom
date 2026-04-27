""" Synchronous Command Unit Tests """

import unittest

from os import environ
from socket import gethostname
from threading import Thread
from typing import ClassVar

from commands.command import Command

# pylint: disable=duplicate-code

class TestCommand(unittest.TestCase):
    """ Synchronous Command Unit Tests """
    HOST : ClassVar[str] = gethostname().split('.')[0]
    USER : ClassVar[str] = environ.get('USER')
    TIMEOUT : ClassVar[float] = 5.0

    def test_one_local(self):
        """ Execute a single command locally """
        with Command(
            ['/usr/bin/hostname', '-s'],
            command_timeout=self.TIMEOUT
        ) as command:
            command.execute()

        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(
            command.stdout.fetch_input(text=True, reset=True).strip(),
            self.HOST
        )
        self.assertIsNone(command.stdout.fetch_input())

    def test_many_local(self):
        """ Execute many commands locally """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                command_timeout=self.TIMEOUT
            )
            for _ in range(100)
        ]

        threads = [Thread(target=command.execute) for command in commands]
        for thread in threads:
            thread.start()

        for thread in threads:
            thread.join()

        for command in commands:
            command.close()
            self.assertEqual(command.status, 0)
            self.assertIsNone(command.reason)
            self.assertEqual(
                command.stdout.fetch_input(text=True).strip(), self.HOST
            )

    def test_timeout(self):
        """ Execute a command that times out """
        with Command(
            ['sleep', '10'],
            command_timeout=1.0,
        ) as command:
            command.execute()

        self.assertEqual(command.status, -15)
        self.assertIn('timed out', command.reason)

    def test_one_remote(self):
        """ Execute a single command remotely (ssh key) """
        with Command(
            ['/usr/bin/hostname', '-s'],
            remote_host=self.HOST,
            command_timeout=self.TIMEOUT
        ) as command:
            command.execute()

        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(
            command.stdout.fetch_input(text=True).strip(), self.HOST
        )

    def test_many_remote(self):
        """ Execute many commands remotely (ssh key) """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                remote_host=self.HOST,
                command_timeout=self.TIMEOUT
            )
            for _ in range(10)  # Limited by sshd_config.
        ]

        threads = [Thread(target=command.execute) for command in commands]
        for thread in threads:
            thread.start()

        for thread in threads:
            thread.join()

        for command in commands:
            command.close()
            self.assertEqual(command.status, 0)
            self.assertIsNone(command.reason)
            self.assertEqual(
                command.stdout.fetch_input(text=True).strip(), self.HOST
            )

if __name__ == '__main__':
    unittest.main()
