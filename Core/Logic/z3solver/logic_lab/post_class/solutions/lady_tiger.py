from z3 import *

# # Declare three Boolean variables
# P = Bool('P')
# Q = Bool('Q')
# R = Bool('R')

# # Initialize solver
# s = Solver()

# # Add logic rules:
# # 1. P or Q is true
# s.add(Or(P, Q))
# # 2. If P is true, then R must be true
# s.add(Implies(P, R))
# # 3. R is strictly false
# s.add(Not(R))

# # Evaluate
# if s.check() == sat:
#     print("Satisfiable! The valid state is:")
#     print(s.model())
# else:
#     print("Unsatisfiable: No solution exists.")



## the lady tiger problem

'''
we will need 6 variables
R1 R2 R3 S1 S2 S3
'''

R1 = Bool('R1')
R2 = Bool('R2')
R3 = Bool('R3')
S1 = Bool('S1')
S2 = Bool('S2')
S3 = Bool('S3')

s = Solver()

# only one room has lady 


def exactly_one(S1,S2,S3):
    return Or((And(S1, Not(S2), Not(S3))),
              (And(S2, Not(S1), Not(S3))),
              (And(S3, Not(S2), Not(S1))))

s.add(exactly_one(S1,S2,S3))
s.add(exactly_one(R1,R2,R3))



#now statement one

#if and only if

a = (S1 == Not(R1))
b = (S2 == Not(R2))
c = (S3 == R2)

s.add(a,b,c)




if s.check() == sat:
    print("Satisfiable! The valid state is:")
    print(s.model())
else:
    print("Unsatisfiable: No solution exists.")

