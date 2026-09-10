from z3 import *


def get_moves(grid):
    """
    Solve a Sokoban level with a SAT encoding.

    Play the puzzle: https://www.mathsisfun.com/games/sokoban.html

    `grid` is a 2-D list of single characters:
        '#' wall   ' ' floor   '.' goal   '$' box   '*' box on goal
        '@' player   '+' player on goal

    Each step the player moves one cell U/D/L/R onto floor or a goal; if the
    target cell holds a box the box is pushed one further cell in the same
    direction (only onto floor or a goal, never a wall or another box).  One
    box at a time, no pulling.  #goals == #boxes.

    Levels are NOT walled in.  A cell off the edge of the grid counts as
    blocked, exactly like a wall: the player may not step off the grid and a
    box may not be pushed off it.

    Return the plan as a string over {U, D, L, R} (e.g. "LUURDDRRRUULDD") that
    ends with every box on a goal, or "" if you find none.  checker.py replays
    what you return and reports VALID / INVALID.

    Approach: introduce booleans for the player position, the box positions and
    the action at each step, assert the rules as constraints, solve, and read
    the moves out of the model.  Working out the constraints is the exercise;
    the rules above are the whole specification.

    Use only propositional logic: Bool, Not, And, Or, Implies, ==, and a plain
    Solver().  Spell "exactly one" out as "at least one" AND "no two at once".
    Do NOT use AtMost, PbEq, Sum, If or Optimize.

    See sokoban.md for the Z3 syntax (m.eval, is_true, ...) and for how to run
    checker.py and visualize.py.
    """
    # Every level you are given is solvable in AT MOST 15 STEPS, so the horizon
    # is fixed: build the encoding once for T = 15 and do a single s.check().
    # There is no need to loop over T, and you may assume no level needs more.
    #
    # Because a level may need fewer than 15 moves, give yourself a fifth
    # "wait" action alongside U/D/L/R that changes nothing, and force every
    # wait to sit at the TAIL of the plan (wait at step t implies wait at step
    # t+1).  A k-move solution then shows up as k real moves followed by 15 - k
    # waits, and you simply skip the waits when decoding the model.
    T = 15

    # TODO: build the encoding, solve it, and return the move string.
    return ""
