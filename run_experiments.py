from datetime import datetime
import os
import subprocess
import pandas as pd
from datetime import datetime
import json

os.makedirs('out', exist_ok=True)
os.makedirs('results', exist_ok=True)

dir = os.path.dirname(__file__)
src_path = os.path.join(dir, 'src')
out_path = os.path.join(dir, 'out')
results_path = os.path.join(dir, 'results')
experiments_path = os.path.join(dir, 'experiments.json')

cpp_path = {
    'src': os.path.join(src_path, 'matrixprod.cpp'),
    'out': os.path.join(out_path, 'matrixprod_cpp')
}
python_path = os.path.join(src_path, 'matrixprod.py')
java_path = os.path.join(src_path, 'MatrixProd.java')

run_cmd = {
    'cpp': ['taskset', '-c', '0', cpp_path['out']],
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

def run_experiment(exp):

    results = []

    for lang in exp['languages']:
        print('\nRunning experiment for language {} ...'.format(lang))

        for alg in exp['algorithms']:
            print('\tAlgorithm {} ...'.format(alg))

            for size in range(exp['start'], exp['end']+exp['step'], exp['step']):
                print('\t\tMatrix size {} ...'.format(size))

                output = run(lang, alg, size, exp['runs'])
                parsed_output = parse_output(output, [lang, alg, size])
                results += parsed_output

    return pd.DataFrame(results, columns=['Language', 'Algorithm', 'Matrix Size', 'Time', 'L1 DCM', 'L2 DCM'])



compile_cpp()
compile_java()


with open(experiments_path) as f:
    experiments = json.load(f)

for exp in experiments:
    print('Starting experiment {} ({} runs) ...'.format(exp['name'], exp['runs']))

    res = run_experiment(exp)

    res_path = os.path.join(results_path, 'exp_{}_{}.csv'.format(exp['name'], datetime.now()))
    res.to_csv(res_path, index=False, header=True)

    print('Exp. {} results saved in a file ...\n'.format(exp['name']))
