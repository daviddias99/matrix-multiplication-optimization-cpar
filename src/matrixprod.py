import sys
import time
import numpy as np

def mult_matrix_1(mat_size, mat_a, mat_b, _):
  mat_c = [0]*(mat_size**2)

  start = time.process_time()

  for i in range(mat_size):
    for j in range(mat_size):
      temp = 0

      for k in range(mat_size):
        temp += mat_a[i * mat_size + k] * mat_b[k * mat_size + j]

      mat_c[i * mat_size + j] = temp

  end = time.process_time()

  return end - start, mat_c

def mult_matrix_2(mat_size, mat_a, mat_b, _):
  mat_c = [0]*(mat_size**2)

  start = time.process_time()

  for i in range(mat_size):
    for k in range(mat_size):
      for j in range(mat_size):
        mat_c[i*mat_size + j] += mat_a[i * mat_size + k] * mat_b[k * mat_size + j]

  end = time.process_time()

  return end - start, mat_c

def mult_matrix_3(mat_size, mat_a, mat_b, block_size):
  mat_c = [0]*(mat_size**2)

  start = time.process_time()

  for jj in range(0, mat_size, block_size):
    for kk in range(0, mat_size, block_size):
      for i in range(mat_size):
        for j in range(jj, jj + block_size):
          temp = 0

          for k in range(kk, kk + block_size):
            temp += mat_a[i * mat_size + k] * mat_b[k * mat_size + j]

          mat_c[i * mat_size + j] += temp

  end = time.process_time()

  return end - start, mat_c

def mult_matrix_4(mat_size, mat_a, mat_b, block_size):
  mat_c = [0]*(mat_size**2)

  start = time.process_time()

  for jj in range(0, mat_size, block_size):
    for kk in range(0, mat_size, block_size):
      for i in range(mat_size):
        for k in range(kk, kk + block_size):
          for j in range(jj, jj + block_size):
            mat_c[i*mat_size + j] += mat_a[i * mat_size + k] * mat_b[k * mat_size + j]

  end = time.process_time()

  return end - start, mat_c


functions = [mult_matrix_1, mult_matrix_2, mult_matrix_3, mult_matrix_4]

def main(args):

  mat_size = int(args[1])
  n_runs = int(args[2])
  alg_idx = int(args[3]) - 1
  block_size = int(args[4]) if len(args) > 4 else mat_size

  mat_a = [[1] * mat_size] * mat_size
  mat_b = [[i + 1 for i in range(mat_size)] for j in range(mat_size)]

  for _ in range(n_runs):
    mult_matrix = functions[alg_idx]
    elapsed_time, _ = mult_matrix(mat_size, mat_a, mat_b, block_size)
    print(f'{elapsed_time}')

if __name__ == '__main__':
  main(sys.argv)
