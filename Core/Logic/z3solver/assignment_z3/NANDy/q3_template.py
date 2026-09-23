def synthesize(clauses):
    """
    Input
    -----
    clauses : list[list[int]]
        A CNF formula.  Each inner list is one clause, and each integer in it
        is a literal: +i means x_i, -i means NOT x_i.  Literals are never 0.
        The variables are x_1 .. x_n, where n = the largest |literal| that
        occurs anywhere in the formula, or 1 if the formula has no literals.

        Example: [[-1, 2]] is the formula (NOT x_1 OR x_2), and n = 2.

    Output
    ------
    list[list[int]]
        The NAND circuit, as a nested list.  Element k of the outer list is
        the list of inputs of gate k.  An input is an integer node id:

            0 .. n-1     the primary input x_1 .. x_n   (id j means x_{j+1})
            n .. n+k-1   the output of an earlier gate  (id n+j means gate j)

        Every id in element k must be strictly less than n + k, and no element
        may be empty.  The LAST gate of the list is the circuit output.

        Example: [[1], [0, 2]] over n = 2 is the two-gate circuit
                 gate 0 = NAND(x_2)          -- fan-in 1, i.e. NOT x_2
                 gate 1 = NAND(x_1, gate 0)  -- the output
    """
