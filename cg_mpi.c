#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>

#define ITERMAX 100
#define ERROR 0.00000001

void createPosDefMatrix(int n, double *matA, double *arrX, double *arrB);
double scaleArray(int n, double scalar, double *arr, double *result);
double dotProd(int n, double *arrA, double *arrB);
void addArrays(int n, double *arrA, double *arrB, double *result);
void subArrays(int n, double *arrA, double *arrB, double *result);
void arrayMultiplication(int n, double *arrA, double *arrB, double *result);

int main(int argc, char **argv)
{

  int id, np, N;
  int iter = 0;
  double *matA, *arrX, *arrB, *arrAux, *arrOpp, *arrR, *arrD, *arrQ, *arrProd, *arrResCG,
      sigma, oldSigma, newSigma, initialSigma, alpha, beta, start, end;

  if (argc != 2)
  {
    printf("usage: %s <matrix_order>\n", argv[0]);
    exit(0);
  }

  N = atoi(argv[1]);

  MPI_Status s;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  matA = (double *)malloc(N * N * sizeof(double));
  arrX = (double *)malloc(N * sizeof(double));
  arrB = (double *)malloc(N * sizeof(double));
  arrAux = (double *)malloc(N * sizeof(double));
  arrR = (double *)malloc(N * sizeof(double));
  arrD = (double *)malloc(N * sizeof(double));
  arrQ = (double *)malloc(N * sizeof(double));
  arrOpp = (double *)malloc(N * sizeof(double));
  arrProd = (double *)malloc(N * sizeof(double));
  arrResCG = (double *)malloc(N * sizeof(double));

  createPosDefMatrix(N, matA, arrX, arrB);

  MPI_Bcast(arrX, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Scatter(matA, N / np * N, MPI_DOUBLE, arrOpp, N / np * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  /* ------------------------- MATRIX MULTIPLICATION ------------------------- */

  for (int i = 0; i < N / np; i++)
  {
    arrProd[i] = 0;
    for (int j = 0; j < N; j++)
    {
      arrProd[i] += arrOpp[i * N + j] * arrX[j];
    }
  }

  MPI_Allgather(arrProd, N / np, MPI_DOUBLE, arrAux, N / np, MPI_DOUBLE, MPI_COMM_WORLD);

  subArrays(N, arrB, arrAux, arrR);
  memcpy(arrD, arrR, N * sizeof(double));

  newSigma = dotProd(N, arrR, arrR);
  sigma = newSigma;

  while (iter < ITERMAX && newSigma > ERROR)
  {

    MPI_Scatter(matA, N / np * N, MPI_DOUBLE, arrOpp, N / np * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (iter < 2 && id == 0)
    {
      for (int i = 0; i < N / np; i++)
        printf("[%d] Array Opp: %.2f\n", id, arrOpp[i]);
    }

    for (int i = 0; i < N / np; i++)
    {
      arrProd[i] = 0;
      for (int j = 0; j < N; j++)
      {
        arrProd[i] += arrOpp[i * N + j] * arrD[j];
      }
    }

    // MPI_Gather(arrProd, N / np, MPI_DOUBLE, arrQ, N / np, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // MPI_Bcast(arrQ, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Allgather(arrProd, N / np, MPI_DOUBLE, arrQ, N / np, MPI_DOUBLE, MPI_COMM_WORLD);

    alpha = newSigma / dotProd(N, arrD, arrQ);

    scaleArray(N, alpha, arrD, arrAux);
    addArrays(N, arrX, arrAux, arrX);
    scaleArray(N, alpha, arrQ, arrAux);
    subArrays(N, arrR, arrAux, arrR);

    oldSigma = newSigma;
    newSigma = dotProd(N, arrR, arrR);

    beta = newSigma / oldSigma;
    scaleArray(N, beta, arrD, arrAux);
    addArrays(N, arrR, arrAux, arrD);

    iter++;
  }

  if (id == 0)
  {

    printf("Numero de Iterações: %d\n", iter);
  }

  MPI_Finalize();
}

void createPosDefMatrix(int n, double *matA, double *arrX, double *arrB)
{
  int i;

  memset(matA, 0, n * sizeof(double));

  for (i = 0; i < n; i++)
  {
    arrB[i] = 1;
    arrX[i] = 0;
    matA[i * n + i] = 4;
  }

  for (i = 0; i < n - 1; i++)
  {
    matA[i * n + (i + 1)] = 1;
    matA[(i + 1) * n + i] = 1;
  }
}

double scaleArray(int n, double scalar, double *arr, double *result)
{

  int i;

  for (i = 0; i < n; i++)
  {
    result[i] = scalar * arr[i];
  }
}

double dotProd(int n, double *arrA, double *arrB)
{

  int i;
  double prod = 0;
  for (i = 0; i < n; i++)
  {
    prod += arrA[i] * arrB[i];
  }

  return prod;
}

void addArrays(int n, double *arrA, double *arrB, double *result)
{

  int i;
  for (i = 0; i < n; i++)
  {
    result[i] = arrA[i] + arrB[i];
  }
}

void subArrays(int n, double *arrA, double *arrB, double *result)
{

  int i;
  for (i = 0; i < n; i++)
  {
    result[i] = arrA[i] - arrB[i];
  }
}
