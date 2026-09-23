"""
Watch the encoding solve the Great Cube.

Two things are on screen.  The top half is the SOLVER: every depth the
iterative deepening tries, how big the propositional formula is at that depth,
how long CDCL took, and whether it came back UNSAT (a proof that no plan of
that length exists) or SAT.  The bottom half is the CUBE: the unfolded net,
redrawn after every turn of the plan the solver found.

Usage
-----
    python3 visualizer.py --test tests/05_three_turns.json
    python3 visualizer.py --scramble "R U F' L2 D"
    python3 visualizer.py --random 4
    python3 visualizer.py --random 4 --seed 7 -T 6 --delay 0.6
    python3 visualizer.py --scramble "R U" --no-color

Options
-------
    --test FILE        run a testcase from tests/ (uses its cube and its T)
    --scramble MOVES   scramble the solved cube with these moves
    --random N         scramble with N random moves instead
    --seed S           make --random reproducible
    -T, --deadline T   Tamanna's deadline in minutes (default: scramble length)
    --delay SECONDS    pause between turns while replaying (default 0.7)
    --no-color         plain letters instead of ANSI colour blocks
"""

import argparse
import os
import random
import sys
import time

import check
from rubik_solution import (COLOUR_OF_FACE, FACES, MOVE_NAMES, OFFSET, RING,
                            apply_move, solve)

# ---------------------------------------------------------------------------
#  Drawing
# ---------------------------------------------------------------------------

# background colour, then two spaces: a sticker is a solid block.
ANSI = {"W": "\033[107m", "Y": "\033[103m", "G": "\033[102m",
        "B": "\033[104m", "R": "\033[101m", "O": "\033[48;5;208m"}
RESET = "\033[0m"
DIM = "\033[2m"
BOLD = "\033[1m"

USE_COLOUR = True


def sticker(colour):
    if USE_COLOUR:
        return f"{ANSI[colour]}  {RESET}"
    return f" {colour}"


def net(state):
    """The unfolded cube, in the layout of README.md."""
    def row(face, r):
        return "".join(sticker(state[OFFSET[face] + 3 * r + c]) for c in range(3))

    gap = " " * 7
    lines = [gap + row("U", r) for r in range(3)]
    lines.append("")
    for r in range(3):
        lines.append(" ".join(row(f, r) for f in ("L", "F", "R", "B")))
    lines.append("")
    lines += [gap + row("D", r) for r in range(3)]
    return "\n".join("  " + ln for ln in lines)


def rule(title=""):
    width = 66
    if not title:
        return DIM + "-" * width + RESET if USE_COLOUR else "-" * width
    head = f"-- {title} "
    line = head + "-" * (width - len(head))
    return (DIM + line + RESET) if USE_COLOUR else line


def bold(text):
    return f"{BOLD}{text}{RESET}" if USE_COLOUR else text


# ---------------------------------------------------------------------------
#  The solver half of the screen
# ---------------------------------------------------------------------------

def formula_size(solver, depth):
    """How many Boolean variables and asserted constraints this depth has."""
    colour_vars = 54 * 6 * (depth + 1)
    move_vars = 18 * depth
    constraints = len(solver.assertions()) if solver is not None else 0
    return colour_vars + move_vars, constraints


HEADER = (f"  {'depth':>5}  {'variables':>9}  {'constraints':>11}  "
          f"{'encode':>8}  {'CDCL':>8}   verdict")


def report_depth(depth, solver, satisfiable, build_seconds, solve_seconds):
    if depth == 0:
        verdict = "SAT -- already solved" if satisfiable else "not solved yet"
        print(f"  {depth:>5}  {'-':>9}  {'-':>11}  {'-':>8}  {'-':>8}   {verdict}")
        return
    variables, constraints = formula_size(solver, depth)
    verdict = (bold("SAT -- plan found") if satisfiable
               else "UNSAT -- no plan this short")
    print(f"  {depth:>5}  {variables:>9}  {constraints:>11}  "
          f"{build_seconds:7.2f}s  {solve_seconds:7.2f}s   {verdict}")


# ---------------------------------------------------------------------------

def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Watch the propositional encoding solve the Great Cube.")
    source = ap.add_mutually_exclusive_group()
    source.add_argument("--scramble", help='moves to scramble with, e.g. "R U F\'"')
    source.add_argument("--random", type=int, metavar="N",
                        help="scramble with N random moves")
    source.add_argument("--test", metavar="FILE",
                        help="run a testcase from tests/ (uses its cube and T)")
    ap.add_argument("--seed", type=int, help="seed for --random")
    ap.add_argument("-T", "--deadline", type=int,
                    help="deadline in minutes (default: the scramble length)")
    ap.add_argument("--delay", type=float, default=0.7,
                    help="seconds between turns when replaying (default 0.7)")
    ap.add_argument("--no-color", action="store_true", help="plain letters")
    args = ap.parse_args(argv)

    global USE_COLOUR
    USE_COLOUR = not args.no_color and sys.stdout.isatty()

    # ---- build the scramble ----------------------------------------------
    if args.test:
        name, cube, T, solvable = check.load_test(args.test)
        state = check.parse_cube(cube)
        scramble = None
        deadline = args.deadline if args.deadline is not None else T
        title = f"{name}  ({os.path.basename(args.test)})"
        return run(state, cube, deadline, title, scramble, args)

    if args.random is not None:
        rng = random.Random(args.seed)
        scramble, last_face = [], None
        while len(scramble) < args.random:          # no two turns of one face in a row
            name = rng.choice(MOVE_NAMES)
            if name[0] != last_face:
                scramble.append(name)
                last_face = name[0]
    elif args.scramble:
        scramble = args.scramble.split()
        for name in scramble:
            if name not in check.MOVES:
                ap.error(f"{name!r} is not one of the 18 legal moves")
    else:
        scramble = ["R", "U", "F'"]

    deadline = args.deadline if args.deadline is not None else max(1, len(scramble))

    state = check.SOLVED
    for name in scramble:
        state = apply_move(state, name)
    cube = check.to_cube_input(state)
    return run(state, cube, deadline, None, scramble, args)


def run(state, cube, deadline, title, scramble, args):
    # ---- the scramble ----------------------------------------------------
    print()
    print(rule("THE GREAT CUBE"))
    if title:
        print(f"  testcase       : {title}")
    if scramble is not None:
        print(f"  scrambled with : {' '.join(scramble) or '(nothing)'}")
    print(f"  deadline       : T = {deadline} minute(s), one turn per minute")
    print()
    print(net(state))
    print()

    # ---- the solver ------------------------------------------------------
    print(rule("SPIDEY'S ENCODING"))
    print("  Each depth is a fresh propositional formula: one one-hot colour")
    print("  variable per facelet per time step, one one-hot move variable per")
    print("  step, and a transition clause for every (move, facelet, colour).")
    print("  UNSAT is not a failure -- it is a proof no plan that short exists.")
    print()
    print(HEADER)
    started = time.perf_counter()
    plan = solve(cube, deadline, on_depth=report_depth)
    total = time.perf_counter() - started
    print()
    print(f"  total solver time: {total:.2f}s")
    print()

    if plan == -1:
        print(rule("RESULT"))
        print(f"  UNSAT at every depth up to {deadline}.")
        print("  The Cube cannot be solved inside the deadline. Jean wins.")
        print()
        return 1

    # ---- verify before showing anything ----------------------------------
    ok, message = check.check(cube, deadline, plan)
    print(rule("VERIFIED BY check.py"))
    print(f"  {'PASS' if ok else 'FAIL'}: {message}")
    print()
    if not ok:
        return 1

    # ---- replay ----------------------------------------------------------
    print(rule("TAMANNA EXECUTES, BLIND"))
    print(f"  plan: {bold(' '.join(plan)) if plan else '(nothing to do)'}")
    print()
    print("  minute 0 - start")
    print(net(state))
    for minute, name in enumerate(plan, 1):
        time.sleep(args.delay)
        state = apply_move(state, name)
        print()
        print(f"  minute {minute} - {bold(name)}")
        print(net(state))
    print()

    print(rule("RESULT"))
    if check.is_solved(state):
        print(f"  Six clean faces in {len(plan)} minute(s). The hold breaks.")
    else:
        print("  The cube is not solved. Something is wrong with the encoding.")
        return 1
    print()
    return 0


if __name__ == "__main__":
    sys.exit(main())
