from datetime import datetime
import os
import subprocess
import pandas as pd
from datetime import datetime

os.makedirs('out', exist_ok=True)
os.makedirs('results', exist_ok=True)

src_path = os.path.join(os.path.dirname(__file__), 'src')
out_path = os.path.join(os.path.dirname(__file__), 'out')
results_path = os.path.join(os.path.dirname(__file__), 'results')

cpp_path = {
    'src': os.path.join(src_path, 'matrixprod.cpp'),
    'out': os.path.join(out_path, 'matrixprod_cpp')
}
python_path = os.path.join(src_path, 'matrixprod.py')
java_path = os.path.join(src_path, 'MatrixProd.java')

run_cmd = {
    'cpp': [cpp_path['out']],
    'py': ['python', python_path],
    'java': ['java', '-cp', out_path, 'MatrixProd']
}


def compile_cpp():
    subprocess.run(['g++', '-O2', cpp_path['src'], '-o', cpp_path['out'], '-lpapi'])

def compile_java():
    subprocess.run(['javac', java_path, '-d', 'out'])

def run(language, algorithm, size, n_runs):
    if not isinstance(algorithm, list):
        algorithm = [algorithm]

    alg = [str(arg) for arg in algorithm]

    process = subprocess.run(run_cmd[language] + [str(size), str(n_runs)] + alg,
                             stdout=subprocess.PIPE,
                             stderr=subprocess.PIPE,
                             universal_newlines=True)

    return process.stdout

def parse_output(output, other_data):
    return [other_data + row.split(',') for row in output.split('\n') if row.strip() != '']

def run_experiments(start, end, step, n_runs, algorithms, languages):

    results = []

    for alg in algorithms:
        for lang in languages:
            for size in range(start, end+step, step):
                output = run(lang, alg, size, n_runs)
                parsed_output = parse_output(output, [lang, alg, size])
                results += parsed_output

    return pd.DataFrame(results, columns=['Language', 'Algorithm', 'Matrix Size', 'Time', 'L1 DCM', 'L2 DCM'])



compile_cpp()
compile_java()

n_runs = 3

exp_1_results = run_experiments(600, 3000, 400, n_runs, [1], ['cpp', 'py', 'java'])
exp_1_results.to_csv(os.path.join(results_path, 'exp_1_{}.csv'.format(datetime.now())), index=False, header=True)

exp_2a_results = run_experiments(600, 3000, 400, n_runs, [2], ['cpp', 'py', 'java'])
exp_2a_results.to_csv(os.path.join(results_path, 'exp_2a_{}.csv'.format(
    datetime.now())), index=False, header=True)

exp_2b_results = run_experiments(4096, 10240, 2048, n_runs, [2], ['cpp'])
exp_2b_results.to_csv(os.path.join(results_path, 'exp_2b_{}.csv'.format(
    datetime.now())), index=False, header=True)

exp_3_results = run_experiments(4096, 10240, 2048, n_runs, [[4, 128], [4, 256], [4, 512]], ['cpp'])
exp_3_results.to_csv(os.path.join(results_path, 'exp_3_{}.csv'.format(
    datetime.now())), index=False, header=True)
