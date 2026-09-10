# The Great Cube

*Propositional planning for a cube you are not allowed to think about.*

---

## The story

Tamanna's Tuesday was going fine until Jean Grey showed up in the lab.

It isn't a fight. Jean doesn't do fights. She just reads ahead. Every time
Tamanna starts to think about what to do next, Jean is already there, and the
thought quietly doesn't happen. Tamanna can still walk, still talk, still pick
things up. She just can't **plan**.

Survivable, except for the cube sitting on the table.

The rule attached to it is old and dumb and completely non-negotiable: **solve
the cube and the hold breaks.** Jean seems relaxed about this. Jean has watched
Tamanna try to solve a Rubik's cube before.

Here's the actual problem. Solving a cube is a loop — turn it, look at it,
decide, turn again. Jean lives in the "decide". The moment Tamanna looks at the
cube and thinks *okay so next I should* — gone.

Peter has been standing in the corner this entire time not being noticed, which
is more or less his whole skill set.

Jean reads minds. Jean does not read laptops.

So the plan is this, and there is no other plan:

> Tamanna decides nothing. She doesn't look at the cube once she starts. Peter
> works out the **entire sequence of turns up front**, hands it over, and she
> runs it like a script — no feedback, no adjusting, nothing to read.

One shot. If the sequence is right, six clean faces and she gets her head back.
If it's wrong, it's wrong all the way to the end, and now Jean knows what they
tried.

There's a clock, because there's always a clock: **T minutes**, one turn per
minute, and Peter is not getting an extension.

Peter, for the record, also cannot solve a Rubik's cube.

Peter can write down a formula that is satisfiable exactly when a solution
exists, and make a CDCL solver do the work.

That's you.

---

## What you are actually building

You get a scrambled cube and a deadline `T`. You must produce a sequence of at
most `T` face turns that leaves every face a single colour — or prove that no
such sequence exists.

The catch, and the whole reason this is a SAT problem: **the sequence is fixed
in advance.** You cannot look at the cube after move 3 and decide move 4. So
you cannot search the way a human does. You have to write down a formula whose
variables *are* the whole plan — the state of all 54 stickers at all `T+1`
moments in time, and which turn happens at each of the `T` steps — such that
the formula is satisfiable if and only if a `T`-move solution exists.

A CDCL solver then does the searching for you, and the satisfying assignment
*is* Tamanna's instruction list.

This shape of problem is called **bounded planning**. If the solver says UNSAT,
that is not a failure — it is a *proof* that the Cube cannot be solved in `T`
minutes.

**You must use only propositional (Boolean) variables.** No integers, no
arithmetic, no theory reasoning. Spidey's laptop runs CDCL and nothing else.

---

## The Cube

A standard 3×3×3 Rubik's cube. 6 faces, 9 stickers each, 54 in total. The 6
centre stickers never move relative to one another — this is what makes a face
identifiable at all, and you should lean on it hard.

### Colours and the faces they name

The Cube uses the standard Western colour scheme. Because centres never move,
**a colour names a face, permanently:**

| centre colour | code | face | meaning | opposite face |
|---|---|---|---|---|
| White | `W` | **U** | Up | D (Yellow) |
| Yellow | `Y` | **D** | Down | U (White) |
| Green | `G` | **F** | Front | B (Blue) |
| Blue | `B` | **B** | Back | F (Green) |
| Red | `R` | **R** | Right | L (Orange) |
| Orange | `O` | **L** | Left | R (Red) |

Hold the cube with **White up and Green facing you**; then Red is on your
right. That is the whole convention. A cube is **solved** when each face shows
nine copies of its own centre colour.

### The net

Unfold the cube like this. This picture defines everything.

```
                +----------+
                | U1 U2 U3 |
                | U4 U5 U6 |          U5 = W
                | U7 U8 U9 |
     +----------+----------+----------+----------+
     | L1 L2 L3 | F1 F2 F3 | R1 R2 R3 | B1 B2 B3 |
     | L4 L5 L6 | F4 F5 F6 | R4 R5 R6 | B4 B5 B6 |
     | L7 L8 L9 | F7 F8 F9 | R7 R8 R9 | B7 B8 B9 |
     +----------+----------+----------+----------+
       L5 = O     F5 = G     R5 = R     B5 = B
                | D1 D2 D3 |
                | D4 D5 D6 |          D5 = Y
                | D7 D8 D9 |
                +----------+
```

Read every face **row by row, left to right**, in the orientation shown above.
Each face is seen from *outside* the cube. The only thing you have to be
careful about is which neighbour each edge of a face touches, so here it is
written out:

| face | its top row touches | bottom row | left column | right column |
|---|---|---|---|---|
| **U** (W) | B | F | L | R |
| **D** (Y) | F | B | L | R |
| **F** (G) | U | D | L | R |
| **B** (B) | U | D | **R** | **L** |
| **L** (O) | U | D | **B** | **F** |
| **R** (R) | U | D | **F** | **B** |

The three bolded rows are where people go wrong. `B`, `L` and `R` are viewed
from outside, so their left/right are *not* the same as `F`'s. If your solver
is producing nonsense, check these first.

### From the net to `c1 .. c8`

The input gives you each face as a centre colour plus the **8 stickers around
it**, read in the same row-by-row order, with the centre skipped:

```
        c1 c2 c3              1  2  3
        c4  *  c5     <-->    4  5  6      * = 5 = the centre = the dict key
        c6 c7 c8              7  8  9
```

So for the face whose centre is `X`:

| given as | is facelet |
|---|---|
| `c1` | `X1` |
| `c2` | `X2` |
| `c3` | `X3` |
| `c4` | `X4` |
| `c5` | `X6` |
| `c6` | `X7` |
| `c7` | `X8` |
| `c8` | `X9` |

Note the jump: **`c5` is facelet 6, not 5.** Facelet 5 is the centre, and the
centre is the dictionary key, not one of the eight.

---

## Input format

```python
solve(cube, T)
```

`cube` is a list of exactly 6 single-key dictionaries, **in any order**. The
key is that face's centre colour; the value is the list of its 8 surrounding
stickers as above.

A solved cube:

```python
[
    {'W': ['W', 'W', 'W', 'W', 'W', 'W', 'W', 'W']},
    {'R': ['R', 'R', 'R', 'R', 'R', 'R', 'R', 'R']},
    {'G': ['G', 'G', 'G', 'G', 'G', 'G', 'G', 'G']},
    {'Y': ['Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y', 'Y']},
    {'O': ['O', 'O', 'O', 'O', 'O', 'O', 'O', 'O']},
    {'B': ['B', 'B', 'B', 'B', 'B', 'B', 'B', 'B']},
]
```

`T` is an integer: the deadline in minutes. One face turn takes one minute, so
your answer may use **at most `T` turns**.

---

## The moves

A move turns one face 90°, 180° or 270°, taking the layer of 9 stickers with
it. Standard notation, where **clockwise means clockwise as seen looking at
that face from outside the cube**:

| notation | meaning |
|---|---|
| `U` `D` `F` `B` `L` `R` | that face, 90° clockwise |
| `U'` `D'` `F'` `B'` `L'` `R'` | that face, 90° counter-clockwise |
| `U2` `D2` `F2` `B2` `L2` `R2` | that face, 180° |

**18 moves in total.** Each one costs one minute, including the 180° turns —
this is the half-turn metric. Nothing else is legal: no slice moves, no whole
cube rotations, no peeling stickers off.

Only the 6 centres are fixed. Everything else the turn touches — the 8 stickers
on the turning face, and the 12 stickers in the ring around it on the four
neighbouring faces — moves.

## Output format

Return the list of moves, as strings, in the order Tamanna must execute them:

```python
['R', 'U2', "F'", 'L']
```

The list must have length at most `T`. Return `-1` if the Cube cannot be solved
in `T` minutes.

An already-solved cube is solved in zero moves: return `[]`.

---

## Worked example — check your parsing against this

Start from the solved cube above and apply exactly one move, `R`.

`R` turns the Right (Red) face clockwise as seen from the right. Looking from
there, Front is on your left and Back is on your right, so the ring goes
**F → U → B → D → F**: the right column of `F` rises to the right column of
`U`, which passes to the left column of `B` (reversed, because `B` faces the
other way), which drops to the right column of `D`, which comes round to `F`.

The resulting cube:

```
                +----------+
                | W  W  G  |
                | W  W  G  |     the Green column arrived from F
                | W  W  G  |
     +----------+----------+----------+----------+
     | O  O  O  | G  G  Y  | R  R  R  | W  B  B  |
     | O  O  O  | G  G  Y  | R  R  R  | W  B  B  |
     | O  O  O  | G  G  Y  | R  R  R  | W  B  B  |
     +----------+----------+----------+----------+
                | Y  Y  B  |
                | Y  Y  B  |
                | Y  Y  B  |
                +----------+
```

which reaches `solve` as:

```python
[
    {'W': ['W', 'W', 'G', 'W', 'G', 'W', 'W', 'G']},
    {'R': ['R', 'R', 'R', 'R', 'R', 'R', 'R', 'R']},
    {'G': ['G', 'G', 'Y', 'G', 'Y', 'G', 'G', 'Y']},
    {'Y': ['Y', 'Y', 'B', 'Y', 'B', 'Y', 'Y', 'B']},
    {'O': ['O', 'O', 'O', 'O', 'O', 'O', 'O', 'O']},
    {'B': ['W', 'B', 'B', 'W', 'B', 'W', 'B', 'B']},
]
```

Read the `B` entry carefully — `B1`, `B4`, `B7` are White, and they are `c1`,
`c4` and `c6` in the list. If you get that one right, your face orientations
are correct.

With `T = 1`, the answer is `["R'"]`. With `T = 3` the answer is still one
move: any correct list of length ≤ `T` is accepted.

---

## Setup

```bash
python3 -m venv z3env
source z3env/bin/activate
pip install z3-solver
```

Use only Boolean variables and Boolean connectives. The solver underneath is
CDCL; keeping the encoding purely propositional is what lets it do its job.

## Files

| file | what it is |
|------|-----------|
| `README.md` | this document |
| `q4_template.py` | the file you fill in and submit |
| `check.py` | independent checker. Replays your moves. Don't change it. |
| `visualizer.py` | watch the encoding solve a cube, turn by turn |
| `make_test.py` | build your own testcase from a scramble |
| `tests/*.json` | sample cubes, each with its own deadline `T` |

`q4_template.py` contains one function, `solve(cube, T)`. Its docstring
specifies the input and the return value. This document specifies everything
else.

## Checking your answer

`check.py` does not import Z3 and does not import anything else from this
folder. It knows the rules of the cube and nothing about how you solved it: it
replays your move sequence one turn at a time and reports whether the cube ends
up solved.

```bash
# run your solve() on every testcase in tests/
python3 check.py q4_template.py

# run it on just one
python3 check.py q4_template.py tests/05_three_turns.json

# see what testcases exist
python3 check.py --list
```

You can also use it from your own code:

```python
from check import check
ok, message = check(cube, T, plan)
```

It rejects a plan that uses an illegal move name, one that runs past `T`, and
one that runs cleanly but leaves the cube unsolved — printing the final net so
you can see what went wrong.

## Watching it work

`visualizer.py` runs a scramble end to end and shows you both halves: the
solver ruling out each depth in turn (with the size of the formula at that
depth), and then the cube itself, redrawn after every turn of the plan.

```bash
python3 visualizer.py --test tests/05_three_turns.json
python3 visualizer.py --scramble "R U F' L2"
python3 visualizer.py --random 4 --seed 7
python3 visualizer.py --random 3 --delay 0.4 --no-color
```

The depth table is worth staring at: the UNSAT rows are the solver *proving*
no shorter plan exists, and that proof is what makes the plan it finally
returns the shortest one.

## The testcases

Each file in `tests/` is one cube and one deadline:

```json
{
  "name": "one turn",
  "scramble": "R",
  "T": 3,
  "solvable": true,
  "cube": [
    {"W": ["W", "W", "G", "W", "G", "W", "W", "G"]},
    {"R": ["R", "R", "R", "R", "R", "R", "R", "R"]},
    {"G": ["G", "G", "Y", "G", "Y", "G", "G", "Y"]},
    {"Y": ["Y", "Y", "B", "Y", "B", "Y", "Y", "B"]},
    {"O": ["O", "O", "O", "O", "O", "O", "O", "O"]},
    {"B": ["W", "B", "B", "W", "B", "W", "B", "B"]}
  ]
}
```

`cube` and `T` are exactly the two arguments handed to `solve`. `scramble` is
there so you can see what the cube is; **the checker never uses it** — it only
ever replays the moves *you* return. `solvable: false` marks a case where the
deadline is deliberately too short, so `solve` must return `-1`.

| file | scramble | `T` | expects |
|------|----------|-----|---------|
| `00_solved.json` | *(nothing)* | 3 | `[]` |
| `01_one_turn.json` | `R` | 3 | a plan |
| `02_two_turns.json` | `R U` | 4 | a plan |
| `03_half_turns.json` | `R2 U2 F2` | 4 | a plan |
| `04_opposites.json` | `R L U` | 4 | a plan |
| `05_three_turns.json` | `R U F'` | 5 | a plan |
| `06_four_turns.json` | `R U2 F' L` | 6 | a plan |
| `07_five_turns.json` | `R U F' L2 D` | 7 | a plan |
| `08_tight_deadline.json` | `R U2 F' L` | 2 | `-1` |

Any plan of length ≤ `T` that solves the cube is accepted — it does not have to
be the shortest one.

### Making your own

Writing 54 stickers by hand is miserable, so `make_test.py` does it for you:

```bash
# a scramble you choose, with a deadline
python3 make_test.py tests/09_mine.json --scramble "R U2 F' L" -T 6

# random moves, reproducible with a seed
python3 make_test.py tests/10_random.json --random 5 --seed 3 -T 7

# a deadline that is deliberately too short: solve must return -1
python3 make_test.py tests/11_tight.json --scramble "R U F' L" -T 2 --unsolvable

# just look at what a scramble produces, without writing a file
python3 make_test.py --scramble "R" --show
```

Anything you drop in `tests/` is picked up automatically by
`python3 check.py q4.py`, and can be run on its own with
`python3 visualizer.py --test tests/09_mine.json`.

Keep `T` small. The formula grows with `T` and the search space grows much
faster; scrambles of five or six turns already take several seconds.
