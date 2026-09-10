"""
Visualise what a Sokoban plan is doing, step by step.

Use this to debug your encoding: it imports your solver, gets the plan out of
get_moves(grid), and replays it slowly, printing the board and a short note
after every move.  Unlike checker.py it does not stop at the first mistake --
it flags the illegal move, keeps the player where it was, and carries on, so
you can see the whole trajectory your model produced.

Usage (from inside the Sokoban/ folder):

    python3 visualize.py tests/03.txt
    python3 visualize.py tests/03.txt --solver sokoban_solution
    python3 visualize.py tests/03.txt --delay 0.3        # pause between frames
    python3 visualize.py tests/03.txt --plan LUURDDRRRUULDD   # skip the solver
"""

import argparse
import copy
import sys
import time

from checker import DIRS, Level, LevelError, call_solver, normalize_plan, read_grid

BOX = "\033[33m$\033[0m"
BOX_ON_GOAL = "\033[32m*\033[0m"
PLAYER = "\033[36m@\033[0m"
PLAYER_ON_GOAL = "\033[36m+\033[0m"
GOAL = "\033[90m.\033[0m"


def render(level, player, boxes, use_color=True):
    out = []
    for r in range(level.height):
        row = []
        for c in range(level.width):
            pos = (r, c)
            if pos in level.walls:
                row.append("#")
            elif pos == player:
                ch = "+" if pos in level.goals else "@"
                row.append((PLAYER_ON_GOAL if ch == "+" else PLAYER) if use_color else ch)
            elif pos in boxes:
                ch = "*" if pos in level.goals else "$"
                row.append((BOX_ON_GOAL if ch == "*" else BOX) if use_color else ch)
            elif pos in level.goals:
                row.append(GOAL if use_color else ".")
            else:
                row.append(" ")
        out.append("".join(row))
    return "\n".join(out)


def visualize(level, plan, delay=0.0, use_color=True):
    player = level.player
    boxes = set(level.boxes)
    total = len(plan)
    n_goals = len(level.goals)

    def frame(header):
        print(header)
        print(render(level, player, boxes, use_color))
        on_goal = len(boxes & level.goals)
        print(f"    boxes on goals: {on_goal}/{n_goals}")
        print()
        if delay:
            time.sleep(delay)

    frame(f"step 0/{total}   (start)")

    illegal = 0
    for step, mv in enumerate(plan, start=1):
        dr, dc = DIRS[mv]
        target = (player[0] + dr, player[1] + dc)
        beyond = (target[0] + dr, target[1] + dc)
        note = f"move {mv}"

        if not level.inside(target):
            note = f"move {mv}  --  ILLEGAL: off the grid at {target}"
            illegal += 1
        elif target in level.walls:
            note = f"move {mv}  --  ILLEGAL: into a wall at {target}"
            illegal += 1
        elif target in boxes and not level.inside(beyond):
            note = f"move {mv}  --  ILLEGAL: box pushed off the grid at {beyond}"
            illegal += 1
        elif target in boxes and beyond in level.walls:
            note = f"move {mv}  --  ILLEGAL: box pushed into a wall at {beyond}"
            illegal += 1
        elif target in boxes and beyond in boxes:
            note = f"move {mv}  --  ILLEGAL: box pushed into a box at {beyond}"
            illegal += 1
        elif target in boxes:
            boxes.discard(target)
            boxes.add(beyond)
            player = target
            note = f"move {mv}  --  push {target} -> {beyond}"
        else:
            player = target

        frame(f"step {step}/{total}   {note}")

    solved = boxes == level.goals
    print("=" * 40)
    if solved:
        print("SOLVED: every box is on a goal.")
    else:
        stuck = sorted(boxes - level.goals)
        print(f"NOT SOLVED: {len(stuck)} box(es) off goal at {stuck}")
    if illegal:
        print(f"{illegal} illegal move(s) in the plan.")
    return solved


def main(argv=None):
    ap = argparse.ArgumentParser(description="Visualise a Sokoban plan step by step.")
    ap.add_argument("input", help="path to a level file")
    ap.add_argument("--solver", default="sokoban_template",
                    help="solver module to import (default: sokoban_template)")
    ap.add_argument("--plan", help="visualise this literal plan instead of calling the solver")
    ap.add_argument("--delay", type=float, default=0.0, help="seconds to pause between frames")
    ap.add_argument("--no-color", action="store_true", help="plain ASCII output")
    args = ap.parse_args(argv)

    try:
        grid = read_grid(args.input)
        level = Level(grid)

        print(f"Level {args.input}  ({level.width}x{level.height}, {len(level.boxes)} boxes)")
        print()

        if args.plan is not None:
            plan_raw = args.plan
        else:
            plan_raw, elapsed = call_solver(args.solver, grid)
            print(f"{args.solver}.get_moves returned in {elapsed:.1f}s")
        plan = normalize_plan(plan_raw)
        print(f"plan ({len(plan)} moves): {plan or '(empty)'}\n")

        if not plan:
            print("nothing to visualise -- the plan is empty.")
            return 1

        ok = visualize(level, plan, delay=args.delay, use_color=not args.no_color)
        return 0 if ok else 1

    except LevelError as e:
        print(f"ERROR: {e}")
        return 1
    except ModuleNotFoundError as e:
        print(f"ERROR: could not import solver module ({e})")
        return 1
    except FileNotFoundError as e:
        print(f"ERROR: {e}")
        return 1


if __name__ == "__main__":
    sys.exit(main())
