from z3 import *

# ------------------------------------------------------------------
#  Pre-written for you: solver + the five Booleans.
#  Do NOT rename A, B, C, D, E.
# ------------------------------------------------------------------
s = Solver()
A, B, C, D, E = Bools('A B C D E')


def get_model():
    """Add your constraints to `s`, then return a satisfying model.

    Return `s.model()` if the constraints are satisfiable, else `None`.
    """
    # TODO: one s.add(...) per clue
    # s.add(...)
    


    if s.check() == sat:
        return s.model()
    return None


if __name__ == '__main__':
    print(get_model())
