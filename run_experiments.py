import os
import subprocess

cpp_file = os.path.join(os.path.dirname(__file__), 'matrixprod_cpp')

process = subprocess.run(['g++', '-O2', 'matrixprod.cpp', '-o', cpp_file, '-lpapi'])

process = subprocess.run([cpp_file],
                         stdout=subprocess.PIPE,
                         stderr=subprocess.PIPE,
                         universal_newlines=True)

print(process)
