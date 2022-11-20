#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <mpi.h>

void createPosDefMatrix(int n, double *matA, double *arrX, double *arrB); 
void scaleArray(int n, double scalar, double *arr, double *result); 
void dotProd(int n, double *arrA, double *arrB);
void addArrays(int n, double *arrA, double *arrB, double *result);
void subArrays(int n, double *arrA, double *arrB, double *result);
void arrayMultiplication(int n, double *arrA, double *arrB, double *result);

int main(int argc, char **argv) {

  int id, np, N, i, j;
  double *matA, *arrX, *arrB, *arrAux, *arrOpp, *arrR, *arrD, *arrQ, *arrProd, *arrResCG;
  
  if(argc != 2) {
		printf("usage: %s <matrix_order>\n", argv[0]);
		exit(0);
	}

  N = atoi(argv[1]);

  matA     = (double *) malloc(N*N*sizeof(double));
  arrX     = (double *) malloc(N*sizeof(double));
  arrB     = (double *) malloc(N*sizeof(double));
  arrAux   = (double *) malloc(N*sizeof(double));
  arrR     = (double *) malloc(N*sizeof(double));
  arrD     = (double *) malloc(N*sizeof(double));
  arrQ     = (double *) malloc(N*sizeof(double));
  arrOpp   = (double *) malloc(N*sizeof(double));  
  arrProd  = (double *) malloc(N*sizeof(double));  
  arrResCG = (double *) malloc(N*sizeof(double));  

  MPI_Status s;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  MPI_Comm_size(MPI_COMM_WORLD, &np);

  createPosDefMatrix(N, matA, arrX, arrB);

  MPI_Bcast(arrX, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  MPI_Scatter(matA, N/N*np, MPI_DOUBLE, arrOpp, N/N*np, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  /* ------------------------- MATRIX MULTIPLICATION ------------------------- */

  for(i = 0; i < N/np; i++) {
    arrProd[i] = 0;
    for(j = 0; j < N; j++) {
      printf("%lf\n",  arrX[j]);
      arrProd[i] = arrOpp[i*N+j] * arrX[j];
    }
  }

  MPI_Gather(arrProd, N/np, MPI_DOUBLE, arrAux, N/np, MPI_DOUBLE, 0, MPI_COMM_WORLD);

  if(id == 0) {
    for(i = 0; i < N; i++) {
      printf("final[%d] = %lf\n", i, arrAux[i]);
    }
  }

  MPI_Finalize();
}

void createPosDefMatrix(int n, double *matA, double *arrX, double *arrB) {

  int i;

  memset(matA, 0, n * sizeof(double));

  for(i = 0; i < n; i++) {
    arrB[i] = 1;
    arrX[i] = 0;
    matA[i*n+i] = 4;
  }

  for(i = 0; i < n-1; i++) {
    matA[i*n+(i+1)] = 1;
    matA[(i+1)*n+1] = 1;
  }

}

void scaleArray(int n, double scalar, double *arr, double *result) {

  int i;

  for(i = 0; i < n; i++) {
    result[i] = scalar * arr[i];
  }

}

void dotProd(int n, double *arrA, double *arrB) {

  int i;
  double prod = 0;
  for(i = 0; i < n; i++) {
    prod += arrA[i] * arrB[i];
  }

  return(prod);
}

void addArrays(int n, double *arrA, double *arrB, double *result) {

  int i;
  for(i = 0; i < n; i++) {
    result[i] = arrA[i] + arrB[i];
  }

}

void subArrays(int n, double *arrA, double *arrB, double *result) {

  int i;
  for(i = 0; i < n; i++) {
    result[i] = arrA[i] - arrB[i];
  }

}
