"""
Independent checker for NANDy's Logic Lab.

This file does NOT import z3 and does NOT import anything else from this
folder.  It knows the rules of the problem and nothing about how you solved
it: it calls your ``synthesize(clauses)``, then brute-forces all 2**n input
assignments to decide whether the circuit you returned really computes the
formula.

What it verifies
----------------
  1. Type    -- synthesize returns a list of non-empty lists of ints.
  2. Structure -- every input of gate k is a primary input or a STRICTLY
     earlier gate (ids < n + k), so the circuit is acyclic and made only of
     NAND gates.  Fan-in 0 is rejected.
  3. Function -- for all 2**n assignments the last gate's value equals the
     value of the CNF.  On failure it prints a counterexample assignment.
  4. Minimality -- your gate count against tests/<case>.opt, or --opt K.

Usage
-----
    python3 checker.py q3.py                        # every tests/*.txt
    python3 checker.py q3.py tests/04_xor.txt
    python3 checker.py q3.py tests/04_xor.txt --opt 4
    python3 checker.py q3.py --timeout 900          # per case, default 300s

Exit code 0 means every check passed, 1 means at least one failed.

Testcase file format
--------------------
    line 1 : C, the number of clauses
    next C : one clause each, space-separated non-zero integers
             (+i is x_i, -i is NOT x_i)
"""

import argparse
import glob
import json
import os
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))

# Runs in a fresh interpreter so that a student crash, an infinite loop or a
# stray print cannot disturb the checker.  Clauses arrive as JSON on stdin and
# the circuit comes back as JSON on the last line of stdout.
DRIVER = r"""
import importlib.util, json, sys
path = sys.argv[1]
spec = importlib.util.spec_from_file_location("student_solution", path)
mod = importlib.util.module_from_spec(spec)
spec.loader.exec_module(mod)
if not hasattr(mod, "synthesize"):
    sys.stderr.write("module defines no function called synthesize\n")
    raise SystemExit(2)
clauses = json.loads(sys.stdin.read())
circuit = mod.synthesize(clauses)
sys.stdout.write("\n@@CIRCUIT@@" + json.dumps(circuit, default=list))
"""


class Invalid(Exception):
    """The submitted circuit is unacceptable, with a human-readable reason."""


# ---------------------------------------------------------------------------
#  The rules of the problem, implemented from scratch
# ---------------------------------------------------------------------------

def num_vars(clauses):
    n = 1
    for clause in clauses:
        for lit in clause:
            n = max(n, abs(lit))
    return n


def cnf_value(clauses, bits):
    for clause in clauses:
        if not any(bits[abs(l) - 1] if l > 0 else not bits[abs(l) - 1]
                   for l in clause):
            return False
    return True


def check_structure(circuit, n):
    """Validate the nested list.  Returns the normalised circuit."""
    if not isinstance(circuit, list):
        raise Invalid(f"synthesize returned {type(circuit).__name__}, expected a list")
    if not circuit:
        raise Invalid("synthesize returned an empty circuit (no gates)")

    clean = []
    for k, inputs in enumerate(circuit):
        if not isinstance(inputs, list):
            raise Invalid(f"gate {k}: expected a list of inputs, got "
                          f"{type(inputs).__name__}")
        if not inputs:
            raise Invalid(f"gate {k} has no inputs; fan-in 0 is not a gate")
        for j in inputs:
            if not isinstance(j, int) or isinstance(j, bool):
                raise Invalid(f"gate {k}: input {j!r} is not an integer node id")
            if j < 0:
                raise Invalid(f"gate {k}: negative node id {j}")
            if j >= n + k:
                if j < n:
                    raise Invalid(f"gate {k}: reads x{j + 1}, but there are only "
                                  f"{n} primary inputs")
                raise Invalid(
                    f"gate {k}: reads gate {j - n}, but a gate may only read "
                    f"strictly earlier gates (ids < {n + k}); this circuit is cyclic"
                )
        # A repeated input is harmless (NAND(a, a) is NOT a) but means the same
        # gate, so collapse duplicates before evaluating.
        clean.append(sorted(set(inputs)))
    return clean


def evaluate(circuit, n, bits):
    """Value of the last gate under the assignment ``bits``."""
    values = list(bits)
    for inputs in circuit:
        values.append(not all(values[j] for j in inputs))
    return values[-1]


def check_function(circuit, n, clauses):
    """Return None if the circuit matches the CNF, else a counterexample."""
    for t in range(1 << n):
        bits = [(t >> j) & 1 == 1 for j in range(n)]
        want = cnf_value(clauses, bits)
        got = evaluate(circuit, n, bits)
        if got != want:
            assignment = ", ".join(f"x{j + 1}={int(bits[j])}" for j in range(n))
            return (f"on ({assignment}) the formula is {int(want)} "
                    f"but the circuit gives {int(got)}")
    return None


def render(circuit, n):
    """Human-readable form of the circuit, for feedback only."""
    def name(j):
        return f"x{j + 1}" if j < n else f"G{j - n + 1}"

    lines = []
    for k, inputs in enumerate(circuit):
        args = [name(j) for j in inputs]
        if len(args) == 1:
            args = args * 2          # fan-in 1 is NOT, written NAND(a, a)
        lines.append(f"    G{k + 1} = NAND({', '.join(args)})")
    lines.append(f"    OUTPUT = G{len(circuit)}")
    return "\n".join(lines)


# ---------------------------------------------------------------------------
#  Driving one testcase
# ---------------------------------------------------------------------------

def read_case(path):
    with open(path) as fh:
        lines = [ln.strip() for ln in fh if ln.strip()]
    if not lines:
        return []
    count = int(lines[0])
    clauses = []
    for line in lines[1:1 + count]:
        clauses.append([int(tok) for tok in line.split() if int(tok) != 0])
    return clauses


def run_synthesize(program, clauses, timeout):
    """Call the student's synthesize in a subprocess.  Returns the circuit."""
    try:
        proc = subprocess.run(
            [sys.executable, "-c", DRIVER, os.path.abspath(program)],
            input=json.dumps(clauses),
            capture_output=True,
            text=True,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired:
        raise Invalid(f"timed out after {timeout}s")

    if proc.returncode != 0:
        detail = (proc.stderr or "").strip()[-1500:]
        raise Invalid(f"synthesize crashed (exit {proc.returncode})\n{detail}")

    marker = "@@CIRCUIT@@"
    if marker not in proc.stdout:
        raise Invalid("synthesize produced no circuit (did it return None?)")
    payload = proc.stdout.rsplit(marker, 1)[1].strip()
    try:
        return json.loads(payload)
    except json.JSONDecodeError:
        raise Invalid("the returned circuit is not JSON-serialisable "
                      "(it must be a plain nested list of ints)")


def expected_opt(case_path):
    opt_path = os.path.splitext(case_path)[0] + ".opt"
    if os.path.exists(opt_path):
        with open(opt_path) as fh:
            return int(fh.read().split()[0])
    return None


def run_case(program, case_path, opt, timeout, verbose):
    name = os.path.basename(case_path)
    clauses = read_case(case_path)
    n = num_vars(clauses)

    try:
        raw = run_synthesize(program, clauses, timeout)
        circuit = check_structure(raw, n)
    except Invalid as exc:
        print(f"[{name}] FAIL: {exc}")
        return False

    bad = check_function(circuit, n, clauses)
    if bad:
        print(f"[{name}] FAIL (wrong function): {bad}")
        if verbose:
            print(render(circuit, n))
        return False

    size = len(circuit)
    if opt is not None and size != opt:
        verdict = "not minimal" if size > opt else "impossibly small"
        print(f"[{name}] FAIL ({verdict}): used {size} gates, the minimum is {opt}")
        if verbose:
            print(render(circuit, n))
        return False

    note = "" if opt is None else " (= the known minimum)"
    print(f"[{name}] PASS: correct circuit, {size} gates{note}")
    if verbose:
        print(render(circuit, n))
    return True


def main(argv=None):
    parser = argparse.ArgumentParser(
        description="Independent checker for NANDy's Logic Lab (no z3 used).")
    parser.add_argument("program", help="your solution file, e.g. q3.py")
    parser.add_argument("cases", nargs="*",
                        help="testcase files (default: every tests/*.txt)")
    parser.add_argument("--opt", type=int,
                        help="assert this is the minimal gate count")
    parser.add_argument("--timeout", type=int, default=300,
                        help="seconds allowed per testcase (default 300)")
    parser.add_argument("-v", "--verbose", action="store_true",
                        help="print the circuit for every case")
    args = parser.parse_args(argv)

    if not os.path.exists(args.program):
        parser.error(f"no such file: {args.program}")

    cases = args.cases or sorted(glob.glob(os.path.join(HERE, "tests", "*.txt")))
    if not cases:
        parser.error("no testcases found")

    passed = 0
    for case in cases:
        opt = args.opt if args.opt is not None else expected_opt(case)
        if run_case(args.program, case, opt, args.timeout, args.verbose):
            passed += 1

    print()
    print(f"{passed}/{len(cases)} passed")
    return 0 if passed == len(cases) else 1


if __name__ == "__main__":
    sys.exit(main())
