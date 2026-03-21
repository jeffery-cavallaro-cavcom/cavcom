"""
Finite State Machine Unit Tests
"""

import unittest

from typing import ClassVar, Optional

from events.fsm import FiniteStateMachine, State

class FSMForTest(FiniteStateMachine):
    """ FSM For Test """
    # pylint: disable=too-many-instance-attributes
    E_EVENT_1 : ClassVar[int] = 0
    E_EVENT_2 : ClassVar[int] = 1

    Q_START : ClassVar[int] = 0
    Q_STATE_1 : ClassVar[int] = 1
    Q_STATE_2 : ClassVar[int] = 2
    Q_DONE : ClassVar[int] = 3
    Q_BRANCH : ClassVar[int] = 4

    QE_DATA : ClassVar[int] = 42

    enter_calls : int
    exit_calls : int
    action_calls : int
    received_event : int
    received_data : int

    enter_skip : bool
    exit_skip : bool
    branch : bool

    def __init__(
        self,
        branch : Optional[bool] = False,
        enter_skip : Optional[bool] = False,
        exit_skip : Optional[bool] = False
    ):
        """
        Initialize the FSM

        Arguments:
            branch:
                If True then forces the next state action method to select the
                branch state instead of the next state.
            enter_skip:
                Forces all enter methods (except in the done state) to skip to
                the next state.
            exit_skip:
                Forces all exit methods to skip to the done state.
        """
        qe_matrix : list[State] = [
            # Q_START
            State(
                self.count_enter,
                self.count_exit,
                [self.next_state, None]
            ),

            # Q_STATE_1
            State(
                self.count_enter,
                self.count_exit,
                [self.same_state_implicit, self.next_state]
            ),

            # Q_STATE_2
            State(
                self.count_enter,
                self.count_exit,
                [self.next_state, self.same_state_explicit]
            ),

            # Q_DONE
            State(
                self.count_enter,
                self.count_exit,
                [None, None]
            ),

            # Q_BRANCH
            State(
                None,
                None,
                [self.done, self.done]
            )
        ]

        super().__init__(
            qe_matrix, initial_state=self.Q_START, action_data=self.QE_DATA
        )

        self.enter_calls = 0
        self.exit_calls = 0
        self.action_calls = 0
        self.received_event = None
        self.received_data = None
        self.enter_skip = bool(enter_skip)
        self.exit_skip = bool(exit_skip)
        self.branch = bool(branch)

    def count_enter(self, state : int, _event : int, _data : int) -> int:
        """ Count enter call """
        self.enter_calls += 1
        if not self.enter_skip or self.q_now == self.Q_DONE:
            return None
        return state + 1

    def count_exit(self, _state : int, _event : int, _data : int) -> int:
        """ Count exit call """
        self.exit_calls += 1
        return self.Q_DONE if self.exit_skip else None

    def next_state(
        self, state : int, event : int, data : Optional[int] = None
    ) -> int:
        """ Advance to next state """
        self.action_calls += 1
        self.received_event = event
        self.received_data = data
        return self.Q_BRANCH if self.branch else state + 1

    def same_state_implicit(
        self, _state : int, event : int, data : Optional[int] = None
    ) -> None:
        """ Stay in same state state """
        self.action_calls += 1
        self.received_event = event
        self.received_data = data

    def same_state_explicit(
        self, state : int, event : int, data : Optional[int] = None
    ) -> int:
        """ Stay in same state state """
        self.action_calls += 1
        self.received_event = event
        self.received_data = data
        return state

    def done(
        self, _state : int, event : int, data : Optional[int] = None
    ) -> int:
        """ Move to the done state """
        self.action_calls += 1
        self.received_event = event
        self.received_data = data
        return self.Q_DONE

class TestFSM(unittest.TestCase):
    """ Finite State Machine Unit Tests """
    def test_action(self):
        """ Step from start to done """
        # START
        fsm = FSMForTest()
        fsm.run()
        self.assertEqual(fsm.q_now, FSMForTest.Q_START)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

        # STATE 1
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_1)
        self.assertEqual(fsm.enter_calls, 2)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 1)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # STATE 2
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_2)
        self.assertEqual(fsm.enter_calls, 3)
        self.assertEqual(fsm.exit_calls, 2)
        self.assertEqual(fsm.action_calls, 2)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # DONE
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_DONE)
        self.assertEqual(fsm.enter_calls, 4)
        self.assertEqual(fsm.exit_calls, 3)
        self.assertEqual(fsm.action_calls, 3)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

    def test_same(self):
        """ Stay in the same state """
        # pylint: disable=too-many-statements
        # START
        fsm = FSMForTest()
        fsm.run()
        self.assertEqual(fsm.q_now, FSMForTest.Q_START)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

        # NO ACTION
        fsm.run(FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.q_now, FSMForTest.Q_START)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

        # STATE 1
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_1)
        self.assertEqual(fsm.enter_calls, 2)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 1)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # SAME (action returns None)
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_1)
        self.assertEqual(fsm.enter_calls, 2)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 2)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # STATE 2
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_2)
        self.assertEqual(fsm.enter_calls, 3)
        self.assertEqual(fsm.exit_calls, 2)
        self.assertEqual(fsm.action_calls, 3)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # SAME (action returns same state)
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.q_now, FSMForTest.Q_STATE_2)
        self.assertEqual(fsm.enter_calls, 3)
        self.assertEqual(fsm.exit_calls, 2)
        self.assertEqual(fsm.action_calls, 4)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # DONE
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_DONE)
        self.assertEqual(fsm.enter_calls, 4)
        self.assertEqual(fsm.exit_calls, 3)
        self.assertEqual(fsm.action_calls, 5)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

    def test_no_enter_exit(self):
        """ no enter or exit method for state """
        # START
        fsm = FSMForTest(branch=True)
        fsm.run()
        self.assertEqual(fsm.q_now, FSMForTest.Q_START)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

        # BRANCH
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_BRANCH)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 1)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

        # DONE
        fsm.received_event = None
        fsm.received_data = None
        fsm.run(FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.q_now, FSMForTest.Q_DONE)
        self.assertEqual(fsm.enter_calls, 2)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 2)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_2)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

    def test_enter_skip(self):
        """ Skip from start to done """
        fsm = FSMForTest(enter_skip=True)
        fsm.run()
        self.assertEqual(fsm.q_now, FSMForTest.Q_DONE)
        self.assertEqual(fsm.enter_calls, 4)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

    def test_exit_skip(self):
        """ Exit force to done state """
        # START
        fsm = FSMForTest(exit_skip=True)
        fsm.run()
        self.assertEqual(fsm.q_now, FSMForTest.Q_START)
        self.assertEqual(fsm.enter_calls, 1)
        self.assertEqual(fsm.exit_calls, 0)
        self.assertEqual(fsm.action_calls, 0)
        self.assertIsNone(fsm.received_event)
        self.assertIsNone(fsm.received_data)

        # DONE
        fsm.run(FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.q_now, FSMForTest.Q_DONE)
        self.assertEqual(fsm.enter_calls, 2)
        self.assertEqual(fsm.exit_calls, 1)
        self.assertEqual(fsm.action_calls, 1)
        self.assertEqual(fsm.received_event, FSMForTest.E_EVENT_1)
        self.assertEqual(fsm.received_data, FSMForTest.QE_DATA)

if __name__ == '__main__':
    unittest.main()
