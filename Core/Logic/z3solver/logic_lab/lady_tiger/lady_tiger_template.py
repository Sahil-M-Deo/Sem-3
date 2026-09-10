from z3 import *



## the lady tiger problem

# Rooms: 1, 2, 3 — exactly one contains the Lady (L).
# Signs on the doors:
#   Room1: "The Lady is not in this room"   ->  L != 1
#   Room2: "The Lady is not in this room"   ->  L != 2
#   Room3: "The Lady is in Room 2"          ->  L == 2
#
# Constraint: EXACTLY ONE of the three signs is true.

'''
we will need 6 variables
R1 R2 R3 S1 S2 S3

Ri : Room i has the lady
Si : Sign i is true
'''
set_option(proof = True)
#TODO
#Initialise your booleans

R1,R2,R3,S1,S2,S3 = Bools('R1 R2 R3 S1 S2 S3')

s = Solver()
#define a helper function exactly_one to encode exactly 
#one bool out of a, b, c is true


#exactly one sign is true
#TODO



#exactly one room has the lady
#TODO



#encode the statements.
#NOTE: for iff we use ""=="". Eg: s.add(A == B)
#TODO


if s.check() == sat:
    print(s.model())
else:
    print ("Unsatisfied")
    # print(s.proof())