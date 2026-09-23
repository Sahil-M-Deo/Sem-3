# Sokoban as a SAT / SMT problem

Play the puzzle here to get a feel for the rules:
<https://www.mathsisfun.com/games/sokoban.html>

## What is in this folder

| file | what it is |
|------------------------|-------------------------------------------------------|
| `sokoban.md` | this file: the rules, the task, and the Z3 syntax you need |
| `sokoban_template.py` | **the file you edit**: one function, `get_moves(grid)` |
| `checker.py` | marks your plan: `python3 checker.py tests/01.txt` |
| `visualize.py` | replays your plan board by board, for debugging |
| `tests/01.txt` ... `10.txt` | ten levels, easiest first |

You need Z3: `pip install z3-solver`. Run everything from inside this folder.
Your first run will say `INVALID: solver returned no moves`, because
`get_moves` is still a stub. That is the starting point.

---

## The puzzle

Sokoban ("warehouse keeper") is played on a rectangular grid. Every cell is
one of:

| symbol | meaning                        |
|--------|--------------------------------|
| `#`    | wall (impassable)              |
| ` `    | empty floor                    |
| `.`    | goal square                    |
| `$`    | box on floor                   |
| `*`    | box sitting on a goal          |
| `@`    | the player (pusher) on floor   |
| `+`    | the player standing on a goal  |

Rules of movement:

* On each step the player moves one cell **up, down, left or right** (`U`, `D`,
  `L`, `R`). No diagonals, no waiting is required.
* The player may step onto empty floor or a goal square.
* If the target cell holds a box, the player **pushes** it: the box slides one
  cell further in the same direction. The push is legal only if the cell
  *behind* the box is floor or a goal (never a wall, never another box).
* The player can only ever push a single box: never pull, never push two boxes
  at once.

**Goal:** reach a state in which every box stands on a goal square. The number
of goals equals the number of boxes.

We look for a plan of at most `T` steps. `T` (the *horizon* / *makespan*) is a
constant we fix up front; in general, if the solver returns UNSAT you increase
`T` and try again, and with a large enough `T`, UNSAT is a genuine proof that
the level cannot be solved.

For this assignment every level is guaranteed solvable in **at most 15 steps**,
so you can simply set `T = 15` and solve once. See "The assignment" below.

### Levels are not walled in

The levels in `tests/` have **no ring of `#` around the outside**. Walls are
interior obstacles and may sit anywhere, including on an edge row or column,
but they never fence the board in. Instead, the edge of the grid is itself
impassable: the player may not step off it, and a box may not be pushed off
it.

So in your encoding, "blocked" means *wall **or** outside the grid*:

```python
def blocked(r, c):
    return not (0 <= r < H and 0 <= c < W) or (r, c) in walls
```

Use that everywhere you would otherwise have tested only for a wall.

---

## From model to plan

Whatever variables you choose, the answer you hand back is a plain string of
directions, e.g. `"LUURDDRRRUULDD"`. Read the action your model picked at each
step and concatenate the letters. That string is what `checker.py` replays.

---

## The assignment

Edit **`sokoban_template.py`**. It contains exactly one function and nothing
else:

```python
def get_moves(grid):
    ...
```

* `grid` is the level as a **2-D list of characters** (`grid[r][c]`), using the
  symbols from the table at the top of this file.
* Return the plan as a **string** over `U D L R` (e.g. `"LUURDDRRRUULDD"`), or
  a list of those letters. Return `""` if you find no plan.
* Do the work with a Z3 encoding: fix a horizon `T`, choose your variables,
  write down the rules of Sokoban as constraints, solve, decode the model.
  Working out the right set of constraints is the assignment; the rules at the
  top of this file are the whole specification.

**Every level in `tests/` is solvable in at most 15 steps.** So you do not need
to search over `T` at all: fix `T = 15` and do a single solve. Give yourself a
`noop` action that is forced to sit at the *tail* of the plan
(`A[t][noop] -> A[t+1][noop]`); then a `k`-move solution appears as `k` real
moves followed by `15 - k` waits, which you drop when decoding.

### Use propositional logic only

Stick to what the slides cover: `Bool`, `Not`, `And`, `Or`, `Implies`, `==`,
and a plain `Solver()`. Do **not** use `AtMost`, `PbEq`, `Sum`, `If` or
`Optimize`. "Exactly one" is spelled out the long way, as in the slides:

```python
def exactly_one(s, lits):
    s.add(Or(lits))                                   # at least one
    for i in range(len(lits)):                        # no two at once
        for j in range(i + 1, len(lits)):
            s.add(Not(And(lits[i], lits[j])))
```

Because there is no `Optimize`, the plan you get back is *some* plan of at most
15 steps, not necessarily the shortest one. The player may wander a little
before finishing. That is fine: the checker only asks that the plan be legal
and that it end with every box on a goal.

The checker enforces a hard **5 minute** limit per call. If your encoding is
too slow on the larger levels, think about how many variables you are creating
and whether any of them can be ruled out up front.

---

## Z3 syntax you will need beyond the slides

The slides show `Bool`, `Not`, `And`, `Or`, `Implies`, `==`, `Solver()`,
`s.add(...)`, `s.check()` and `s.model()`. Here is everything else this
assignment uses.

### Making many variables at once

You will need far too many booleans to name by hand, so build them in a loop
and keep them in a dict or a list. Give every variable a **unique name**: two
`Bool` objects with the same name are the *same* variable to Z3.

```python
# X[t][(r, c)] is whatever you decide it means about cell (r, c) at time t
X = [{c: Bool(f"X_{t}_{c[0]}_{c[1]}") for c in cells} for t in range(T + 1)]

X[0][(2, 3)]        # the boolean for cell (2,3) at time 0
```

### Constants

`BoolVal(True)` and `BoolVal(False)` are the literal constants. You rarely need
them: instead of `Implies(X, BoolVal(False))`, just write `Not(X)`.

### Biconditional, again

`P == Q` builds the formula "P if and only if Q". It is an ordinary constraint,
so you add it:

```python
s.add(X[t + 1][c] == Or(some_formula, another_formula))
```

This is the natural shape for saying "here is exactly when `X` holds at the
next step", which is usually more convenient than two separate `Implies`.

### Reading the answer out of a model

`s.check()` returns one of the three constants `sat`, `unsat`, `unknown`
(compare with `==`). Only after `sat` may you call `s.model()`.

```python
if s.check() == sat:
    m = s.model()
```

A model `m` maps variables to values. There are two ways to look a value up:

| expression | what it gives you |
|-----------------------------------------|-------------------------------------------|
| `m[x]` | the value Z3 assigned to `x`, or `None` if it never had to decide |
| `m.eval(x)` | the value of any *expression* `x` under `m` |
| `m.eval(x, model_completion=True)` | same, but picks a value for anything undecided instead of returning `None` |

Both return **Z3 objects**, not Python `True` / `False`. Convert with
`is_true` (and `is_false`):

```python
is_true(m.eval(A[t][d], model_completion=True))     # -> a Python bool
```

Do **not** write `if m.eval(x):` or `if m[x] == True:`. A Z3 expression is
truthy in Python whatever its value, so those tests silently do the wrong
thing. Always go through `is_true(...)`.

`model_completion=True` matters when a variable is not mentioned by any
constraint: without it `eval` hands the expression straight back and `is_true`
answers `False`. Passing it is the safe habit.

### Putting the decode together

```python
plan = []
for t in range(T):
    for d in ["U", "D", "L", "R"]:
        if is_true(m.eval(A[t][d], model_completion=True)):
            plan.append(d)
return "".join(plan)
```

Note that the wait action is simply skipped, so a solution shorter than `T`
comes out as a shorter string.

---

## Checking a solution

`checker.py` is an **independent** validator: it never calls Z3. It imports
your solver, hands `get_moves` the level as a 2-D list, replays the returned
plan under the rules above, and reports whether every box ends on a goal.

```bash
# check sokoban_template.get_moves against a level file
python3 checker.py tests/03.txt

# check the reference solution instead
python3 checker.py tests/03.txt --solver sokoban_solution

# quietly (don't print every board)
python3 checker.py tests/10.txt -q
```

Exit code is `0` on a valid solution, `1` otherwise (including a solver that
runs longer than 300 s or returns nothing).

### Test suite

`tests/` holds ten levels of increasing difficulty, `01.txt` to `10.txt`. None
of them has a border wall, and all are solvable in 15 steps or fewer.

| level | size | boxes | what it is there for |
|-------|------|-------|-----------------------------------------------------|
| 01 | 3x1 | 1 | one push, to check the plumbing works |
| 02 | 5x3 | 1 | a wall to step around |
| 03 | 5x3 | 1 | the box sits on the **top edge**: "up" is off the grid, and the pusher stands on the edge too |
| 04 | 5x5 | 1 | the wall cuts the board in half; the player has to walk **all the way around** it to get behind the box |
| 05 | 5x5 | 2 | two boxes stacked: the obvious first push rams one **into the other** and is illegal, so they must be moved in the right order |
| 06 | 7x4 | 2 | two boxes on opposite sides of an interior wall |
| 07 | 6x4 | 3 | three boxes in a row |
| 08 | 9x5 | 3 | three boxes plus a wall band, a longer detour |
| 09 | 7x5 | 4 | four boxes, pushed in four different directions |
| 10 | 13x13 | 6 | the big one: six boxes, four push directions, seven wall blocks |

Levels 03, 04 and 05 exist to **catch a wrong encoding**. The move that looks
shortest in each of them breaks a rule, so an encoding that forgets the rule
happily returns it and the checker rejects the plan. Try them by hand to see
what is being tested:

```bash
python3 visualize.py tests/03.txt --plan LUUU   # pushes the box off the grid
python3 visualize.py tests/04.txt --plan RR     # walks into the wall
python3 visualize.py tests/05.txt --plan U      # pushes a box into a box
```

Run the whole set against the reference solution with:

```bash
for t in tests/*.txt; do python3 checker.py "$t" --solver sokoban_solution -q; done
```

---

## Debugging your encoding

`visualize.py` replays a plan step by step, printing the board and a note after
every move. Unlike the checker it does not stop at the first illegal move; it
flags it and carries on, so you can see the whole trajectory your model
produced.

```bash
python3 visualize.py tests/03.txt                       # uses sokoban_template
python3 visualize.py tests/03.txt --solver sokoban_solution
python3 visualize.py tests/03.txt --delay 0.3           # pause between frames
python3 visualize.py tests/03.txt --plan LUURR          # a literal plan, no solver
python3 visualize.py tests/03.txt --no-color
```
