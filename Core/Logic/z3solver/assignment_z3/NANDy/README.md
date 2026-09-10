# NANDy's Logic Lab

*Exact minimum-size NAND-circuit synthesis with Z3.*

---

## The story

You have been recruited by **NANDy**, an inventor with exactly one conviction:
any circuit worth building can be built out of NAND gates alone. NANDy has no
patience for AND, OR, NOT or XOR — and no patience for waste either. Every gate
costs money, and NANDy wants the **cheapest possible** circuit, not merely a
working one.

You are handed a Boolean formula in CNF. Build NANDy a combinational circuit
that outputs `True` exactly when the formula is satisfied, using **only NAND
gates**, and using **as few of them as mathematically possible**.

## The problem

Given a CNF formula over variables `x1 .. xn`, produce a circuit in which

* every gate is a NAND gate with **arbitrary fan-in** — any number of inputs,
  at least one. A NAND with a single input is a NOT gate;
* every gate reads only primary inputs and **strictly earlier** gates, so the
  circuit is acyclic;
* the last gate is the output, and for **every one of the `2^n` assignments**
  to `x1 .. xn` its value equals the value of the CNF;
* the number of gates is the **minimum over all such circuits**.

**Modelling requirement.** You must express this as a constraint problem and
have **Z3** find the circuit for you. Enumerating circuits in Python,
hand-crafting circuits from the clause structure, or hard-coding answers scores
zero, even when the returned circuit happens to be correct and minimal.

## What you write

`q3_template.py` contains one function and nothing else:

```python
def synthesize(clauses):
    ...
```

Its docstring is the complete specification of the input and output formats.
That function is the entire assignment. You may add helper functions of your
own in the same file, but `synthesize` must exist and must have that signature.

### Input

`clauses` is a `list[list[int]]`. Each inner list is one clause; each integer
is a literal, `+i` for `x_i` and `-i` for `¬x_i`. Literals are never `0`.

`n`, the number of primary inputs, is the largest `|literal|` appearing
anywhere in the formula, or `1` if the formula has no literals at all. So a
variable below that maximum which never appears is still a primary input, and
its value is a don't-care.

`[]` is the empty conjunction — the constant `True`. A `[]` **inside** the list
is an empty clause — the constant `False`.

### Output

A `list[list[int]]`. Element `k` is the list of inputs of gate `k`, and each
input is an integer node id:

| node id | meaning |
|---------|---------|
| `0 .. n-1` | primary input `x1 .. xn` (id `j` is `x_{j+1}`) |
| `n + j` | the output of gate `j` |

Every id in element `k` must be **strictly less than `n + k`**, and no element
may be empty. The **last** gate in the list is the circuit output.

### Example

For `clauses = [[-1, 2]]` — the formula `(¬x1 ∨ x2)`, so `n = 2` — a correct
minimal answer is

```python
[[1], [0, 2]]
```

which reads as

```
G1 = NAND(x2, x2)     # node 1 is x2; fan-in 1, so this is ¬x2
G2 = NAND(x1, G1)     # nodes 0 and 2 = x1 and gate 0; this is the output
```

and computes `¬(x1 ∧ ¬x2) = ¬x1 ∨ x2`. One gate is provably not enough, so
2 is the minimum.

---

## Files in this folder

| file | what it is |
|------|-----------|
| `README.md` | this document |
| `q3_template.py` | **the file you fill in and submit** (renamed `q3.py`) |
| `checker.py` | independent validator. Read it, don't change it. |
| `tests/*.txt` | sample inputs |
| `tests/*.opt` | the known minimum gate count for each sample input |

---

## Setup

```bash
python3 -m venv z3env
source z3env/bin/activate
pip install z3-solver
```

## Running the checker

`checker.py` **does not import Z3 and does not import anything else from this
folder.** It knows the rules of the problem and nothing about how you solved
it. It calls your `synthesize(clauses)` in a fresh subprocess, then brute-forces
all `2^n` assignments to decide whether the circuit you returned really computes
the formula.

From inside the `NANDy/` folder:

```bash
# every sample testcase
python3 checker.py q3_template.py

# one testcase
python3 checker.py q3_template.py tests/04_xor.txt

# check against a minimum you supply yourself
python3 checker.py q3_template.py tests/04_xor.txt --opt 4

# print the circuit for every case
python3 checker.py q3_template.py -v

# give slow cases more time (default 300s each)
python3 checker.py q3_template.py --timeout 900
```

Exit code `0` means every check passed; `1` means at least one failed.

The checker verifies, in this order:

1. **Type** — `synthesize` returns a list of non-empty lists of ints.
2. **Structure** — every input of gate `k` is a primary input or a strictly
   earlier gate (ids `< n + k`), so the circuit is acyclic and made only of
   NAND gates. Fan-in `0` is rejected.
3. **Function** — agreement with the CNF on all `2^n` assignments. On failure
   it prints a counterexample assignment.
4. **Minimality** — your gate count against `tests/<case>.opt`, or `--opt K`.

It also reports crashes, timeouts, and a missing or non-serialisable return
value, each with the reason.

### Testcase file format

```
C
<clause 1>
...
<clause C>
```

Line 1 is `C`, the number of clauses; each of the next `C` lines is one clause
as space-separated non-zero integers. To add your own case, drop
`tests/mycase.txt` in that format. If you know the true minimum, put it in
`tests/mycase.opt` (a single integer) and the checker will enforce it; without
the `.opt` file it verifies correctness only and just reports your gate count.

---

## Sample testcases

| file | what it encodes | minimum gates |
|------|-----------------|---------------|
| `01_example.txt` | `¬x1 ∨ x2` | 2 |
| `02_and.txt` | `x1 ∧ x2` | 2 |
| `03_unsat.txt` | unsatisfiable — constant `0` | 3 |
| `04_xor.txt` | `x1 XOR x2` | 4 |
| `05_majority3.txt` | majority of 3 | 4 |
| `06_chain.txt` | 3 mixed clauses over 3 vars | 5 |
| `07_four_vars.txt` | 4 clauses over 4 vars | 6 |
| `08_implications.txt` | an implication cycle — collapses hard | 2 |
| `09_onehot_xor.txt` | one-hot over `x1..x3` and `x4 XOR x5` | 9 |
| `10_tautology.txt` | no clauses — constant `1` | 2 |

Grading uses **hidden testcases** of comparable size. Make sure your solver is
fast enough on `09_onehot_xor.txt` before you submit.


## Submission

Submit a single file `q3.py` — your filled-in `q3_template.py`. It must be
self-contained: it may import the Python standard library and `z3`, and nothing
from this folder.
