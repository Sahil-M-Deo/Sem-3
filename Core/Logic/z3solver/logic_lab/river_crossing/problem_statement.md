# Exercise 3 — The River Crossing Puzzle

## The story

A farmer named **Shivansh** is standing on the **left bank** of a river. He has
just come back from the market with three things:

- a **Wolf**
- a **Goat**
- a **Cabbage**

His house is on the **right bank**. Shivansh wants to get himself *and* all
three of his purchases safely across the river.

## The boat

There is a single small boat. It is so small that it can only ever carry:

- **Shivansh**, plus
- **at most one** of {Wolf, Goat, Cabbage}.

Shivansh is the only one who can row, so **every** trip across the river has
Shivansh in the boat. A trip may also carry nothing but Shivansh.

An item can only be loaded into the boat if it is currently standing on the
**same bank as Shivansh** (he cannot summon the goat from across the river).

## What is dangerous

Some of these creatures cannot be left together on a bank **without Shivansh
there to supervise**:

- If the **Wolf** and the **Goat** are alone together (no farmer), the wolf eats
  the goat.
- If the **Goat** and the **Cabbage** are alone together (no farmer), the goat
  eats the cabbage.

The Wolf has no interest in the Cabbage.

This safety condition must hold at **every single moment** — including the
instants right before and right after a crossing, not just "on average".

## The goal

Find a sequence of crossings that:

1. starts with Shivansh, the Wolf, the Goat and the Cabbage all on the **left**
   bank,
2. ends with all four of them on the **right** bank,
3. never leaves a dangerous pair unsupervised, and
4. never violates the rules of the boat.

## Your job

You will **not** write a search algorithm. Instead you will describe the puzzle
as a logical formula whose *satisfying assignments are exactly the valid plans*,
hand that formula to Z3, and read the plan back out of the model.

You are given a fixed **time horizon** `N` — the plan must use **exactly** `N`
crossings (the farmer is in the boat on every step, so he swaps banks each time).
Your encoding should be satisfiable if and only if a valid plan of exactly `N`
crossings exists.

Because the farmer changes banks on every crossing, the parity of `N` matters:
everyone must finish on the right bank, so every **even** `N` is automatically
UNSAT. Among odd horizons, `N = 7` is the smallest that works; `N = 5` is too few,
and `N = 9` works by adding one out-and-back detour.

### The two families of variables

So that the provided `checker.py` can read and visualise your solution, your
encoding **must** use these Booleans with exactly this meaning though you can choose different encodings but you will have to write a different verifier for them:

| Variable   | Meaning                                                              |
|------------|---------------------------------------------------------------------|
| `L[e][t]`  | entity `e` is on the **LEFT** bank at time `t` (right bank = `not L`) |
| `M[e][t]`  | cargo `e` is carried in the boat during the step from `t` to `t+1`   |

Here `e` ranges over the entities (`'F'`, `'W'`, `'G'`, `'C'`), `t` ranges over
`0 .. N` for `L`, and `0 .. N-1` for `M` (there is one move between each pair of
consecutive time points). Only the three cargo items get `M` variables — the
farmer is in the boat on every step by definition.

### The five things your formula must say

1. **Initial state** — at `t = 0`, everyone is on the left bank.
2. **Goal state** — at `t = N`, everyone is on the right bank.
3. **The boat** — on every step the farmer switches banks; at most one cargo
   item is carried; a carried item must start on the farmer's bank and ends up
   on the opposite bank.
4. **Frame axiom** — if an item is *not* carried on a step, its bank does not
   change.
5. **Safety** — at every time `t`, no dangerous pair (`Wolf`/`Goat` or
   `Goat`/`Cabbage`) shares a bank while the farmer is on the other bank.

### Deliverable

Implement the function `build(N)` in `cross_river.py` so that it returns
`(solver, L, M)`. Then run:

```

python3 cross_river_template.py             # prints SAT / UNSAT and the raw plan
python3 checker.py cross_river_template.py  # independently verifies and draws the plan
```

The checker imports your `cross_river.py`, uses its horizon `N`, and checks the
returned plan independently in plain Python. A successful check prints
`Plan independently verified`. Run the commands from the
`z3_thursday/river_crossing` directory, or provide paths to both files from
another directory.

Try `N = 7`. Then try `N = 6` and see what Z3 says.
