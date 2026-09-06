#!/usr/bin/python3

import sys
# caution: path[0] is reserved for script path (or '' in REPL)

#------------------------------
# Setup for problem
#------------------------------
exam                 = "Lab Quiz 1"
problem              = "split-list"
prob_files           = ['grader-main.cpp','circular.h']
student_files        = ['circular.cpp']
allowed_include      = ['circular.h']
disallowed_functions = ['cout','cerr','append','alloc(','malloc(']

#------------------------------
# test cases
#------------------------------

merge_tests = ['test1', 'test2', 'test3', 'test4', 'test5', 'test6',
               'test14', 'test16']
merge_functional_tests = ['test1', 'test2', 'test3', 'test4', 'test5',
                          'test6', 'test14']
split_tests = ['test7', 'test8', 'test9', 'test10', 'test11', 'test12',
               'test13', 'test15']
stress_tests = ['test17']
tests = merge_tests + split_tests + stress_tests

merge_marks = 4
merge_linear_marks = 2
split_marks = 4
time_limit_seconds = 2
memory_limit_mb = 256

grades_file = "grade.csv"

#---------------------------
# Paths
#---------------------------
sys.path.insert( 1,    '../../../../utils' )
students_path        = '../../submissions/'
prob_path            = 'harness/'
tests_path           = 'tests'
students_path = "./students/"
prob_path = "./harness/"
tests_path = "./tests/"


import autograder
import pandas as pd
import os

pd.set_option('display.max_rows', 200)

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
    timeout              = time_limit_seconds,
    memory_limit_mb      = memory_limit_mb,
    allow_markerless_template = True,
    extra_compile_flags  = '-fsanitize=undefined -fno-sanitize-recover=undefined',
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
        split_return_errors = []
        for student in full['Roll No']:
            compile_file = f'{base_grader.tmp_path}/{student}.compile.txt'
            compile_output = ''
            if os.path.isfile(compile_file):
                compile_output = open(compile_file, 'r', errors='replace').read()
            split_return_errors.append(
                'control reaches end of non-void' in compile_output or
                'no return statement in function returning non-void' in compile_output
            )
        full['split_return_warning'] = split_return_errors

        merge_functional_pass = (
            full[merge_functional_tests].sum(axis = 1) == len(merge_functional_tests)
        )
        merge_constant_time_pass = full['test16'].astype(bool)
        merge_stress_timeout = []
        for student in full['Roll No']:
            error_file = f'{base_grader.tmp_path}/{student}.test16.error.txt'
            error = ''
            if os.path.isfile(error_file):
                error = open(error_file, 'r').read()
            merge_stress_timeout.append('timeout' in error)
        merge_stress_timeout = pd.Series(merge_stress_timeout, index=full.index)
        merge_score = (
            merge_marks * (merge_functional_pass & merge_constant_time_pass).astype(int) +
            merge_linear_marks * (merge_functional_pass & merge_stress_timeout).astype(int)
        )
        split_pass = (full[split_tests].sum(axis = 1) == len(split_tests)).astype(int)
        full['total'] = merge_score + split_marks*split_pass

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
        full.to_csv( grades_file, index=False )
        print('Stress tests (not scored):')
        for test in stress_tests:
            print(f'  {test}: {int(full[test].sum())}/{len(full)} passed')
        #----------------------------------
        # Score distribution
        #----------------------------------
        counts = full['total'].value_counts().reindex( list(range(0, merge_marks+split_marks+1)), fill_value = 0 )
        #---------------------------------
        # Printing
        #---------------------------------
        print(full[['Roll No','total','limit_breach']])
        full[['Roll No','total','limit_breach']].to_csv( 'post.csv', index=False )
        breached = full[full['limit_breach'] != ''][['Roll No','limit_breach']]
        print('Resource limit breaches:')
        print(breached.to_string(index=False) if not breached.empty else 'None')
        print(counts)
        print(full['total'].mean())

    #-----------------------------------------------
    # create package before sending emails
    #-----------------------------------------------
    if act in ['package']:
        base_grader.grading_files = ['./grader.py','requirements.txt','autograder-setup.sh','../../../../utils/autograder.py']
        base_grader.package_replace_sequence = auto_remove
        base_grader.create_packages()
