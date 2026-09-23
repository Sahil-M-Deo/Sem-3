def solve(n, placed, blocked):
    """
    Input
    -----
    n : int
        The board is n x n, and you must place exactly n queens on it.
        Rows and columns are numbered 0 .. n-1, with (0, 0) the top-left
        corner, the first index the row and the second the column.

    placed : list[tuple[int, int]]
        Cells that MUST hold a queen. Possibly empty.
        Example: [(2, 2)] means there is a queen at row 2, column 2.

    blocked : list[tuple[int, int]]
        Cells that must NOT hold a queen. Possibly empty.
        Example: [(3, 0), (1, 0)] forbids queens on those two cells.

    Output
    ------
    list[int]
        A list of exactly n column indices: element r is the column of the
        queen standing in row r. No two queens may share a row, a column or a
        diagonal, every cell in `placed` must hold a queen, and no cell in
        `blocked` may hold one.

        Example: [1, 3, 0, 2] on a 4 x 4 board is

                . Q . .
                . . . Q
                Q . . .
                . . Q .

        Return -1 if no such placement exists.
    """
