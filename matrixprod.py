import time

n = 50

mat_a = [1.0] * n * n
mat_b = [i + 1.0 for i in range(n) for j in range(n)]
mat_c = []

start = time.process_time()

for i in range(n):
  for j in range(n):
    temp = 0

    for k in range(n):
      temp += mat_a[i * n + k] * mat_b[k * n + j]

    mat_c += [temp]

end = time.process_time()

print(f'{end - start}s')

