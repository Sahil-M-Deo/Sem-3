"""
Independent checker for Sokoban plans.

It does NOT use Z3.  It imports your solver, hands it the level as a 2-D list,
replays the plan it returns against the rules of Sokoban, and reports whether
every box ends on a goal.

Usage (from inside the Sokoban/ folder):

    python3 checker.py tests/01.txt                     # uses sokoban_template.get_moves
    python3 checker.py tests/01.txt --solver sokoban_solution
    python3 checker.py tests/01.txt -q                  # don't print every board

Your solver module must define:

    get_moves(grid) -> str | list      # grid is a 2-D list of characters
                                       # return a plan over {U, D, L, R}

The checker gives the solver at most 5 minutes (300 s) to return.

Exit code 0 = valid solution, 1 = invalid / error / timeout.
"""

import argparse
import copy
import importlib
import signal
import sys
import time

DIRS = {
    "U": (-1, 0),
    "D": (1, 0),
    "L": (0, -1),
    "R": (0, 1),
}

TIME_LIMIT = 300          # seconds the solver is allowed to run


class LevelError(Exception):
    pass


class Timeout(Exception):
    pass


# --------------------------------------------------------------------------
# Level: parsed from a 2-D list of characters
#   #  wall        (space) floor       .  goal
#   $  box         *  box on goal
#   @  player      +  player on goal
# --------------------------------------------------------------------------
class Level:
    def __init__(self, grid):
        rows = [list(row) for row in grid]
        if not rows:
            raise LevelError("empty level")
        width = max(len(r) for r in rows)
        self.walls = set()
        self.goals = set()
        self.boxes = set()
        self.player = None
        for r, line in enumerate(rows):
            for c in range(width):
                ch = line[c] if c < len(line) else " "
                pos = (r, c)
                if ch == "#":
                    self.walls.add(pos)
                elif ch in (" ", "-", "_"):
                    pass
                elif ch == ".":
                    self.goals.add(pos)
                elif ch == "$":
                    self.boxes.add(pos)
                elif ch == "*":
                    self.boxes.add(pos)
                    self.goals.add(pos)
                elif ch == "@":
                    self.player = pos
                elif ch == "+":
                    self.player = pos
                    self.goals.add(pos)
                else:
                    raise LevelError(f"unknown character {ch!r} at row {r}, col {c}")
        self.height = len(rows)
        self.width = width
        self._validate()

    def _validate(self):
        if self.player is None:
            raise LevelError("no player (@ or +) in level")
        if not self.boxes:
            raise LevelError("no boxes ($) in level")
        if len(self.boxes) != len(self.goals):
            raise LevelError(
                f"{len(self.boxes)} boxes but {len(self.goals)} goals -- must be equal"
            )
        if self.player in self.walls:
            raise LevelError("player starts inside a wall")
        if self.boxes & self.walls:
            raise LevelError("a box starts inside a wall")

    def inside(self, pos):
        """Levels are not walled in: the edge of the grid is impassable too."""
        r, c = pos
        return 0 <= r < self.height and 0 <= c < self.width

    def render(self, player, boxes):
        out = []
        for r in range(self.height):
            row = []
            for c in range(self.width):
                pos = (r, c)
                if pos in self.walls:
                    row.append("#")
                elif pos == player:
                    row.append("+" if pos in self.goals else "@")
                elif pos in boxes:
                    row.append("*" if pos in self.goals else "$")
                elif pos in self.goals:
                    row.append(".")
                else:
                    row.append(" ")
            out.append("".join(row))
        return "\n".join(out)


# --------------------------------------------------------------------------
# I/O helpers
# --------------------------------------------------------------------------
def read_grid(path):
    """Read a level file into a 2-D list of characters, padded to a rectangle."""
    with open(path) as fh:
        lines = [line.rstrip("\n") for line in fh]
    # Only genuinely empty trailing lines are noise.  A whitespace-only line is
    # real floor: these levels have no border wall, so a blank row matters.
    while lines and lines[-1] == "":
        lines.pop()
    if not lines:
        raise LevelError(f"{path}: empty file")
    width = max(len(line) for line in lines)
    return [list(line.ljust(width)) for line in lines]


def normalize_plan(plan):
    if plan is None:
        raise LevelError("get_moves returned None")
    if isinstance(plan, (list, tuple)):
        plan = "".join(str(x) for x in plan)
    plan = str(plan).strip().upper()
    for junk in (" ", ",", "-", "\n", "\t"):
        plan = plan.replace(junk, "")
    bad = sorted(set(plan) - set(DIRS))
    if bad:
        raise LevelError(f"plan contains illegal move(s): {bad}")
    return plan


# --------------------------------------------------------------------------
# Replay
# --------------------------------------------------------------------------
def replay(level, plan, verbose=True):
    player = level.player
    boxes = set(level.boxes)

    if verbose:
        print("Start:")
        print(level.render(player, boxes))
        print()

    for step, mv in enumerate(plan, start=1):
        dr, dc = DIRS[mv]
        target = (player[0] + dr, player[1] + dc)
        beyond = (target[0] + dr, target[1] + dc)

        if not level.inside(target):
            raise LevelError(f"move {step} ({mv}): player walks off the grid at {target}")
        if target in level.walls:
            raise LevelError(f"move {step} ({mv}): player walks into a wall at {target}")

        if target in boxes:
            if not level.inside(beyond):
                raise LevelError(f"move {step} ({mv}): box pushed off the grid at {beyond}")
            if beyond in level.walls:
                raise LevelError(f"move {step} ({mv}): box pushed into a wall at {beyond}")
            if beyond in boxes:
                raise LevelError(f"move {step} ({mv}): box pushed into another box at {beyond}")
            boxes.discard(target)
            boxes.add(beyond)
            player = target
        else:
            player = target

        if verbose:
            print(f"move {step}: {mv}")
            print(level.render(player, boxes))
            print()

    solved = boxes == level.goals
    return solved, player, boxes


# --------------------------------------------------------------------------
# Calling the solver, with a hard time limit
# --------------------------------------------------------------------------
def _alarm(signum, frame):
    raise Timeout()


def call_solver(module_name, grid):
    sys.path.insert(0, ".")
    mod = importlib.import_module(module_name)
    if not hasattr(mod, "get_moves"):
        raise LevelError(f"module {module_name!r} does not define get_moves(grid)")

    handler_ok = hasattr(signal, "SIGALRM")
    if handler_ok:
        signal.signal(signal.SIGALRM, _alarm)
        signal.alarm(TIME_LIMIT)
    t0 = time.time()
    try:
        plan = mod.get_moves(copy.deepcopy(grid))
    finally:
        if handler_ok:
            signal.alarm(0)
    return plan, time.time() - t0


# --------------------------------------------------------------------------
def main(argv=None):
    ap = argparse.ArgumentParser(description="Independent Sokoban plan checker.")
    ap.add_argument("input", help="path to a level file")
    ap.add_argument("--solver", default="sokoban_template",
                    help="solver module to import (default: sokoban_template)")
    ap.add_argument("-q", "--quiet", action="store_true", help="don't print each step")
    args = ap.parse_args(argv)

    try:
        grid = read_grid(args.input)
        level = Level(grid)

        print(f"Level {args.input}  ({level.width}x{level.height}, "
              f"{len(level.boxes)} boxes)   solver: {args.solver}")
        print(level.render(level.player, level.boxes))
        print()

        plan_raw, elapsed = call_solver(args.solver, grid)
        plan = normalize_plan(plan_raw)
        print(f"Solver returned {len(plan)} moves in {elapsed:.1f}s: {plan or '(empty)'}\n")

        if not plan:
            print("INVALID: solver returned no moves.")
            return 1

        solved, player, boxes = replay(level, plan, verbose=not args.quiet)

    except Timeout:
        print(f"INVALID: solver exceeded the {TIME_LIMIT}s time limit.")
        return 1
    except LevelError as e:
        print(f"INVALID: {e}")
        return 1
    except ModuleNotFoundError as e:
        print(f"INVALID: could not import solver module ({e})")
        return 1
    except FileNotFoundError as e:
        print(f"INVALID: {e}")
        return 1

    on_goal = len(boxes & level.goals)
    print(f"Boxes on goals: {on_goal}/{len(level.goals)}")
    if solved:
        print("VALID: all boxes are on goals.")
        return 0
    print("INVALID: not every box is on a goal.")
    return 1


if __name__ == "__main__":
    sys.exit(main())
