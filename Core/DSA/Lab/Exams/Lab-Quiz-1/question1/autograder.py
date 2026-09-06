#!/usr/bin/python3

import re
import os
import shutil
import sys
import codecs
from datetime import datetime, timedelta, timezone
import secrets
import string
import csv
from joblib import Parallel, delayed
from tqdm import tqdm
import subprocess
import filecmp 
import time
import random
import statistics
import pandas as pd
import networkx as nx
import zipfile
from collections import defaultdict
import platform

#-----------------------------------------------------------
# 

# def text_to_map2( output, result ):
#     if output == None or (len(output) < 1): return
#     for o in output:
#         splits = o.strip().split(':')
#         if len(splits) > 1:            
#             results[ splits[0] ] = splits[1]
#     return

# def text_to_map(output):
#     results = {}
#     current = None
#     for o in output:
#         line = o.rstrip('\n')
#         stripped = line.lstrip()
#         low = stripped.lower()
#         if low.startswith('result:') or low.startswith('time:'):
#             key, val = stripped.split(':', 1)
#             key = key.strip().capitalize()
#             val = val.lstrip()
#             results[key] = val
#             current = key
#         else:
#             if current is None:
#                 current = 'Result'
#                 results.setdefault(current, '')
#             if results[current] == '':
#                 results[current] = line
#             else:
#                 results[current] += '\n' + line
#     return results

def text_to_map( output, default_results = {'':False}, result_key = 'Result'):
    # --------------------------------------
    #  If there is nothing in the output
    # --------------------------------------
    if output == None or (len(output) < 1): return default_results
    # --------------------------------------
    #  Process key value pairs in the output
    # --------------------------------------
    results = {}
    for o in output:
        splits = o.strip().split(':')
        if len(splits) > 1:            
            results[ splits[0] ] = splits[1]
    # --------------------------------------
    #  Handling the result key
    # --------------------------------------
    if not (result_key in results) : return default_results
    results[''] = results[result_key]
    results.pop(result_key, None)
    return results

def comment_remover(text):
    def replacer(match):
        s = match.group(0)
        if s.startswith('/'):
            return " " # note: a space and not an empty string
        else:
            return s
    pattern = re.compile(
        r'//.*?$|/\*.*?\*/|\'(?:\\.|[^\\\'])*\'|"(?:\\.|[^\\"])*"',
        re.DOTALL | re.MULTILINE
    )
    return re.sub(pattern, replacer, text)

def install_deps():
    def package( p ):
        os.system(f'sudo apt install {p} -y')
    for p in ['llvm','clang,''python3-pandas','python3-networkx']:
        package(p)

def zip_folder(folder_path, zip_filename):
    with zipfile.ZipFile(zip_filename, 'w', zipfile.ZIP_DEFLATED) as zipf:
        for root, dirs, files in os.walk(folder_path):
            for file in files:
                file_path = os.path.join(root, file)
                zipf.write(file_path, os.path.relpath(file_path, folder_path))

def if_space_exit( ss ):
    for s in ss:
        if " " in s:
            print(f"Space in path {s}! This script cannot run.")
            print(f"Move the autograder folder to a location where")
            print(f"there is no SPACE in the ancestor folders.")
            exit()
    
        
#-----------------------------------------------------------

class Autograder:
    prob_path       = './harness'
    prob_files      = []
    students_path   = ''
    student_files   = []
    allowed_include = []
    disallowed_functions = ['cout','cerr','new ','append','alloc(','malloc(','free']
    
    # --------------------------
    # Running configurations
    # --------------------------
    test_repeat   =  1
    timeout       =  1
    parallel      = -1
    exam          = ""
    problem       = ""
    plag_check    = False
    
    tests_path    = './tests'
    tests         = []
    tmp_path      = ""
    students      = []
    roll_file      = None
    seating        = pd.DataFrame()
    grading_files = []
    expected_output_file   = 'output.txt'
    cmp_function = None
    template_path = "./template/"
    cheating_list = 'cheats.csv'
    skip_attendance_while_packaging = True
    
    #-----------------------------------------------------------


    # constructor function    
    def __init__(self,
                 exam,
                 problem,
                 prob_path,
                 prob_files,     
                 students_path,        
                 student_files,        
                 allowed_include,      
                 disallowed_functions, 
                 timeout,              
                 tests_path,           
                 tests,
                 students = None,
                 roll_file = None,
                 template_path = None,
                 skip_attendance_while_packaging = True,
                 memory_limit_mb = None,
                 allow_markerless_template = False,
                 extra_compile_flags = ''
                 ):
        # -------------------------------------------
        # Initialize object variables
        # -------------------------------------------
        self.exam                 = exam             
        self.problem              = problem            
        self.prob_path            = os.path.abspath(prob_path)
        self.prob_files           = prob_files            
        self.students_path        = os.path.abspath(students_path)
        if students == None:
            self.students         = [ s for s in os.listdir(self.students_path) if os.path.isdir(self.students_path+'/'+s) ]
        else:
            self.students         = students
        if os.path.exists(f'{students_path}/{self.cheating_list}'):
            for s in pd.read_csv( f'{students_path}/{self.cheating_list}' )['Roll No'].to_list():
                self.students.remove(s)
        if os.path.exists(f'{students_path}/seating.csv'):
            self.seating = pd.read_csv( f'{students_path}/seating.csv' )
        if template_path : self.template_path = template_path
        self.roll_file = roll_file
        self.check_extra_students()            
        self.student_files        = student_files        
        self.allowed_include      = allowed_include      
        self.allowed_recursion    = True    
        self.disallowed_functions = disallowed_functions 
        self.timeout              = timeout              
        self.memory_limit_mb      = memory_limit_mb
        self.allow_markerless_template = allow_markerless_template
        self.extra_compile_flags       = extra_compile_flags
        self.tests_path           = os.path.abspath(tests_path)
        self.tests                = tests
        self.tmp_path              = f'/tmp/cs213/{self.problem}'
        self.skip_attendance_while_packaging = skip_attendance_while_packaging
        # -------------------------------------------
        # This script cannot handle paths with spaces
        # -------------------------------------------
        if_space_exit([self.prob_path, self.students_path, self.tests_path, self.tmp_path] )
        # -------------------------------------------
        # Create path for temporary files 
        # -------------------------------------------
        os.system(f'mkdir -p {self.tmp_path}')
        
    # students = students #[1:10]
    # action  = sys.argv[1]

    def check_extra_students(self):
        if self.roll_file and os.path.exists(self.roll_file):
            enrolled = pd.read_csv( self.roll_file )
            enrolled = enrolled['Roll No'].to_list()
            extra = []
            found = []
            for s in self.students:
                if not s in enrolled:
                    extra.append(s)
                else:
                    found.append(s)
            if extra:
                print('Extra students found:',extra)
            self.students = found
            
                    
    # -------------------------------------
    # Print marks distribution
    # -------------------------------------
    def print_distribution( self, full, col):
        if len(self.students) == 1:
            return
        maximum = full[col].max()
        if pd.isna(maximum): maximum = 0
        counts = full[col].value_counts().reindex( list(range(0, maximum+1)), fill_value = 0 )
        print(counts)
        print(full[col].mean())
        
    #----------------------------------
    # Compile
    #----------------------------------

    def cleanup_student_code( self, student, rm_includes = True ):
        spath = self.students_path
        new_sfiles = []
        for f in self.student_files:
            cleaned_file = f'{self.tmp_path}/{student}.{f}'
            if os.path.isfile(cleaned_file):
                os.remove(cleaned_file)
            if not os.path.isfile( f'{spath}/{student}/{f}' ):
                print(f'{spath}/{student}/{f} not found!')
                continue
            text = open( f'{spath}/{student}/{f}', 'r' ).read()
            # --------------------------
            # Template check
            # --------------------------
            if os.path.exists(f'{self.template_path}/{f}'):
                tmplate_text = open( f'{self.template_path}/{f}', 'r' ).read()
                template_parts = [[]]
                lines = tmplate_text.split('\n')
                in_section = False
                for l in lines:
                    if "// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE" in l:
                        assert(not in_section)
                        in_section = True
                    elif "// Your code ends here -- DO NOT EDIT ANYTHING BELOW" in l:
                        assert(in_section)
                        in_section = False
                        template_parts.append([])
                    else:
                        if not in_section: template_parts[-1].append(l)
                        
                student_parts = []
                lines = text.split('\n')
                in_section = False
                malformed_markers = False
                for l in lines:
                    if "// Your code starts from here -- DO NOT EDIT ANYTHING ABOVE" in l:
                        if in_section:
                            malformed_markers = True
                            break
                        in_section = True
                        student_parts.append([])
                    elif "// Your code ends here -- DO NOT EDIT ANYTHING BELOW" in l:
                        if not in_section:
                            malformed_markers = True
                            break
                        in_section = False
                    else:
                        if in_section: student_parts[-1].append(l)

                malformed_markers = malformed_markers or in_section
                marker_count_mismatch = len(template_parts) - 1 != len(student_parts)
                markerless = (
                    self.allow_markerless_template and
                    not malformed_markers and
                    len(student_parts) == 0
                )
                if malformed_markers or (not markerless and marker_count_mismatch):
                    print(f'{student}/{f} did not match with the template!')
                    continue # do not save file causing error
                if not markerless:
                    build_file = []
                    for i in range(0,len(template_parts)-1):
                        build_file.append(template_parts[i])
                        build_file.append(student_parts[i])
                    build_file.append(template_parts[-1])
                    text = [line for parts in build_file for line in parts ]
                    text = '\n'.join(text)
            
            text = comment_remover(text)
            # --------------------------
            # Remove bad include files
            # --------------------------
            lines = text.split('\n')
            clines = []
            for line in lines:
                if '#include' in line:
                    found = True
                    for include in self.allowed_include:
                        if include in line:
                            found = False
                            break
                    if found and rm_includes:
                        line = '//'+line
                clines.append(line)
            text = "\n".join(clines)
            # --------------
            # save the files
            # --------------
            open( cleaned_file, 'w' ).write(text)
            new_sfiles.append( f"'{self.tmp_path}/{student}.{f}'" )
        return new_sfiles
    
    def source_files(self,student, rm_includes = True):
        ppath = self.prob_path #.replace(" ", "\\ ")
        # spath = self.students_path    
        files = [ f"'{ppath}/{f}'" for f in self.prob_files]
        sfiles = self.cleanup_student_code( student, rm_includes )
        return files + sfiles

    def source_paths(self,student):
        ppath = "'"+self.prob_path+"'" # .replace(" ", "\\ ")
        return [ppath, f"'{self.students_path}/{student}'"]
        # spath = os.path.abspath(self.students_path)
        # tpath = os.path.abspath(self.tmp_path)
        # return [ppath,spath,tpath]

    def compile_internal(self,student, show_error=False, rm_includes= True, verbose=False):
        # ---------------------
        extra_inclues = ""
        if not rm_includes: extra_inclues = "-extra-includes"
        # ---------------------
        includes  = " ".join( [ f'-I{p}' for p in self.source_paths(student)] )
        # includes += " -I"+self.students_path+f"/{student}"        
        try:
            files = self.source_files(student, rm_includes)
            # remove header files, gives error on mac
            files = [ f for f in files if f[-5:-1] == ".cpp"] 
        except Exception as e:
            print( student, "Failed to process files: ", '{}!'.format(e) )
            return
        files = " ".join(files)
        dump_file = f"2> {self.tmp_path}/{student}.compile.txt{extra_inclues}"
        if show_error:
            dump_file = ""
        cmd = f"g++ -Wall -std=c++2a -O2 {self.extra_compile_flags} -o {self.tmp_path}/{student} {includes} {files} {dump_file}"
        if verbose == True:
            print(cmd)
        else:
            if extra_inclues:
                print( f"Compiling {student} with extra inclues!")
            else:
                print( f"Compiling {student}")
        os.system(cmd)

    def compile(self,student, show_error=False, verbose = False):
        self.compile_internal(student, show_error, verbose = verbose )
        if not self.check_compile_ok(student):
            self.compile_internal(student, show_error = show_error, rm_includes = False, verbose = verbose)

    def compile_all(self, show_error=False, verbose=False):
        Parallel(n_jobs=-1)( delayed(self.compile)(student, show_error=show_error, verbose=verbose) for student in tqdm(self.students) )

    #----------------------------------
    # Compile check
    #----------------------------------

    def check_compile_ok(self,student):
        compile_file = f"{self.tmp_path}/{student}.compile.txt"
        if not os.path.isfile(compile_file):
            return False
        f = open(compile_file, 'rb')
        s = f.read()
        if b'error' in s:
            return False
        else:
            return True

    def check_compile_with_extra_includes_ok(self,student):
        compile_file = f"{self.tmp_path}/{student}.compile.txt-extra-includes"
        if not os.path.isfile(compile_file):
            return False
        f = open(compile_file, 'rb')
        s = f.read()
        if b'error' in s:
            return False
        else:
            return True

    def report_compile_error(self):
        errors = {}
        for student in self.students:
            if self.check_compile_ok(student):
                errors[student] = "PASS"
            elif self.check_compile_with_extra_includes_ok(student):
                errors[student] = "PASS-WITH-INCLUDES"                
            else:
                errors[student] = "FAIL"
        df = pd.DataFrame(errors.items())
        df = df.rename(columns = { 0: 'Roll No', 1: 'Compile Success' })
        return df

    #----------------------------------
    # Policy checks
    #----------------------------------

    def include_check(self,student,lines):
        lines = [line for line in lines if "#include" in line]
        for line in lines:
            found = False
            for include in self.allowed_include:
                if include in line:
                    found = True
            if not found:
                with open(f'{self.tmp_path}/{student}.policy.txt', 'a') as f:
                    f.write(f'Dis-allowed includes: {line}\n')

    def disallowed_functions_check(self,student,lines):
        for line in lines:
            line = line.split('//')[0]
            for f in self.disallowed_functions:
                if f in line:
                    with open(f'{self.tmp_path}/{student}.policy.txt', 'a') as f:
                        f.write(f'Dis-allowed functions:{line}\n')
                        
    def recursion_check(self,student,sfile):
        # sfile = f"{self.tmp_path}/{student}.{f}"
        includes = " ".join( [ f'-I{p}' for p in self.source_paths(student)] )
        cmd = f'clang++ -S -emit-llvm {includes} {sfile} -o - 2> /dev/null | opt -passes=dot-callgraph  > /dev/null 2>&1'
        # print(cmd)
        os.system(cmd)
        os.system(f'mv \\<stdin\\>.callgraph.dot {sfile}.cfg.dot')
        text = open( f'{sfile}.cfg.dot', 'r' ).read()
        p = re.compile(r'Node0x(.*) -> Node0x(.*);')
        out = re.findall( p, text)
        G = nx.DiGraph(out)
        # if len(nx.simple_cycles(G)) > 0:
        #     print(student)
        for cycle in nx.simple_cycles(G):
            # print(student)
            lines = open( sfile, 'r' ).readlines()
            # for line in lines:
            #     print(line)
            with open(f'{self.tmp_path}/{student}.policy.txt', 'a') as f:
                f.write(f'Recursion found!\n')
            break
            # print(cycle)

    def get_neighbors(self,student):
        neighbors = []
        if len(self.seating) > 0:
            srow = self.seating[self.seating['Roll No'] == student]
            if( len(srow) == 1 ):
                venue = str(srow.iloc[0]['Venue'])
                machine = int(srow.iloc[0]['Machine'])
                n1 = self.seating[(self.seating['Venue'] == venue) & (self.seating['Machine'] == machine+1)]
                n2 = self.seating[(self.seating['Venue'] == venue) & (self.seating['Machine'] == machine-1)]
                if len(n1) > 1:  print(f'Seating data has problem:\n {n1}')
                if len(n2) > 1:  print(f'Seating data has problem:\n {n2}')
                if len(n1) == 1: neighbors.append(n1.iloc[0]['Roll No'])
                if len(n2) == 1: neighbors.append(n2.iloc[0]['Roll No'])
            else:
                print(f'Seating data has problem:\n {srow}')
        return neighbors

    def get_fingerprint( self, f ):
        import fingerprint
        fprint = fingerprint.Fingerprint(kgram_len=15, window_len=20)
        #( kgram_len=4, window_len=5, base=10, modulo=1000)
        if not os.path.exists(f): return []
        sfile = open(f, "r").read()
        if len(sfile) < 80: return []
        sprint = fprint.generate(sfile)
        sprint, _ = zip(*sprint)
        return sprint
        
    def neighbor_check(self,student, my_plag_check = False):
        if not self.plag_check and not my_plag_check: return 
        neighbors = self.get_neighbors(student)
        # if not '0696' in student: return # print(neighbors)
        matched = set()
        if neighbors:
            for f in self.student_files:
                tprint = self.get_fingerprint( f'{self.template_path}/{f}' )
                sprint = self.get_fingerprint( f"{self.students_path}/{student}/{f}" )
                for neighbor in neighbors:
                    nprint = self.get_fingerprint( f"{self.students_path}/{neighbor}/{f}" )
                    match_count = 0
                    for fp in nprint:
                        if (fp in sprint) and not (fp in tprint):
                            match_count += 1
                    if match_count > 5:
                        matched.add(neighbor)
        if matched:
            print(f"Code matched with: {student},{matched}" )
            with open(f'{self.tmp_path}/{student}.policy.txt', 'a') as pfile:
                pfile.write(f"Code matched with:")
                for m in matched: pfile.write(f"{m},")
                pfile.write(f"\n")
                
        
    def policy_checks(self,student,plag_check=False):
        policy_file = f"{self.tmp_path}/{student}.policy.txt"
        os.system( f"rm -rf {policy_file}" )
        self.neighbor_check(student,plag_check)
        spath = self.students_path
        for f in self.student_files:
            sfile = f"{self.tmp_path}/{student}.{f}"
            if os.path.isfile(sfile):
                text = open( sfile, 'r' ).read()
                text = comment_remover(text)
                lines = text.split('\n')
                self.include_check(student, lines)
                self.disallowed_functions_check(student, lines)
                if not self.allowed_recursion:
                    self.recursion_check(student,sfile)
            else:
                print(f'Run compile first for {student}!')

    def check_policy(self,plag_check=False):
        for student in self.students:
            self.policy_checks(student,plag_check)

    def report_policy_error(self):
        errors = []
        for student in self.students:
            if os.path.isfile( f'{self.tmp_path}/{student}.policy.txt' ):
                err_txt = open(f'{self.tmp_path}/{student}.policy.txt', 'r').read()
                errors.append( (student, True, err_txt) )
                print('-------------------------')
                print( f'Policy error: {student}' )
                os.system( f'cat {self.tmp_path}/{student}.policy.txt' )
            else:
                errors.append( (student, False, "") )
                # errors[student] = False
        df = pd.DataFrame(errors)
        df = df.rename(columns = { 0: 'Roll No', 1: 'Policy Error', 2: 'Policy Error Reason' })
        return df

    #----------------------------------
    # Run
    #----------------------------------

    def run_test( self, student, test, timeout, std_interface=False ):
        # ------------------------------------
        #  Input output interface
        #    -- input folder where input file(s) are located 
        #    -- output file
        # ------------------------------------
        if std_interface:
            cmd = [f'{self.tmp_path}/{student}']
            print(" ".join(cmd), f"< '{self.tests_path}/{test}/input.txt'",f'> {self.tmp_path}/{student}.{test}.output.txt')
        else:
            cmd = [f'{self.tmp_path}/{student}',f"{self.tests_path}/{test}/input.txt",f'{self.tmp_path}/{student}.{test}.output.txt', f'2> /dev/null']
            print(" ".join(cmd))
        compile_ok = (self.check_compile_ok(student) or self.check_compile_with_extra_includes_ok(student))
        if os.path.isfile(f'{self.tmp_path}/{student}') and compile_ok:
            wrong = ""
            try:
                if os.path.isfile( f"{self.tests_path}/{test}/input.txt" ):
                    input_text = open(f"{self.tests_path}/{test}/input.txt", 'rb').read()
                else:
                    input_text = open(f"/dev/null", 'rb').read()
                start_time  = time.time()
                stdout_file = f'{self.tmp_path}/{student}.{test}.stdout.txt'
                stderr_file = f'{self.tmp_path}/{student}.{test}.stderr.txt'
                memory_file = f'{self.tmp_path}/{student}.{test}.memory.txt'
                run_cmd = cmd
                if self.memory_limit_mb is not None:
                    memory_bytes = self.memory_limit_mb * 1024 * 1024
                    run_cmd = [
                        '/usr/bin/time', '-f', '%M', '-o', memory_file,
                        '/usr/bin/timeout', '-s', 'KILL', str(timeout),
                        '/usr/bin/prlimit', f'--as={memory_bytes}', '--'
                    ] + cmd
                with open(stdout_file, 'w') as stdout_f, open(stderr_file, 'w') as stderr_f:
                    subprocess.run( run_cmd,
                                    input=input_text,
                                    stdout=stdout_f,
                                    stderr=stderr_f,
                                    timeout=timeout + 1,
                                    check=True )
                tt = time.time() - start_time
                with open(f'{self.tmp_path}/{student}.{test}.time.txt', 'a') as f:
                    f.write(str(tt)+'\n')
                if std_interface:
                    shutil.copy(stdout_file, f'{self.tmp_path}/{student}.{test}.output.txt')
            except subprocess.CalledProcessError as exc:
                if exc.returncode in (124, 137, -9):
                    os.system( f'touch {self.tmp_path}/{student}.timeout.txt')
                    wrong = "timeout"
                else:
                    stderr = ""
                    if os.path.isfile(stderr_file):
                        stderr = open(stderr_file, 'r', errors='replace').read()
                    if 'bad_alloc' in stderr or 'Cannot allocate memory' in stderr:
                        wrong = "memory-limit"
                    else:
                        wrong = "runtime-error"
            except subprocess.TimeoutExpired as exc:
                os.system( f'touch {self.tmp_path}/{student}.timeout.txt')
                wrong = "timeout"
            except Exception as e:
                wrong = f"{e}"
            if wrong:
                print(f'{student} {test} {wrong}')
                with open(f'{self.tmp_path}/{student}.{test}.error.txt', 'a') as f:
                    f.write(wrong+'\n')

    # TODO via shell
    def create_tasks( self, tasks ):
        f = open(f'{self.tmp_path}/tasks.txt', 'w')
        for (student,test,timeout) in tasks:
            if os.path.isfile(f'{self.tmp_path}/{student}') and self.check_compile_ok(student):
                cmd = [f'{self.tmp_path}/{student}',f'{self.tests_path}/{test}',f'{self.tmp_path}/{student}.{test}.output.txt', f'1> {self.tmp_path}/{student}.{test}.stdout.txt', f'2> {self.tmp_path}/{student}.{test}.stderr.txt']
                f.write(" ".join(cmd)+'\n')
        f.close()
         

    def run_tests(self, std_interface = False):
        tasks = []
        for test in self.tests:
            for student in self.students:   
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.stdout.txt' )
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.stderr.txt' )
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.output.txt' )
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.time.txt' )
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.error.txt' )
                os.system( f'rm -rf {self.tmp_path}/{student}.{test}.memory.txt' )
        for test in self.tests:
            for student in self.students:
                for i in range(0,self.test_repeat):                    
                    tasks.append( (student, test, self.timeout) )
        # --------------------------------
        # Randomize the order of execution (disabled for now)
        # --------------------------------
        if False: random.shuffle(tasks)
        # Via python concurrency
        def runner( task ):
            self.run_test( task[0], task[1], task[2], std_interface=std_interface )
        Parallel(n_jobs=self.parallel, backend='threading')( delayed(runner)( task ) for task in tasks )


    #----------------------------------
    # Run check
    #----------------------------------

    def report_runtime_error(self):
        all_errors = pd.DataFrame( columns = ['Roll No'] )
        for test in self.tests:
            errors = {}
            for student in self.students:
                if os.path.isfile( f'{self.tmp_path}/{student}.{test}.error.txt' ):
                    errors[student] = True
                    # print( f'Runtime error: {student} {test}' )
                else:
                    errors[student] = False                    
            df = pd.DataFrame(errors.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test })
            all_errors = pd.merge( all_errors, df, on= ['Roll No'], how='outer' )
        return all_errors

    def test_ok(self,student,test,expected):
        # assumption: if output.txt exists, there is no error.
        output   = f"{self.tmp_path}/{student}.{test}.output.txt"
        if self.cmp_function == None:
            expected = f"{self.tests_path}/{test}/{self.expected_output_file}"
            if not os.path.isfile(output): return False
            #--------------------
            # Simple file compare
            #--------------------
            if filecmp.cmp(output,expected): return True
            return False
        else:
            #------------------------------------------------------
            # custom compare function, May return any type of value
            #------------------------------------------------------
            if not os.path.isfile(output):
                output = None
            else:
                output = open( output, 'r', errors='replace' ).readlines()
                output = [o.rstrip() for o in output]
            result = self.cmp_function( expected, output )
            return result
    
    def test_match_count(self, student, test):
        output   = f"{self.tmp_path}/{student}.{test}.output.txt"
        expected = f"{self.tests_path}/{test}/{self.expected_output_file}"
        expected = open( expected, 'r' ).readlines()
        score = 0
        if os.path.isfile(output):
            output = open( output, 'r' ).readlines()
            for i,e_output in enumerate(expected):
                if i < len(output):
                    if( output[i] == e_output ):
                        score = score + 1
        return score
    
    
    def print_fail(self,student):
        for test in self.tests:
            output   = f"{self.tmp_path}/{student}.{test}.output.txt"
            expected = f"{self.tests_path}/{test}/{self.expected_output_file}"
            if os.path.isfile(output) and not filecmp.cmp(output,expected):
                os.system(f'diff {expected} {output} | head -10')

    def get_time(self,student,test):
        tfile   = f"{self.tmp_path}/{student}.{test}.time.txt"
        if os.path.isfile(tfile):
            times = [float(t) for t in open( tfile, 'r' ).readlines()]
            return statistics.mean(times)
        else:
            return float(self.timeout)

    def get_stdout(self,student,test):
        tfile   = f"{self.tmp_path}/{student}.{test}.stdout.txt"
        if os.path.isfile(tfile):
            stdout = open( tfile, 'r' ).read()
            return stdout
        else:
            return None
        
    def get_stderr(self,student,test):
        tfile   = f"{self.tmp_path}/{student}.{test}.stderr.txt"
        if os.path.isfile(tfile):
            stderr = open( tfile, 'r' ).read()
            return stderr
        else:
            return None

    def report_match_count(self):
        all_results = pd.DataFrame( columns = ['Roll No'] )
        for test in self.tests:
            counts = {}
            for student in self.students:
                counts[student] = self.test_match_count(student,test)
            df = pd.DataFrame(counts.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test })
            all_results = pd.merge( all_results, df, on= ['Roll No'], how='outer' )
        return all_results

    def report_result(self):
        all_results = pd.DataFrame( columns = ['Roll No'] )
        for test in self.tests:
            if os.path.exists( f"{self.tests_path}/{test}/{self.expected_output_file}" ):
                expected = open( f"{self.tests_path}/{test}/{self.expected_output_file}", 'r' ).readlines()
                expected = [o.rstrip() for o in expected]
            else:
                expected = None
            # -------------------------------
            # Collect responses
            # -------------------------------
            counts = {}
            all_keys = set()
            for student in self.students:
                result = self.test_ok(student,test,expected)
                if not isinstance(result, dict):
                    result = {'':result}
                for key in result: all_keys.add(key)
                counts[student] = result
            # -------------------------------
            # Turn responses into vectors
            # -------------------------------
            results = []
            for student in self.students:
                result = counts[student]
                tpl = [student]
                for key in all_keys:
                    if key in result:
                        tpl.append(result[key])
                    else:
                        tpl.append(None)
                results.append(tpl)
            df = pd.DataFrame(results)
            col_names = { 0: 'Roll No'}
            i = 1
            for key in all_keys:
                col_names[i] = test+key
                i = i + 1
            df = df.rename(columns = col_names)
            all_results = pd.merge( all_results, df, on= ['Roll No'], how='outer' )
        return all_results

    def report_time(self):
        all_timings = pd.DataFrame(columns = ['Roll No'])
        for test in self.tests:
            scores = {}
            for student in self.students:
                scores[student] = self.get_time(student,test)
            scores = dict(sorted(scores.items(), key=lambda item: item[1]))
            df = pd.DataFrame(scores.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test+'-time' })
            all_timings = pd.merge( all_timings, df, on= ['Roll No'], how='outer' )
        return all_timings

    def get_memory(self,student,test):
        mfile = f"{self.tmp_path}/{student}.{test}.memory.txt"
        if os.path.isfile(mfile):
            values = [int(m) for m in open(mfile, 'r').readlines() if m.strip().isdigit()]
            if values:
                return max(values)
        return None

    def report_memory(self):
        all_memory = pd.DataFrame(columns = ['Roll No'])
        for test in self.tests:
            usage = {}
            for student in self.students:
                usage[student] = self.get_memory(student,test)
            df = pd.DataFrame(usage.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test+'-memory-kb' })
            all_memory = pd.merge( all_memory, df, on= ['Roll No'], how='outer' )
        return all_memory

    def report_stdouts(self):
        all_results = pd.DataFrame( columns = ['Roll No'] )
        for test in self.tests:
            stdouts = {}
            for student in self.students:
                stdouts[student] = self.get_stdout(student,test)
            df = pd.DataFrame(stdouts.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test+'-stdout' })
            all_results = pd.merge( all_results, df, on= ['Roll No'], how='outer' )
        return all_results

    def report_stderrs(self):
        all_results = pd.DataFrame( columns = ['Roll No'] )
        for test in self.tests:
            stderrs = {}
            for student in self.students:
                stderrs[student] = self.get_stderr(student,test)
            df = pd.DataFrame(stderrs.items())
            df = df.rename(columns = { 0: 'Roll No', 1: test+'-stderr' })
            all_results = pd.merge( all_results, df, on= ['Roll No'], how='outer' )
        return all_results

    def report_all(self, include_stdouts=False, include_stderrs=True):
        ce      = self.report_compile_error()
        pe      = self.report_policy_error()
        base    = self.report_result ()
        time    = self.report_time ()
        full = pd.merge(   ce, pe,      on=['Roll No'], how='outer' )
        full = pd.merge( full, base,    on=['Roll No'], how='outer' )
        full = pd.merge( full, time,    on=['Roll No'], how='outer' )
        if self.memory_limit_mb is not None:
            memory = self.report_memory()
            full = pd.merge( full, memory, on=['Roll No'], how='outer' )
        if include_stdouts:
            stdouts = self.report_stdouts ()
            full = pd.merge( full, stdouts, on=['Roll No'], how='outer' )
        if include_stderrs:
            stderrs = self.report_stderrs ()
            full = pd.merge( full, stderrs, on=['Roll No'], how='outer' )
        full.to_csv(f'{self.tmp_path}/results.csv',index=False,escapechar='\\')

    def detect_redundant_tests(self):
        full =  self.get_results()
        # full['total'] = full[self.tests].sum( axis = 1 )
        # full = full[full['total'] > 40]
        groups = defaultdict(list)
        for test in self.tests: groups[tuple(full[test])].append(test)
        identical_tests = [v for v in groups.values()]
        print(identical_tests)
        # ----------------------------
        #
        # ----------------------------
        print('Implied testcases:')
        for test2 in self.tests:
            for test1 in self.tests:
                if test1 == test2 : continue
                already_equal = False
                for s in identical_tests:
                    if test1 in s and test2 in s:
                        already_equal = True
                if already_equal : continue
                # violations = full[(full[test1] == True) & (full[test2] == False)]
                violations = full[ full[test1] > full[test2] ]
                if violations.empty:
                    print( f'{test1} -> {test2}' )
                else:
                    pass
                    # print(violations[['Roll No',test1,test2]])
                    # exit()
                

    def create_student_package(self,student):
        folder = f'{self.tmp_path}/{student}-grader'
        os.system( f'rm -rf {folder}' )
        os.system(f'mkdir -p {folder}')
        os.system(f'mkdir -p {folder}/tests')
        os.system(f'mkdir -p {folder}/students/{student}')
        os.system(f'cp -r {self.prob_path} {folder}/harness')
        os.system(f'cp -r {self.template_path} {folder}/template')
        for src in self.student_files:
            student_src = f'{self.students_path}/{student}/{src}'
            if os.path.exists(student_src):
                os.system(f'cp -r {student_src} {folder}/students/{student}/')
        # Create attendance.csv for the student
        if not self.skip_attendance_while_packaging:
            attendance_file = f'{self.students_path}/attendance.csv'
            if not os.path.exists(attendance_file):
                raise FileNotFoundError(f'Attendance file not found: {attendance_file}')
            
            with open(attendance_file, 'r') as f:
                lines = f.readlines()
            
            if len(lines) == 0:
                raise ValueError(f'Attendance file is empty: {attendance_file}')
            
            header = lines[0]
            student_line = None
            for line in lines[1:]:
                # Case insensitive match for roll number (first column)
                roll = line.split(',')[0].strip()
                if roll.lower() == student.lower():
                    student_line = line
                    break
            
            if not student_line:
                raise ValueError(f'Student {student} not found in attendance file: {attendance_file}')
            
            output_attendance = f'{folder}/students/attendance.csv'
            with open(output_attendance, 'w') as f:
                f.write(header)
                f.write(student_line)
        
        for t in self.tests:
            os.system(f'cp -r {self.tests_path}/{t} {folder}/tests/')
        is_mac = platform.system() == "Darwin"
        for f in self.grading_files:
            fname = os.path.basename(f)
            os.system(f'cp {f} {folder}/')
            if fname.endswith('.pkl'): continue
            if is_mac:
                os.system(f"sed -i '' 's|.*{self.package_replace_sequence}.*|students_path = \"./students/\"\\nprob_path = \"./harness/\"\\ntests_path = \"./tests/\"\\n|g' {folder}/{fname}")
            else:
                os.system(f"sed -i 's|.*{self.package_replace_sequence}.*|students_path = \"./students/\"\\nprob_path = \"./harness/\"\\ntests_path = \"./tests/\"\\n|g' {folder}/{fname}")
        zip_folder(folder, folder+'.zip')
        print(folder+'.zip')
            
    def create_packages(self):
        Parallel(n_jobs=self.parallel)( delayed(self.create_student_package)( s ) for s in self.students )

    def send_emails(self,delay=10):
        import bulk_email
        send = False
        delay = 1
        for student in self.students:
            # email is already sent
            if os.path.exists(f"{self.tmp_path}/{student}-email-sent"): continue
            file_path =  f"{self.tmp_path}/{student}-grader.zip"
            student_email = student + '@iitb.ac.in'
            if os.path.exists(file_path):
                print(f"Sending email to {student_email} for roll number: {student}")
                try_sending_email = True
                while try_sending_email:
                    success = bulk_email.send_email( student, student_email, self.exam, self.problem, file_path)
                    try_sending_email = not success # try again if email fails
                    if try_sending_email : delay += 2
                    time.sleep(delay)
                os.system( f"touch {self.tmp_path}/{student}-email-sent" )
            else:
                print(f"File not found for roll number: {student}")
            
    def move_tests(self,test_store, prefix):
        inputs = test_store+'/inputs'
        outputs = test_store+'/outputs'
        onlyfiles = [f for f in os.listdir(inputs) if os.path.isfile(os.path.join(inputs, f))]
        i = 0
        for test in onlyfiles:
            os.system( f'mkdir -p ./tests/{prefix}{i}')
            os.system( f'cp {inputs}/{test} ./tests/{prefix}{i}/input.txt' )
            os.system( f'cp {outputs}/{test} ./tests/{prefix}{i}/{self.expected_output_file}' )
            i = i + 1
            
    def find_student(self, rollno):
        collected = []
        for s in self.students:
            if rollno in s:
                collected.append(s)
        if len(collected) != 1:
            print(len(collected), 'students mathed with the roll no')
            if( len(collected) < 5):
                print('The matched students:',collected)
            exit()
        return collected[0]

    def set_student(self, rollno):
        rollno = self.find_student(rollno)
        print('Running for student:', rollno)
        for f in self.student_files:
            f = f'{self.students_path}/{rollno}/{f}'
            if os.path.isfile(f):
                os.system( f'cat {f}' )
                pass
        self.students = [rollno]

    def copy_student_code(self):
        if len(self.students) > 1:
            print('Cannot copy for the multiple students!')
        rollno = self.students[0]
        for f in self.student_files:
            src = f'{self.students_path}/{rollno}/{f}'
            dst = f'{self.students_path}/{rollno}/{f}.submitted'
            # ---------------------------
            # Block repeated copy       #
            # ---------------------------
            if not os.path.exists(dst):
                os.system(f'cp -r {src} {dst}')
                print(f'Creating {dst}')
                print(f'Now you can edit {src}')
        
        
    def save_marks(self,full,col,other_problems=[],dump_table = True, merge_type = 'append', comment = "", othercols = []):
        full = full[['Roll No','Compile Success','Policy Error','Policy Error Reason']+self.tests+[col] + othercols].copy()
        #--------------------------------------------
        # Any complilation/policy error leads to zero 
        #--------------------------------------------
        full.loc[full['Compile Success'] == "PASS-WITH-INCLUDES", col] = 0
        full.loc[full['Policy Error'] == True, col] = 0
        # -------------------------------------
        # Print results table
        # -------------------------------------
        if(dump_table): print(full)
        # -------------------------------------
        # Don't save in cribs!
        # -------------------------------------
        if len(self.students) == 1: return
        # -------------------------------------
        # Save
        # -------------------------------------        
        full.to_csv( 'grade.csv', index=False )

        full['Comment'] = comment
        # Append error reason to comment if comment exists
        full['Comment'] = full.apply(lambda x: f"{x['Comment']} {x['Policy Error Reason'].strip()}" if not pd.isna(x['Policy Error Reason']) else x['Comment'], axis=1)
                
        # -------------------------------------
        # Save marks file, which to be uploaded
        # -------------------------------------
        marks_file = "marks.csv"
        marks = full[['Roll No', col,'Comment']].copy()
        marks.columns = ['Roll No', 'Q1','Comment1']
        
        marks.to_csv(marks_file, index=False)
        print(f'Dumping {marks_file}')
        
        # marks['Comment1'] = comment
        # marks['Comment1'] += marks['Policy Error Reason']

        # -----------------------------------------
        # merge with the other problems of the exam
        # -----------------------------------------
        if other_problems != []:
            if merge_type == 'append':
                other_marks = [marks]
                for prob in other_problems:
                    if os.path.exists(f'{prob}/{marks_file}'):
                        df = pd.read_csv( f'{prob}/{marks_file}' )
                        other_marks.append(df)
                    else:
                        print(f'Problem at path {prob} is not found.')
                collected = pd.concat(other_marks)
                # self.print_distribution( collected, 'Q1')
                collected.to_csv('collected.csv', index=False)
                print(f'Dumping collected.csv')
            elif merge_type == 'merge':
                marks.columns = ['Roll No', f'Q{len(other_problems)+1}', f'Comment{len(other_problems)+1}']
                i = 1
                for prob in other_problems:
                    if os.path.exists(f'{prob}/{marks_file}'):
                        df = pd.read_csv( f'{prob}/{marks_file}' )
                        df.columns = ['Roll No', f'Q{i}', f'Comment{i}']
                        marks = pd.merge( marks, df, on= ['Roll No'], how='outer' )
                    else:
                        print(f'Problem at path {prob} is not found.')
                    i += 1
                marks.to_csv('collected.csv', index=False)
                print(f'Dumping collected.csv')
            else:
                assert(False) # Should never occur
        self.view_distribution()

    def view_distribution(self):
        if not os.path.exists(f'marks.csv'):
            print('marks.csv is missing!')
        marks = pd.read_csv( f'marks.csv' )
            
        # -------------------------------------
        # Print marks distribution
        # -------------------------------------
        print('Problem distribution')
        self.print_distribution( marks, 'Q1')
        if not os.path.exists(f'collected.csv'):
            return
        exam = pd.read_csv( f'collected.csv' )
        print('Exam distribution')
        self.print_distribution( exam, 'Q1')
        
    
    def get_results(self):
        if os.path.exists(f'{self.tmp_path}/results.csv'):
            return pd.read_csv( f'{self.tmp_path}/results.csv' )
        elif os.path.exists(f'./grade.csv'):
            return pd.read_csv( f'./grade.csv' )
        else:
            print('Results found! Run all')
            exit()
            
        
    def print_locations(self):
        if len(self.students) != 1: return
        rollno = self.students[0]
        fs =f"""
        --------------------------------------------------------------
        Output of the runs are being saved in the following locations:
        Your submission: ./students/{rollno}/<problem-files>
        Compiler stdout: {self.tmp_path}/{rollno}.compile.txt
        Policy output: {self.tmp_path}/{rollno}.policy.txt        
        Binary: {self.tmp_path}/{rollno}
        Output: {self.tmp_path}/{rollno}.<test>.output.txt        
        Time taken: {self.tmp_path}/{rollno}.<test>.time.txt        
        Runtime error: {self.tmp_path}/{rollno}.<test>.error.txt        
        Collated results: {self.tmp_path}/results.csv
        --------------------------------------------------------------        
        """
        print(fs)
        return
    def action( self, act ):
        if act in ['locations','all'] : self.print_locations()
        if act in ['compile'  ,'all'] : self.compile_all () # verbose = True
        if act in ['policy'   ,'all'] : self.check_policy(plag_check=False)
        if act in ['run'      ,'all'] : self.run_tests(std_interface = True)
        if act in ['results'  ,'all'] : self.report_all()
        if act in ['plag'           ] : self.check_policy(plag_check=True)
        if act in ['view'           ] : self.view_distribution()
        if act in ['email'          ] : self.send_emails()
        if act in ['redundant'      ] : self.detect_redundant_tests()
        if act in ['copy'           ] : self.copy_student_code()

    def display_usage( self, script_name ):
        print(f'Usage {script_name} ' + '{action} [rollno]')
        print('Actions: all, policy, compile, run, results, grade, redundant, email')
        print('Roll no is optional')
        exit()
