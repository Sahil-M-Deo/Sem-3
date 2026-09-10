#!/usr/bin/env python3
"""Quiz grader.

    python check.py

Imports pop.py, calls get_model(), and checks the assignment it returns.
"""

import importlib.util
import pathlib
import sys

from z3 import is_true, sat, Or

HERE = pathlib.Path(__file__).resolve().parent
VARS = ['A', 'B', 'C', 'D', 'E']
TA = {'A': 'Tamanna', 'B': 'Shishir', 'C': 'Shivansh', 'D': 'Anshul', 'E': 'Shayna'}

# The one assignment that satisfies every clue.
KEY = {'A': True, 'B': False, 'C': True, 'D': False, 'E': True}


def load_pop():
    path = HERE / 'pop.py'
    if not path.exists():
        sys.exit('[check] pop.py not found next to check.py')
    spec = importlib.util.spec_from_file_location('pop', path)
    mod = importlib.util.module_from_spec(spec)
    try:
        spec.loader.exec_module(mod)
    except Exception as exc:  # noqa: BLE001
        sys.exit(f'[check] pop.py raised an error on import: {exc!r}')
    return mod


def main():
    mod = load_pop()

    if not hasattr(mod, 'get_model'):
        sys.exit('[check] pop.py has no function get_model()')

    try:
        m = mod.get_model()
    except Exception as exc:  # noqa: BLE001
        sys.exit(f'[check] get_model() raised an error: {exc!r}')

    if m is None:
        sys.exit('[check] get_model() returned None -- your constraints are UNSAT.')

    try:
        got = {name: is_true(m[getattr(mod, name)]) for name in VARS}
    except Exception:  # noqa: BLE001
        sys.exit('[check] could not read A..E from the model. '
                 'Did you keep the variable names?')

    print('Returned model:')
    for name in VARS:
        verdict = 'can eat 30' if got[name] else 'cannot'
        print(f'    {TA[name]:9} ({name}) = {str(got[name]):5}  -- {verdict}')

    # uniqueness check (only if the solver `s` is reachable)
    unique = None
    s = getattr(mod, 's', None)
    if s is not None and s.check() == sat:
        s.push()
        s.add(Or(*[getattr(mod, n) != m[getattr(mod, n)] for n in VARS]))
        unique = s.check() != sat
        s.pop()

    print()
    if got == KEY and unique is not False:
        print('PASS -- correct' + (' and unique.' if unique else '.'))
    elif got == KEY:
        print('ALMOST -- right assignment, but your constraints admit other '
              'models too. Tighten them.')
        sys.exit(1)
    else:
        print('FAIL -- not the expected assignment. Re-check your clues.')
        sys.exit(1)


if __name__ == '__main__':
    main()
