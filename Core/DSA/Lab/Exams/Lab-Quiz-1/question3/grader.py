#!/usr/bin/python3

import sys
# caution: path[0] is reserved for script path (or '' in REPL)

#------------------------------
# Setup for problem
#------------------------------
exam                 = "Lab Quiz 1"
problem              = "printmanager"
prob_files           = ['main.cpp', 'printmanager.h']
student_files        = ['printmanager.cpp']
allowed_include      = ['printmanager.h', 'deque', 'iostream', 'vector', 'queue', 'stack', 'utility', 'algorithm']
disallowed_functions = ['append', 'alloc(', 'malloc(']

#------------------------------
# marks
#------------------------------
basic_marks  = 2
cancel_marks = 3
recall_marks = 3

grades_file = "grade.csv"

#---------------------------
# Paths
#---------------------------
sys.path.insert(1, '../../../../utils')
students_path = '../../submissions/'
prob_path     = './harness/'
tests_path    = './tests'
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
basic_tests  = ['test1', 'test2', 'test3', 'test4', 'test5', 'test6', 'test7', 'test8', 'test9', 'test10']
cancel_tests = ['test11', 'test12', 'test13', 'test14', 'test15', 'test16', 'test17', 'test18', 'test19', 'test20', 'test21', 'test22']
recall_tests = ['test23', 'test24', 'test25', 'test26', 'test27', 'test28', 'test29', 'test30', 'test31', 'test32', 'test33', 'test34']
mixed_tests  = ['test35', 'test36', 'test37', 'test38', 'test39', 'test40', 'test41', 'test42', 'test43', 'test44', 'test45', 'test46']
stress_tests = ['test47', 'test48', 'test49', 'test50']

tests = basic_tests + cancel_tests + recall_tests + mixed_tests + stress_tests

#------------------------------
# student lists
#------------------------------
students = [s for s in os.listdir(students_path) if os.path.isdir(students_path + '/' + s)]

#---------------------------------
# Allocate auto-grader
#---------------------------------
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

if len(sys.argv) < 2:
    base_grader.display_usage(sys.argv[0])
if len(sys.argv) == 3:
    base_grader.set_student(sys.argv[2])

# ---------------------
# Actions of auto-grader
# ---------------------
act = sys.argv[1]
if act in ['compile', 'policy', 'run', 'results', 'grade', 'package', 'email', 'all']:
    #----------------------------------------
    # Policy check, compile, run, and results
    #----------------------------------------
    base_grader.action(act)
    #-----------------------------------------------
    # Assign grade
    #-----------------------------------------------
    if act in ['grade', 'all']:
        full = base_grader.get_results()
        #----------------------------------
        # Compute total score
        #----------------------------------
        basic_pass  = (full[basic_tests + mixed_tests].sum(axis=1) == len(basic_tests + mixed_tests)).astype(int)
        cancel_pass = (full[cancel_tests].sum(axis=1) == len(cancel_tests)).astype(int)
        recall_pass = (full[recall_tests].sum(axis=1) == len(recall_tests)).astype(int)
        full['total'] = basic_marks * basic_pass + cancel_marks * cancel_pass + recall_marks * recall_pass
        
        limit_breaches = []
        for student in full['Roll No']:
            reasons = set()
            for test in tests:
                error_file = f'{base_grader.tmp_path}/{student}.{test}.error.txt'
                if os.path.isfile(error_file):
                    error = open(error_file, 'r').read()
                    if 'timeout' in error:
                        reasons.add('time')
                    if 'memory-limit' in error:
                        reasons.add('memory')
            limit_breaches.append(','.join(sorted(reasons)))
        full['limit_breach'] = limit_breaches

        full.to_csv(grades_file, index=False)
        #----------------------------------
        # Stress tests, reported only
        #----------------------------------
        print('Stress tests (not scored):')
        for t in stress_tests:
            print(f'  {t}: {int(full[t].sum())}/{len(full)} passed')
        #----------------------------------
        # Score distribution
        #----------------------------------
        counts = full['total'].value_counts().reindex(list(range(0, basic_marks + cancel_marks + recall_marks + 1)), fill_value=0)
        #---------------------------------
        # Printing
        #---------------------------------
        print(full[['Roll No', 'total', 'limit_breach']])
        full[['Roll No', 'total', 'limit_breach']].to_csv('post.csv', index=False)
        breached = full[full['limit_breach'] != ''][['Roll No', 'limit_breach']]
        print('Resource limit breaches:')
        print(breached.to_string(index=False) if not breached.empty else 'None')
        print(counts)
        print(full['total'].mean())

    #-----------------------------------------------
    # create package before sending emails
    #-----------------------------------------------
    if act in ['package']:
        base_grader.grading_files = ['./grader.py', 'requirements.txt', 'autograder-setup.sh', '../../../../utils/autograder.py']
        base_grader.package_replace_sequence = auto_remove
        base_grader.create_packages()
