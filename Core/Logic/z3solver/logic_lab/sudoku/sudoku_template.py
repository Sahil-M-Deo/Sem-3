from z3 import *

grid = []


def print_sudoku(m):
    print("Solved Sudoku Board:")
    for i in range(9):
        row_output = []
        for j in range(9):
            for k in range(9):
                if is_true(m.evaluate(grid[i][j][k])):
                    row_output.append(str(k + 1))
        print(" ".join(row_output))

#initialising a 9 x 9 x 9 grid with the following constraints
for i in range(9):
    l1 = []
    for j in range(9):
        l2 = []
        for k in range(9):
            ijk = Bool(f'{i}{j}{k}')
            l2.append(ijk)
        l1.append(l2)
    grid.append(l1)


s = Solver()


def load_clues_from_file(filename, solver, grid):
    with open(filename, 'r') as file:
        lines = file.readlines()

    row_idx = 0
    for line in lines:
        clean_line = line.replace(" ", "").replace(",", "").strip()

        if not clean_line:
            continue

        for col_idx, char in enumerate(clean_line):
            if char in '123456789':
                k = int(char) - 1
                solver.add(grid[row_idx][col_idx][k] == True)
            

        row_idx += 1
        if row_idx == 9:
            break


# only one digit in one cell
for i in range(9):
    for j in range(9):
        # TODO: add at least one digit in each cell
        pass
        for k in range(9):
            for k2 in range(k+1, 9):
                # TODO: add a no-duplicate constraint for this cell
                pass


# one row should have only one digit of a kind
for i in range(9):
    for k in range(9):
        # for the ith row and kth digit
        for j in range(9):
            for j2 in range(j+1, 9):
                # TODO: add a constraint that no two columns j and j2 should exist with same digit in the ith row
                pass


# one column should have only one digit of a kind
for j in range(9):
    for k in range(9):
        for i in range(9):
            for i2 in range(i + 1, 9):
                # TODO: add a no-duplicate constraint for this digit in the column
                pass


# every 3x3 block should have one number of a kind
for block_row in range(3):
    for block_col in range(3):
        for k in range(9):
            cells = [grid[r][c][k]
                     for r in range(block_row * 3, block_row * 3 + 3)
                     for c in range(block_col * 3, block_col * 3 + 3)]
            for a in range(len(cells)):
                for b in range(a + 1, len(cells)):
                    # TODO: add a no-duplicate constraint for digit k in this 3x3 block
                    pass

##You can run different tests given in the folder tests
##Just replace "tests/easy_puzzle1.txt" with your test file
load_clues_from_file("tests/easy_puzzle1.txt", s, grid)
if s.check() == sat:
    print("Satisfiable! The valid state is:")
    print_sudoku(s.model())
else:
    print("Unsatisfiable: No solution exists.")
