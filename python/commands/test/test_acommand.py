""" Asynchronous Command Unit Tests """

import unittest

import asyncio
from os import environ
from socket import gethostname
from typing import ClassVar

from commands.acommand import Command

class TestACommand(unittest.IsolatedAsyncioTestCase):
    """ Asynchronous Command Unit Tests """
    HOST : ClassVar[str] = gethostname().split('.')[0]
    USER : ClassVar[str] = environ.get('USER')
    TIMEOUT : ClassVar[float] = 5.0

    async def test_one_local(self):
        """ Execute a single command locally """
        with Command(
            ['/usr/bin/hostname', '-s'],
            command_timeout=self.TIMEOUT
        ) as acommand:
            await acommand.execute()

        self.assertEqual(acommand.status, 0)
        self.assertIsNone(acommand.reason)
        self.assertEqual(acommand.stdout.data.decode().strip(), self.HOST)

    async def test_two_local(self):
        """ Execute two different commands locally """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                command_timeout=self.TIMEOUT
            ),
            Command(
                ['whoami'],
                command_timeout=self.TIMEOUT
            )
        ]

        try:
            tasks = [
                asyncio.create_task(command.execute()) for command in commands
            ]
            await asyncio.gather(*tasks)
        finally:
            for command in commands:
                command.close()

        command = commands[0]
        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(command.stdout.data.decode().strip(), self.HOST)

        command = commands[1]
        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(command.stdout.data.decode().strip(), self.USER)

    async def test_many_local(self):
        """ Execute many commands locally """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                command_timeout=self.TIMEOUT
            )
            for _ in range(100)
        ]

        try:
            tasks = [
                asyncio.create_task(command.execute()) for command in commands
            ]
            await asyncio.gather(*tasks)
        finally:
            for command in commands:
                command.close()

        for command in commands:
            self.assertEqual(command.status, 0)
            self.assertIsNone(command.reason)
            self.assertEqual(command.stdout.data.decode().strip(), self.HOST)

    async def test_one_remote(self):
        """ Execute a single command remotely (ssh key) """
        with Command(
            ['/usr/bin/hostname', '-s'],
            remote_host=self.HOST,
            command_timeout=self.TIMEOUT
        ) as acommand:
            await acommand.execute()

        self.assertEqual(acommand.status, 0)
        self.assertIsNone(acommand.reason)
        self.assertEqual(acommand.stdout.data.decode().strip(), self.HOST)

    async def test_two_remote(self):
        """ Execute two different commands remotely (ssh key) """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                remote_host=self.HOST,
                command_timeout=self.TIMEOUT
            ),
            Command(
                ['whoami'],
                remote_host=self.HOST,
                command_timeout=self.TIMEOUT
            )
        ]

        try:
            tasks = [
                asyncio.create_task(command.execute()) for command in commands
            ]
            await asyncio.gather(*tasks)
        finally:
            for command in commands:
                command.close()

        command = commands[0]
        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(command.stdout.data.decode().strip(), self.HOST)

        command = commands[1]
        self.assertEqual(command.status, 0)
        self.assertIsNone(command.reason)
        self.assertEqual(command.stdout.data.decode().strip(), self.USER)

    async def test_many_remote(self):
        """ Execute many commands remotely (ssh key) """
        commands = [
            Command(
                ['/usr/bin/hostname', '-s'],
                remote_host=self.HOST,
                command_timeout=self.TIMEOUT
            )
            for _ in range(10)  # Limited by sshd_config.
        ]

        try:
            tasks = [
                asyncio.create_task(command.execute()) for command in commands
            ]
            await asyncio.gather(*tasks)
        finally:
            for command in commands:
                command.close()

        for command in commands:
            self.assertEqual(command.status, 0)
            self.assertIsNone(command.reason)
            self.assertEqual(command.stdout.data.decode().strip(), self.HOST)

if __name__ == '__main__':
    unittest.main()
