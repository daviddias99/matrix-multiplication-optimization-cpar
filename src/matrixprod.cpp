//#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <time.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string.h>

// TODO: verify the same time is being used for all programming languages

using namespace std;

#define SYSTEMTIME clock_t

double simpleCycle(double* op1Matrix, double* op2Matrix, double* resMatrix, int matrixSize) {
  SYSTEMTIME Time1, Time2;
  double temp;
  int i, j, k;
  Time1 = clock();

  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      temp = 0;
      for (k = 0; k < matrixSize; k++) {
        temp += op1Matrix[i * matrixSize + k] * op2Matrix[k * matrixSize + j];
      }
      resMatrix[i * matrixSize + j] = temp;
    }
  }

  Time2 = clock();

  return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

double optimCycle(double* op1Matrix, double* op2Matrix, double* resMatrix, int matrixSize) {
  SYSTEMTIME Time1, Time2;
  int i, j, k;

  Time1 = clock();

  for (i = 0; i < matrixSize; i++) {
    for (k = 0; k < matrixSize; k++) {
      for (j = 0; j < matrixSize; j++) {
        resMatrix[i * matrixSize + j] += op1Matrix[i * matrixSize + k] * op2Matrix[k * matrixSize + j];
      }
    }
  }

  Time2 = clock();

  return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

double blockSimpleCycle(double* op1Matrix, double* op2Matrix, double* resMatrix,
                  int matrixSize, int blockSize) {
  double temp;
  SYSTEMTIME Time1, Time2;
  int jj, kk, i, j, k;
  Time1 = clock();

  for (jj = 0; jj < matrixSize; jj = jj + blockSize)
    for (kk = 0; kk < matrixSize; kk = kk + blockSize)
      for (i = 0; i < matrixSize; i = i + 1)
        for (j = jj; j < jj + blockSize; j = j + 1) {
          temp = 0;
          for (k = kk; k < kk + blockSize; k = k + 1)
            temp = temp + op1Matrix[i * matrixSize + k] * op2Matrix[k * matrixSize + j];

          resMatrix[i*matrixSize + j] = resMatrix[i*matrixSize + j] + temp;
        };

  Time2 = clock();

  return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

double blockOptimCycle(double* op1Matrix, double* op2Matrix, double* resMatrix,
                  int matrixSize, int blockSize) {

  SYSTEMTIME Time1, Time2;
  int jj, kk, i, j, k;
  Time1 = clock();

  for (jj = 0; jj < matrixSize; jj = jj + blockSize)
    for (kk = 0; kk < matrixSize; kk = kk + blockSize)
      for (i = 0; i < matrixSize; i = i + 1)
        for (k = kk; k < kk + blockSize; k = k + 1) {
          for (j = jj; j < jj + blockSize; j = j + 1)
            resMatrix[i * matrixSize + j] += op1Matrix[i * matrixSize + k] * op2Matrix[k * matrixSize + j];

        };

  Time2 = clock();

  return (double)(Time2 - Time1) / CLOCKS_PER_SEC;
}

void handleError(int retval) {
  fprintf(stderr, "PAPI error %d: %s\n", retval, PAPI_strerror(retval));
  exit(1);
}

void initPapi(int &eventSet) {
  int ret;

  ret = PAPI_library_init(PAPI_VER_CURRENT);
  if (ret != PAPI_VER_CURRENT) handleError(ret);

  ret = PAPI_create_eventset(&eventSet);
  if (ret != PAPI_OK) handleError(ret);

  ret = PAPI_add_event(eventSet, PAPI_L1_DCM);
  if (ret != PAPI_OK) handleError(ret);

  ret = PAPI_add_event(eventSet, PAPI_L2_DCM);
  if (ret != PAPI_OK) handleError(ret);
}

void closePapi(int &eventSet) {
  int ret;

  ret = PAPI_remove_event(eventSet, PAPI_L1_DCM);
  if (ret != PAPI_OK) handleError(ret);

  ret = PAPI_remove_event(eventSet, PAPI_L2_DCM);
  if (ret != PAPI_OK) handleError(ret);

  ret = PAPI_destroy_eventset(&eventSet);
  if (ret != PAPI_OK) handleError(ret);
}

/* Usage: matrixprod <operation> <square matrix size> <runs> [block size]*/
int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "ERROR: insufficient number of arguments (square matrix size, runs, operation)" << endl;
    return -1;
  }

  // Get arguments
  int matrixSize = atoi(argv[1]);
  int runs = atoi(argv[2]);
  int op = atoi(argv[3]);
  int blockSize = argc == 5 ? atoi(argv[4]) : matrixSize*matrixSize;

  const int MATRIX_SIZE_BYTES = (matrixSize * matrixSize) * sizeof(double);

  // init matrices
  double * op1Matrix = (double *)malloc(MATRIX_SIZE_BYTES);
  double * op2Matrix = (double *)malloc(MATRIX_SIZE_BYTES);
  double * resMatrix = (double *)malloc(MATRIX_SIZE_BYTES);

  memset(resMatrix, 0, matrixSize * matrixSize * sizeof(double));

  for (int i = 0; i < matrixSize; i++){
    for (int j = 0; j < matrixSize; j++){
      op1Matrix[i * matrixSize + j] = (double)1.0;
      op2Matrix[i * matrixSize + j] = (double)(i + 1);
    }
  }

  // Init PAPI
  int EventSet = PAPI_NULL;
  long long values[2];

  initPapi(EventSet);

  for(int i = 0; i < runs; i++) {

    // Start counting
    int ret = PAPI_start(EventSet);
    double algorithmTime;
    if (ret != PAPI_OK) handleError(ret);

    switch (op) {
      case 1:
        algorithmTime = simpleCycle(op1Matrix, op2Matrix, resMatrix, matrixSize);
        break;
      case 2:
        algorithmTime = optimCycle(op1Matrix, op2Matrix, resMatrix, matrixSize);
        break;
      case 3:
        algorithmTime = blockSimpleCycle(op1Matrix, op2Matrix, resMatrix, matrixSize, blockSize);
        break;
      case 4:
        algorithmTime = blockOptimCycle(op1Matrix, op2Matrix, resMatrix, matrixSize, blockSize);
        break;
    }
    
    for (size_t i = 0; i < matrixSize*matrixSize; i++)
    {
      cout << resMatrix[i];
    }
    

    memset(resMatrix, 0, MATRIX_SIZE_BYTES);

    ret = PAPI_stop(EventSet, values);
    if (ret != PAPI_OK) handleError(ret);

    long long l1DataCacheMisses = values[0];
    long long l2DataCacheMisses = values[1];

    cout << algorithmTime << ',' << l1DataCacheMisses << ',' << l2DataCacheMisses << endl;

    ret = PAPI_reset(EventSet);
    if (ret != PAPI_OK) handleError(ret);
  }

  free(op1Matrix);
  free(op2Matrix);
  free(resMatrix);
  closePapi(EventSet);
}