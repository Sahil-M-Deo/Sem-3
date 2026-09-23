# The Treaty

*N queens, one hall, and nobody making eye contact.*

---

## The story

There are `N` queens and one hall, and the hall is an `N × N` grid of tiles.

The queens are not on speaking terms. Nobody remembers exactly why; it has been
a while. The treaty that ended it is one sentence long:

> **No queen may be able to see another one.**

A queen sees along her row, along her column, and along both diagonals, and she
sees all the way to the wall. There is nothing to hide behind.

All `N` of them have to stand somewhere. A few have already picked their tile
and are not moving. A few tiles are off limits — that is where the last
argument happened, and the floor has not recovered.

You have been handed the floor plan and told to make it work, or to state in
writing that it cannot be done.

---

## The problem

Place exactly `N` queens on an `N × N` board so that

* no two share a **row**,
* no two share a **column**,
* no two share a **diagonal** — either direction, any length,
* every cell in `placed` holds a queen,
* no cell in `blocked` holds a queen.

Report a placement, or report that none exists.

**Modelling requirement.** You must express this as a constraint problem and
have **Z3** find the placement. Backtracking it yourself in Python, or using a
known formula for queen placements, scores zero.

Use **only Boolean variables and Boolean connectives**. In particular, do not
reach for `Sum(If(cell, 1, 0)) == n` to count the queens — the count follows
from the row and column rules on its own, and working out why is part of the
exercise.

## The board

Rows and columns are numbered `0 .. N-1`. Cell `(r, c)` is row `r`, column `c`,
with `(0, 0)` the **top-left** corner — the first index is the row, the second
is the column, exactly as in `grid[i][j]`.

Two cells `(r1, c1)` and `(r2, c2)` are on a common diagonal when

```
|r1 - r2| == |c1 - c2|
```

which covers both the `↘` and the `↙` directions in one line.

## What you write

`q5_template.py` contains one function:

```python
def solve(n, placed, blocked):
    ...
```

Its docstring is the complete specification of the input and the return value.
That function is the entire assignment. You may add helpers of your own in the
same file, but `solve` must exist with that signature.

### Input

| argument | type | meaning |
|---|---|---|
| `n` | `int` | board size; you place exactly `n` queens |
| `placed` | `list[tuple[int, int]]` | cells that **must** hold a queen (possibly empty) |
| `blocked` | `list[tuple[int, int]]` | cells that must **not** hold a queen (possibly empty) |

### Output

A `list[int]` of length `n`: element `r` is the column of the queen in row `r`.

Because no two queens may share a row and there are `n` queens on `n` rows,
every row holds exactly one — which is why one column index per row is enough
to describe the whole board.

Return `-1` if no valid placement exists.

---

## Worked example

```python
solve(4, [], [])
```

`4 × 4` has exactly two solutions. One of them is `[1, 3, 0, 2]`:

```
        col: 0 1 2 3
    row 0:   . Q . .        queen in row 0 at column 1
    row 1:   . . . Q        queen in row 1 at column 3
    row 2:   Q . . .        queen in row 2 at column 0
    row 3:   . . Q .        queen in row 3 at column 2
```

Check the treaty holds: no column repeats in `[1, 3, 0, 2]`, and for every pair
of rows the column gap differs from the row gap — rows 0 and 2 are 2 apart and
their columns are 1 apart, rows 1 and 3 are 2 apart and their columns are 1
apart, and so on. Nobody can see anybody.

The other solution is `[2, 0, 3, 1]`, the mirror image. **Either is accepted** —
any placement satisfying the treaty is a correct answer.

### With constraints

```python
solve(8, [(2, 2)], [(3, 0), (1, 0)])
```

An `8 × 8` board where the queen in row 2 has already claimed column 2, and
rows 1 and 3 may not use column 0.

### No solution

```python
solve(2, [], [])   ->  -1
solve(3, [], [])   ->  -1
```

Two queens on a `2 × 2` board see each other whatever you do, and `3 × 3` is no
better. `N = 1` is trivially fine, and every `N ≥ 4` has at least one solution
— though `placed` and `blocked` can still make a particular board impossible.

---

## Setup

```bash
python3 -m venv z3env
source z3env/bin/activate
pip install z3-solver
```

## Files

| file | what it is |
|------|-----------|
| `README.md` | this document |
| `q5_template.py` | the file you fill in and submit |

`q5_template.py` contains one function, `solve(n, placed, blocked)`. Its
docstring specifies the input and the return value. This document specifies
everything else.

## Submission

Submit a single file `q5.py` — your filled-in `q5_template.py`. It must be
self-contained: it may import the Python standard library and `z3`, and nothing
from this folder.
