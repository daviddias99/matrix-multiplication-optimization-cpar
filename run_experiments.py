import os
import subprocess

import pandas as pd

os.makedirs('out', exist_ok=True)

out_path = os.path.join(os.path.dirname(__file__), 'out')
src_path = os.path.join(os.path.dirname(__file__), 'src')


cpp_src = os.path.join(src_path, 'matrixprod.cpp')
cpp_out = os.path.join(out_path, 'matrixprod_cpp')

process = subprocess.run(['g++', '-O2', cpp_src, '-o', cpp_out, '-lpapi'])


process = subprocess.run([cpp_out, '1', '100', '15'],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True)

df = pd.DataFrame(
    [row.split(',') for row in process.stdout.split('\n') if row.strip() != ''],
    columns=['Time', 'L1 DCM', 'L2 DCM'])

print(df)
