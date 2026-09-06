#!/usr/bin/python3

import sys
# caution: path[0] is reserved for script path (or '' in REPL)

#------------------------------
# Setup for problem
#------------------------------
exam                 = "Lab Quiz 1"
problem              = "hash"
prob_files           = ['main.cpp','hash.h']
student_files        = ['hash.cpp']
allowed_include      = ['hash.h']
disallowed_functions = ['cout','cerr', #'new ', 'delete ', 'free',
                        'append','alloc(','malloc('
                       ]

#------------------------------
# marks
#------------------------------
core_marks = 6
neg_marks  = 2

grades_file = "grade.csv"

#---------------------------
# Paths
#---------------------------
sys.path.insert( 1,    '../../../../utils' )
students_path        = '../../submissions/'
prob_path            = './harness/'
tests_path           = './tests'
students_path = "./students/"
prob_path = "./harness/"
tests_path = "./tests/"


import autograder
import pandas as pd
import numpy as np
import os
import re
import copy
import matplotlib.pyplot as plt

pd.set_option('display.max_rows', 200)

#------------------------------
# test cases
#------------------------------
# core   : non-negative keys only; repeated keys are kept, not collapsed
# neg    : keys that need ((val % N) + N) % N
# stress : reported but not scored; undefined behaviour and pathological cost
#          in common student idioms (-val / abs(val) at INT_MIN, and
#          `while (val < 0) val += N` for large negative keys), plus repeated
#          negative keys, which core already charges via tests 29-33 and 38

core_tests   = ['test1', 'test2', 'test3', 'test4', 'test5', 'test6', 'test7',
                'test8', 'test9', 'test10', 'test11', 'test12', 'test13', 'test14',
                'test15', 'test16', 'test17', 'test18', 'test19', 'test20', 'test21',
                'test22', 'test23', 'test24', 'test25', 'test26', 'test27', 'test28',
                'test29', 'test30', 'test31', 'test32', 'test33', 'test34', 'test35',
                'test36', 'test37', 'test38', 'test39', 'test40']
neg_tests    = ['test41', 'test42', 'test43', 'test44', 'test45', 'test46', 'test47',
                'test48', 'test49', 'test50', 'test51', 'test52', 'test53', 'test54']
stress_tests = ['test55', 'test56', 'test57', 'test58', 'test59', 'test60']

tests        = core_tests + neg_tests + stress_tests

#------------------------------
# student lists
#------------------------------

students             = [ s for s in os.listdir(students_path) if os.path.isdir(students_path+'/'+s) ]

#---------------------------------
# Allocate auto-grader
#--------------------------------
base_grader = autograder.Autograder(
    exam                 = exam,
    problem              = problem,
    prob_path            = prob_path,
    prob_files           = prob_files,
    students             = students,
    students_path        = students_path,
    student_files        = student_files,
    tests_path           = tests_path,
    tests                = tests,
    allowed_include      = allowed_include,
    disallowed_functions = disallowed_functions,
    timeout              = 10,
)
# base_grader.parallel = 1

if len(sys.argv) < 2 : base_grader.display_usage(sys.argv[0])
if len(sys.argv) == 3: base_grader.set_student( sys.argv[2] )

# ---------------------
# Actions of auto-grader
# --------------------
act  = sys.argv[1]
if act in ['compile','policy','run','results','grade','package','email','all']:
    #----------------------------------------
    # Policy check, compile, run, and results
    #----------------------------------------
    base_grader.action(act)
    #-----------------------------------------------
    # Assign grade
    #-----------------------------------------------
    if act in ['grade','all']:
        full =  base_grader.get_results()
        #----------------------------------
        # Compute total score
        #----------------------------------
        core_pass = (full[core_tests].sum( axis = 1 ) == len(core_tests)).astype(int)
        neg_pass  = (full[neg_tests ].sum( axis = 1 ) == len(neg_tests )).astype(int)
        full['total'] = core_marks*core_pass + neg_marks*neg_pass
        full.to_csv( grades_file, index=False )
        #----------------------------------
        # Stress tests, reported only
        #----------------------------------
        print('Stress tests (not scored):')
        for t in stress_tests:
            print( f'  {t}: {int(full[t].sum())}/{len(full)} passed' )
        #----------------------------------
        # Score distribution
        #----------------------------------
        counts = full['total'].value_counts().reindex( list(range(0, core_marks+neg_marks+1)), fill_value = 0 )
        #---------------------------------
        # Printing
        #---------------------------------
        print(full[['Roll No','total']])
        full[['Roll No','total']].to_csv( 'post.csv', index=False )
        print(counts)
        print(full['total'].mean())

    #-----------------------------------------------
    # create package before sending emails
    #-----------------------------------------------
    if act in ['package']:
        base_grader.grading_files = ['./grader.py','requirements.txt','autograder-setup.sh','../../../../utils/autograder.py']
        base_grader.package_replace_sequence = auto_remove
        base_grader.create_packages()
