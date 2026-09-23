"""
Independent checker for the Great Cube.

This file does NOT import z3 and does NOT import anything else from this
folder.  It knows the rules of the cube and nothing about how you solved it:
you hand it the scramble and the sequence of moves your ``solve`` returned, and
it replays the sequence one turn at a time and reports whether the cube ends up
solved.

What it verifies
----------------
  1. The scramble is a well-formed cube description: 6 single-key dicts, known
     centre colours, 8 stickers each, 9 stickers of every colour.
  2. The answer is a list of legal move names -- one of the 18 -- or -1.
  3. The answer uses at most T moves.
  4. Replaying it really does solve the cube.

Usage
-----
    # as a library, which is how the visualiser and your own tests use it
    from check import check
    ok, message = check(cube, T, plan)

    # from the command line: run YOUR solve() on the testcases in tests/
    python3 check.py q4.py                       # every tests/*.json
    python3 check.py q4.py tests/05_three_turns.json
    python3 check.py q4.py --list                # just list the testcases

A testcase is a JSON file:

    {
      "name": "one turn",
      "scramble": "R",          # documentation only; never used to solve
      "T": 3,
      "solvable": true,         # false means solve must return -1
      "cube": [ {"W": [...8...]}, ... six faces ... ]
    }

Write your own by hand, or generate one with make_test.py.
"""

import argparse
import copy
import glob
import json
import os
import sys
import time

FACES = ["U", "R", "F", "D", "L", "B"]
OFFSET = {f: 9 * i for i, f in enumerate(FACES)}
COLOUR_OF_FACE = {"U": "W", "R": "R", "F": "G", "D": "Y", "L": "O", "B": "B"}
FACE_OF_COLOUR = {c: f for f, c in COLOUR_OF_FACE.items()}
COLOURS = set(COLOUR_OF_FACE.values())
RING = [0, 1, 2, 3, 5, 6, 7, 8]          # the 8 slots around a centre

# --- geometry, derived from the net in README.md (see below) ---------------
NORMAL = {"U": (0, 1, 0), "D": (0, -1, 0), "F": (0, 0, 1),
          "B": (0, 0, -1), "L": (-1, 0, 0), "R": (1, 0, 0)}
# (one step DOWN a row, one step RIGHT along a column), from the adjacency
# table: U's top row touches B, so down a row on U points toward F, etc.
NET = {"U": ("F", "R"), "D": ("B", "R"), "F": ("D", "R"),
       "B": ("D", "L"), "L": ("D", "F"), "R": ("D", "B")}


def _cubie(face, index):
    row, col = divmod(index, 3)
    n, down, right = NORMAL[face], NORMAL[NET[face][0]], NORMAL[NET[face][1]]
    return tuple(n[k] + (row - 1) * down[k] + (col - 1) * right[k] for k in range(3))


def _rotate(axis, v):
    x, y, z = v
    return (x, z, -y) if axis == 0 else (-z, y, x) if axis == 1 else (y, -x, z)


_STICKER = {OFFSET[f] + i: (_cubie(f, i), NORMAL[f]) for f in FACES for i in range(9)}
_LOOKUP = {v: k for k, v in _STICKER.items()}
assert len(_LOOKUP) == 54


def _build_moves():
    moves = {}
    for face in FACES:
        normal = NORMAL[face]
        axis = next(i for i in range(3) if normal[i] != 0)
        sign = normal[axis]
        turns = 1 if sign > 0 else 3          # clockwise seen from OUTSIDE
        quarter = list(range(54))
        for facelet, (coord, norm) in _STICKER.items():
            if coord[axis] != sign:
                continue
            for _ in range(turns):
                coord, norm = _rotate(axis, coord), _rotate(axis, norm)
            quarter[facelet] = _LOOKUP[(coord, norm)]

        assert sum(1 for i in range(54) if quarter[i] != i) == 20
        compose = lambda p, q: [q[p[i]] for i in range(54)]
        four = quarter
        for _ in range(3):
            four = compose(four, quarter)
        assert four == list(range(54)), f"{face}^4 is not the identity"

        half = compose(quarter, quarter)
        moves[face] = quarter
        moves[face + "2"] = half
        moves[face + "'"] = compose(half, quarter)
    return moves


MOVES = _build_moves()
LEGAL = sorted(MOVES)


class Bad(Exception):
    """The scramble or the answer is unacceptable, with a readable reason."""


# ---------------------------------------------------------------------------

def parse_cube(cube):
    """The list of 6 single-key dicts -> a flat 54-list of colours."""
    if not isinstance(cube, (list, tuple)) or len(cube) != 6:
        raise Bad(f"expected a list of 6 faces, got {cube!r}")
    state = [None] * 54
    seen = set()
    for entry in cube:
        if not isinstance(entry, dict) or len(entry) != 1:
            raise Bad(f"each face must be a single-key dict, got {entry!r}")
        (centre, ring), = entry.items()
        if centre not in FACE_OF_COLOUR:
            raise Bad(f"unknown centre colour {centre!r}")
        if centre in seen:
            raise Bad(f"two faces both have centre colour {centre!r}")
        seen.add(centre)
        if len(ring) != 8:
            raise Bad(f"face {centre!r} lists {len(ring)} stickers, expected 8")
        base = OFFSET[FACE_OF_COLOUR[centre]]
        state[base + 4] = centre
        for slot, colour in zip(RING, ring):
            if colour not in COLOURS:
                raise Bad(f"unknown colour {colour!r} on face {centre!r}")
            state[base + slot] = colour
    counts = {c: state.count(c) for c in sorted(COLOURS)}
    if any(n != 9 for n in counts.values()):
        raise Bad(f"colour counts are not 9 each: {counts}")
    return state


def apply_move(state, name):
    """``state`` after one turn.  Raises Bad on an illegal move name."""
    if name not in MOVES:
        raise Bad(f"{name!r} is not one of the 18 legal moves {LEGAL}")
    perm = MOVES[name]
    out = [None] * 54
    for src in range(54):
        out[perm[src]] = state[src]
    return out


def is_solved(state):
    return all(state[OFFSET[f] + i] == COLOUR_OF_FACE[f]
               for f in FACES for i in range(9))


def render(state):
    """The unfolded net, for reporting."""
    def row(face, r):
        return " ".join(state[OFFSET[face] + 3 * r + c] for c in range(3))
    pad = " " * 6
    lines = [pad + row("U", r) for r in range(3)]
    for r in range(3):
        lines.append(" ".join(row(f, r) for f in ("L", "F", "R", "B")))
    lines += [pad + row("D", r) for r in range(3)]
    return "\n".join("    " + ln for ln in lines)


def check(cube, T, plan):
    """Is ``plan`` a correct answer for ``cube`` with deadline ``T``?

    Returns (ok, message).  ``plan`` is the sequence of moves returned by
    solve, or -1 meaning "no solution within T".  Note that -1 is only
    accepted as *unverified*: proving no plan exists is the solver's job, and
    this checker deliberately does no search of its own.
    """
    try:
        state = parse_cube(cube)
    except Bad as exc:
        return False, f"the scramble itself is malformed: {exc}"

    if plan == -1:
        if is_solved(state):
            return False, "returned -1, but the cube was already solved (expected [])"
        return True, "returned -1 (claims unsolvable within T -- not verified here)"

    if not isinstance(plan, (list, tuple)):
        return False, f"expected a list of moves or -1, got {type(plan).__name__}"
    if len(plan) > T:
        return False, f"used {len(plan)} moves but the deadline is T = {T}"

    try:
        for step, name in enumerate(plan, 1):
            if not isinstance(name, str):
                raise Bad(f"move {step} is {name!r}, not a string")
            state = apply_move(state, name)
    except Bad as exc:
        return False, str(exc)

    if not is_solved(state):
        return False, ("the sequence runs, but the cube is not solved at the end:\n"
                       + render(state))
    return True, f"solved in {len(plan)} move(s): {' '.join(plan) or '(none)'}"


# ---------------------------------------------------------------------------
#  Command line: run someone's solve() on the built-in scrambles
# ---------------------------------------------------------------------------

SOLVED = [COLOUR_OF_FACE[f] for f in FACES for _ in range(9)]


def to_cube_input(state):
    """A flat 54-list -> the list-of-dicts format ``solve`` receives."""
    return [{COLOUR_OF_FACE[f]: [state[OFFSET[f] + i] for i in RING]}
            for f in FACES]


def scrambled(moves):
    state = SOLVED
    for name in moves:
        state = apply_move(state, name)
    return to_cube_input(state)


TESTS_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tests")


def load_test(path):
    """Read a testcase file.  Returns (name, cube, T, solvable)."""
    with open(path) as fh:
        data = json.load(fh)
    for key in ("T", "cube"):
        if key not in data:
            raise Bad(f"{path}: testcase has no {key!r} field")
    name = data.get("name") or os.path.splitext(os.path.basename(path))[0]
    return name, data["cube"], int(data["T"]), bool(data.get("solvable", True))


def all_tests():
    return sorted(glob.glob(os.path.join(TESTS_DIR, "*.json")))


def main(argv=None):
    ap = argparse.ArgumentParser(
        description="Replay a plan and check it solves the cube (no z3 used).")
    ap.add_argument("program", nargs="?", help="your solution file, e.g. q4.py")
    ap.add_argument("cases", nargs="*",
                    help="testcase files (default: every tests/*.json)")
    ap.add_argument("--list", action="store_true",
                    help="list the available testcases and exit")
    args = ap.parse_args(argv)

    cases = args.cases or all_tests()
    if not cases:
        ap.error(f"no testcases found in {TESTS_DIR}")

    if args.list:
        for path in cases:
            name, _, T, solvable = load_test(path)
            expect = "a plan" if solvable else "-1"
            print(f"  {os.path.basename(path):<26} T = {T:<3} name: {name}"
                  f"   expects {expect}")
        return 0

    if not args.program:
        ap.error("give the solution file to check, or use --list")
    if not os.path.exists(args.program):
        ap.error(f"no such file: {args.program}")

    import importlib.util
    spec = importlib.util.spec_from_file_location(
        "student", os.path.abspath(args.program))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    if not hasattr(module, "solve"):
        print(f"FAIL: {args.program} defines no function called solve")
        return 1

    passed = 0
    for path in cases:
        try:
            name, cube, T, solvable = load_test(path)
        except (Bad, ValueError, OSError) as exc:
            print(f"[{os.path.basename(path)}] FAIL: {exc}")
            continue

        started = time.perf_counter()
        try:
            plan = module.solve(copy.deepcopy(cube), T)
        except Exception as exc:                       # noqa: BLE001
            print(f"[{name}] FAIL: solve raised {type(exc).__name__}: {exc}")
            continue
        elapsed = time.perf_counter() - started

        if not solvable:
            if plan == -1:
                print(f"[{name}] PASS: returned -1 as expected  ({elapsed:.2f}s)")
                passed += 1
            else:
                print(f"[{name}] FAIL: expected -1 (no plan within T = {T}), "
                      f"got {plan!r}")
            continue
        if plan == -1:
            print(f"[{name}] FAIL: returned -1, but this cube is solvable "
                  f"within T = {T}")
            continue

        ok, message = check(cube, T, plan)
        print(f"[{name}] {'PASS' if ok else 'FAIL'}: {message}  ({elapsed:.2f}s)")
        passed += ok

    print(f"\n{passed}/{len(cases)} passed")
    return 0 if passed == len(cases) else 1


if __name__ == "__main__":
    sys.exit(main())
