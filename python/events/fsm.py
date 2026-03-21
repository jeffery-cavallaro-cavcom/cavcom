"""
A finite state machine (FSM) is a computational model consisting of a finite
set of states, transitions between them, and actions based on input events. A
FSM can exist in only one state at a time, changing state in response to input
events.  This class acts as a base class for a FSM.

Each state defines an enter method, called when the state is entered, an exit
method, called when the state is exited, and a list of action methods for the
defined events.  State and event IDs are integers that are used to index the
supplied state/event matrix.  An external event loop determines the next state
and calls the FSM, which selects and calls an action method based on the event
and current state.  The return value of the action method determines the next
state.

Note that both the enter and exit methods can override the next state selection
made by a called action method.  This results in the following special cases:

    1.  If an exit method explicitly causes the FSM to remain in the current
        state then the current state was never really left and the state's
        enter method is not called.

    2.  If an enter method explicitly cases the FSM to change to a new state
        then the new state was never really entered and the state's exit method
        is not called.

Once an FSM is created, its run() method should be called with no event ID (or
an event ID < 0).  This forces the FSM into the initial state, calling the
initial state enter method, if any.  The event loop should then continue to call
the FSM until the done state is achieved.
"""

from dataclasses import dataclass
from typing import Any, Callable, Optional

@dataclass
class State:
    """ A single state in a FSM """
    enter : Callable
    exit : Callable
    actions : list[Callable]

class FiniteStateMachine:
    """ Finite State Machine Base Class """
    # pylint: disable=too-few-public-methods

    # Begin, end, and action methods accept the current state, the next event,
    # and an opaque data value.  Methods should return the next state value, or
    # None to not change state.  A None value for a method means that no action
    # should be taken and do not change state.
    Action = Callable[[int, int, Any], int]  # action(state, event, data)

    states : list[State]
    q_now : int
    action_data : Any

    def __init__(
        self,
        states : list[State],
        initial_state : Optional[int] = 0,
        action_data : Optional[Any] = None
    ):
        """
        Set the state matrix and initial state

        Arguments:
            states:
                State/event matrix.
            initial_state:
                Initial state.
            action_data:
                Opaque data passed to called action methods.
        """
        self.states = states
        self.q_now = initial_state or 0
        self.action_data = action_data

    def run(self, next_event : Optional[int] = -1) -> None:
        """
        Run the FSM for the next event

        Arguments:
            next_event:
                Next event.  A special value of < 0 is used by the init method
                to enter the initial state.
        """
        start = next_event < 0

        if start:
            action = None
        else:
            action = self.states[self.q_now].actions[next_event]

        if action:
            action_next = action(self.q_now, next_event, self.action_data)
            q_next = self.q_now if action_next is None else action_next
        else:
            q_next = self.q_now

        if (q_next == self.q_now) and (not start):
            return

        state = self.states[self.q_now]

        if state.exit and (not start):
            q_exit = state.exit(
                self.q_now, next_event, self.action_data
            )
            if q_exit is not None:
                q_next = q_exit
            if q_next == self.q_now:
                return

        while (q_next != self.q_now) or start:
            self.q_now = q_next
            state = self.states[self.q_now]
            start = False

            if state.enter:
                q_enter = state.enter(
                    self.q_now, next_event, self.action_data
                )
                if q_enter is not None:
                    q_next = q_enter
