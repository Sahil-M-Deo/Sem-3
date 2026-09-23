r"""
If you want to be able to check your solution using the checker provided make
sure your encoding adhere to the following such that L[e][t] holds this boolean
and M[e][t] holds this boolean:

  L[e][t]  ==  "entity e is on the LEFT bank at time t"   (right bank = not L)
  M[e][t]  ==  "cargo e crosses in the boat during step t -> t+1"
"""

from z3 import (Bool, Solver, And, Or, Not, Implies, AtMost,
                sat, is_true)

# --------------------------------------------------------------------
# The world
# --------------------------------------------------------------------
FARMER = 'F'
CARGO = ['W', 'G', 'C']            # everything that is not the farmer
ENTITIES = [FARMER] + CARGO

NAMES = {'F': 'Farmer', 'W': 'Wolf', 'G': 'Goat', 'C': 'Cabbage'}

# who eats whom, when the farmer is not watching
CONFLICTS = [('W', 'G'), ('G', 'C')]

# --------------------------------------------------------------------
# The time horizon.
#
# N is the number of river crossings the farmer makes -- the plan must
# use *exactly* N of them (t runs 0, 1, ..., N).
#
# The farmer swaps banks on every crossing, so after N crossings he is
# back on the left bank when N is even and on the right bank when N is
# odd. Since everyone (the farmer included) must finish on the RIGHT,
# every even N is automatically UNSAT -- N = 8 fails for the same parity
# reason as N = 2, not because 8 is "too many".
#
# Among the odd horizons, the puzzle needs at least 7 crossings, so:
#   N = 5  -> UNSAT (odd, but too few crossings to keep everyone safe)
#   N = 6  -> UNSAT (even: farmer would end on the wrong bank)
#   N = 7  -> SAT   (the classic solution)
#   N = 8  -> UNSAT (even: farmer would end on the wrong bank)
#   N = 9  -> SAT   (7 real crossings + one out-and-back detour)
# --------------------------------------------------------------------
N = 7


# --------------------------------------------------------------------
# (a)-(d): building the formula
# --------------------------------------------------------------------
def build(n):
    """Encode the puzzle over a horizon of n crossings.

    Returns (solver, L, M) where L[e][t] and M[e][t] are the Bools above.
    """
    ## You can later try it out with different set of Booleans too but 
    ## for the sake of uniformity across pre-defined functions and checker please use these encodings
    '''
    L[e][t]  ==  "entity e is on the LEFT bank at time t"   (right bank = not L)
    M[e][t]  ==  "cargo e crosses in the boat during step t -> t+1"
  '''

    L = {e: [Bool(f'L_{e}_{t}') for t in range(n + 1)] for e in ENTITIES}
    M = {e: [Bool(f'M_{e}_{t}') for t in range(n)] for e in CARGO}

    s = Solver()

    #TODO

    return s, L, M


def solve(n=N):
    """Return a plan for horizon n, or None if the formula is unsat.

    A plan is (states, moves):
      states[t] : dict e -> True if e is on the left bank at time t
      moves[t]  : the cargo carried during step t->t+1, or None
    """
    s, L, M = build(n)
    if s.check() != sat:
        return None

    m = s.model()
    states = [{e: is_true(m[L[e][t]]) for e in ENTITIES}
              for t in range(n + 1)]
    moves = [next((e for e in CARGO if is_true(m[M[e][t]])), None)
             for t in range(n)]
    return states, moves


# --------------------------------------------------------------------
def main():
    plan = solve(N)
    if plan is None:
        print(f"Unsatisfiable: no plan uses {N} crossings.")
        return
    print(f"Satisfiable! A plan exists with a horizon of {N}.")

    states, moves = plan
    print(f"  Plan states: {states}")
    print(f"  Plan moves: {moves}")


if __name__ == '__main__':
    main()
