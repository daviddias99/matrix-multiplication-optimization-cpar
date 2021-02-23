import sys
import time

def mult_matrix_1(mat_size, mat_a, mat_b):
  mat_c = []

  start = time.process_time()

  for i in range(mat_size):
    for j in range(mat_size):
      temp = 0

      for k in range(mat_size):
        temp += mat_a[i * mat_size + k] * mat_b[k * mat_size + j]

      mat_c += [temp]

  end = time.process_time()

  return end - start, mat_c


functions = [mult_matrix_1]

def main(args):
  alg_idx = 0
  mat_size = 0
  n_runs = 1

  if len(args) - 1 >= 2:
    alg_idx = int(args[1]) - 1
    mat_size = int(args[2])
  if len(args) - 1 == 3:
    n_runs = int(args[3])
  else:
    print('Invalid number of args', file=sys.stderr)
    exit(-1)

  mat_a = [1.0] * mat_size * mat_size
  mat_b = [i + 1.0 for i in range(mat_size) for j in range(mat_size)]

  for _ in range(n_runs):
    mult_matrix = functions[alg_idx]

    elapsed_time, _ = mult_matrix(mat_size, mat_a, mat_b)

    print(f'{elapsed_time}')


if __name__ == '__main__':
  main(sys.argv)
