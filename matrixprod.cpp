//#include <omp.h>
#include <papi.h>
#include <stdio.h>
#include <time.h>

#include <cstdlib>
#include <iomanip>
#include <iostream>

using namespace std;

#define SYSTEMTIME clock_t

double simpleCycle(double* op1Matrix, double* op2Matrix, double* resMatrix, int matrixSize) {
  SYSTEMTIME Time1, Time2;
  double temp;
  Time1 = clock();

  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      temp = 0;
      for (int k = 0; k < matrixSize; k++) {
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
  double temp;
  Time1 = clock();

  for (int i = 0; i < matrixSize; i++) {
    for (int k = 0; k < matrixSize; k++) {
      temp = 0;
      for (int j = 0; j < matrixSize; j++) {
        temp += op1Matrix[i * matrixSize + k] * op2Matrix[k * matrixSize + j];
      }
      resMatrix[i * matrixSize + k] = temp;
    }
  }

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

/* Usage: matrixprod <operation> <square matrix size> [runs]*/
int main(int argc, char *argv[]) {
  if (argc < 3) {
    cerr << "ERROR: insufficient number of arguments (operation, square matrix size, runs)" << endl;
    return -1;
  }

  // Get arguments
  int op = atoi(argv[1]);
  int matrixSize = atoi(argv[2]);
  int runs = argc == 4 ? atoi(argv[3]) : 1;

  // init matrices
  double * op1Matrix = (double *)malloc((matrixSize * matrixSize) * sizeof(double));
  double * op2Matrix = (double *)malloc((matrixSize * matrixSize) * sizeof(double));
  double * resMatrix = (double *)malloc((matrixSize * matrixSize) * sizeof(double));

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
    }

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