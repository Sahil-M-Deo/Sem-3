def solve(cube, T):
    """
    Input
    -----
    cube : list[dict[str, list[str]]]
        The scrambled Great Cube: a list of exactly 6 single-key dictionaries,
        one per face, in any order.

        In each dictionary the key is the colour of that face's CENTRE sticker,
        which is what names the face, and the value is the list of the 8
        stickers around that centre, read row by row, left to right, skipping
        the centre itself:

                c1 c2 c3
                c4  *  c5          key = colour of  *
                c6 c7 c8

        Colours are the single characters 'W', 'Y', 'G', 'B', 'R', 'O'.
        See README.md for which face each centre colour names and for the
        orientation each face is read in.

        Example (a solved cube):
            [{'W': ['W'] * 8}, {'R': ['R'] * 8}, {'G': ['G'] * 8},
             {'Y': ['Y'] * 8}, {'O': ['O'] * 8}, {'B': ['B'] * 8}]

    T : int
        The deadline, in minutes. Every face turn takes exactly one minute, so
        the solution may use at most T turns.

    Output
    ------
    list[str]
        The sequence of moves Tamanna must execute, in order, as a list of
        move names -- at most T of them. The 18 legal names are

            U  U'  U2    D  D'  D2    F  F'  F2
            B  B'  B2    L  L'  L2    R  R'  R2

        where a bare letter is a 90-degree clockwise turn of that face seen
        from outside, ' is counter-clockwise and 2 is a half turn.

        Example: ['R', 'U2', "F'", 'L']

        Return [] if the cube is already solved, and -1 if it cannot be solved
        within T minutes.
    """
