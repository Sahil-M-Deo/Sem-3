"""
Build a testcase file for the Great Cube.

A testcase is a small JSON file holding the faces of a scrambled cube and the
deadline T.  Writing 54 stickers by hand is miserable, so this script does it:
give it a scramble and it applies those moves to a solved cube and writes the
result out in the same format ``solve`` receives.

Usage
-----
    # a 4-move scramble, deadline 6
    python3 make_test.py tests/09_mine.json --scramble "R U2 F' L" -T 6

    # 5 random moves, reproducible, deadline 7
    python3 make_test.py tests/10_random.json --random 5 --seed 3 -T 7

    # mark a case that is NOT solvable in time (solve must return -1)
    python3 make_test.py tests/11_tight.json --scramble "R U F' L" -T 2 --unsolvable

    # just look at the cube a scramble produces, without writing a file
    python3 make_test.py --scramble "R" --show

The scramble is recorded in the file for your own reference; the checker never
looks at it and never uses it to solve anything.
"""

import argparse
import json
import os
import random
import sys

import check


def dump(payload):
    """JSON with one face per line, so the file stays readable by a human."""
    faces = ",\n".join(
        "    " + json.dumps(face, separators=(", ", ": "))
        for face in payload["cube"]
    )
    head = ",\n".join(
        f'  {json.dumps(k)}: {json.dumps(payload[k])}'
        for k in ("name", "scramble", "T", "solvable")
    )
    return "{\n" + head + ',\n  "cube": [\n' + faces + "\n  ]\n}\n"


def build(scramble):
    state = check.SOLVED
    for name in scramble:
        state = check.apply_move(state, name)
    return state


def main(argv=None):
    ap = argparse.ArgumentParser(description="Write a Great Cube testcase.")
    ap.add_argument("path", nargs="?", help="where to write the .json testcase")
    source = ap.add_mutually_exclusive_group(required=True)
    source.add_argument("--scramble", help='moves to apply, e.g. "R U2 F\'"')
    source.add_argument("--random", type=int, metavar="N",
                        help="apply N random moves instead")
    ap.add_argument("--seed", type=int, help="seed for --random")
    ap.add_argument("-T", "--deadline", type=int,
                    help="deadline in minutes (default: the scramble length)")
    ap.add_argument("--name", help="human-readable name (default: from filename)")
    ap.add_argument("--unsolvable", action="store_true",
                    help="mark that solve must return -1 for this T")
    ap.add_argument("--show", action="store_true",
                    help="print the cube and exit without writing")
    args = ap.parse_args(argv)

    if args.random is not None:
        rng = random.Random(args.seed)
        scramble, last = [], None
        while len(scramble) < args.random:
            name = rng.choice(sorted(check.MOVES))
            if name[0] != last:
                scramble.append(name)
                last = name[0]
    else:
        scramble = args.scramble.split()
        for name in scramble:
            if name not in check.MOVES:
                ap.error(f"{name!r} is not one of the 18 legal moves")

    state = build(scramble)
    deadline = args.deadline if args.deadline is not None else max(1, len(scramble))

    if args.show or not args.path:
        print(f"scramble: {' '.join(scramble) or '(nothing)'}")
        print(f"T = {deadline}")
        print(check.render(state))
        print()
        for face in check.to_cube_input(state):
            print("  " + json.dumps(face, separators=(", ", ": ")))
        return 0

    name = args.name or os.path.splitext(os.path.basename(args.path))[0]
    payload = {
        "name": name,
        "scramble": " ".join(scramble),
        "T": deadline,
        "solvable": not args.unsolvable,
        "cube": check.to_cube_input(state),
    }
    os.makedirs(os.path.dirname(os.path.abspath(args.path)), exist_ok=True)
    with open(args.path, "w") as fh:
        fh.write(dump(payload))
    print(f"wrote {args.path}  (scramble '{' '.join(scramble)}', T = {deadline})")
    return 0


if __name__ == "__main__":
    sys.exit(main())
