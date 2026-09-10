from z3 import *

grid = []

def print_sudoku(m):
    print("Solved Sudoku Board:")
    for i in range(9):
        row_output = []
        for j in range(9):
            for k in range(9):
                # Ask the model if grid[i][j][k] is True
                if is_true(m.evaluate(grid[i][j][k])):
                    # k is 0-8, so we add 1 to print digits 1-9
                    row_output.append(str(k + 1))
                    
        # Print the row
        print(" ".join(row_output))   




for i in range(9):
    l1 = []
    for j in range(9):
        l2 = []
        for k in range(9):
            ijk = Bool(f'{i}{j}{k}')
            l2.append(ijk)
        l1.append(l2)
    grid.append(l1)



s=Solver()
#now basically grid[i][j][k] = k is there in row i and col j 

def load_clues_from_file(filename, solver, grid):
    with open(filename, 'r') as file:
        lines = file.readlines()
        
    row_idx = 0
    for line in lines:
        # Clean the line: remove spaces, commas, and newlines
        clean_line = line.replace(" ", "").replace(",", "").strip()
        
        # Skip empty lines
        if not clean_line:
            continue
            
        for col_idx, char in enumerate(clean_line):
            # Check if the character is a valid clue (1 through 9)
            if char in '123456789':
                # Our k index is 0-8, so subtract 1 from the clue
                k = int(char) - 1
                
                # Lock this boolean variable to True in the solver
                solver.add(grid[row_idx][col_idx][k] == True)
                
        row_idx += 1
        # Stop after 9 valid rows
        if row_idx == 9:
            break

#only one digit in one cell
for i in range(9):
    for j in range(9):
        s.add(Or([grid[i][j][k] for k in range(9)]))

        ##No 2 k should be true for one cell
        for k in range(9):
            for k2 in range(k,9):
                if k != k2 : s.add(Not(And([grid[i][j][k], grid[i][j][k2]])))


#one row should have only one digit of a kind

for i in range(9):
    for k in range(9):
        s.add(Or([grid[i][j][k] for j in range(9)]))

        for j in range(9):
            for j2 in range(j,9):
                if j2 != j : s.add(Not(And([grid[i][j2][k], grid[i][j][k]])))


#on column should have only one number of a kind

for j in range(9):
    for k in range(9):
        s.add(Or([grid[i][j][k] for i in range(9)]))

        for i in range(9):
            for i2 in range(i+1,9):
                if i2 != i: s.add(Not(And([grid[i2][j][k], grid[i][j][k]])))



# every 3x3 block should have one number of a kind
for block_row in range(3):
    for block_col in range(3):
        for k in range(9):
            cells = [grid[r][c][k]
                     for r in range(block_row * 3, block_row * 3 + 3)
                     for c in range(block_col * 3, block_col * 3 + 3)]
            s.add(Or(cells))
            for a in range(len(cells)):
                for b in range(a + 1, len(cells)):
                    s.add(Not(And(cells[a], cells[b])))
                    
load_clues_from_file("z3_thursday/sudoku/input_1.txt", s,grid)
if s.check() == sat:
    print("Satisfiable! The valid state is:")
    print_sudoku(s.model())
else:
    print("Unsatisfiable: No solution exists.")







