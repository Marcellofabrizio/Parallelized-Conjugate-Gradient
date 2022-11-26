#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
#define INTMAX 100
#define ERRO 0.00000001

/*----------------------------------------------------------------------------*/
double tempoCorrente(void)
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return (tval.tv_sec + tval.tv_usec / 1000000.0);
}

/*----------------------------------------------------------------------------*/
void inicializa(int n, double *matA, double *vetX, double *vetB)
{
	int i;

	memset(matA, 0, n * sizeof(double));

	for (i = 0; i < n; i++)
		vetB[i] = 1;

	for (i = 0; i < n; i++)
		vetX[i] = 0;

	for (i = 0; i < n; i++)
		matA[i * n + i] = 4;

	for (i = 0; i < n - 1; i++)
	{
		matA[i * n + (i + 1)] = 1;
		matA[(i + 1) * n + i] = 1;
	}
}
/*----------------------------------------------------------------------------*/
void multiplicacao_matriz_vetor(int n, double *mat, double *vet, double *res)
{
	int i, j;
	for (i = 0; i < n; i++)
	{
		res[i] = 0;
		for (j = 0; j < n; j++)
		{
			res[i] += mat[i * n + j] * vet[j];
		}
	}
}

/*----------------------------------------------------------------------------*/
void subtracao_vetor(int n, double *vet1, double *vet2, double *res)
{
	int i;
	for (i = 0; i < n; i++)
		res[i] = vet1[i] - vet2[i];
}

/*----------------------------------------------------------------------------*/
void soma_vetor(int n, double *vet1, double *vet2, double *res)
{
	int i;
	for (i = 0; i < n; i++)
		res[i] = vet1[i] + vet2[i];
}

/*----------------------------------------------------------------------------*/
double produto_escalar(int n, double *vet1, double *vet2)
{
	int i;
	double prod = 0;
	for (i = 0; i < n; i++)
		prod += vet1[i] * vet2[i];
	return (prod);
}

/*----------------------------------------------------------------------------*/
double escalar_vetor(int n, double escalar, double *vet, double *res)
{
	int i;
	for (i = 0; i < n; i++)
		res[i] = escalar * vet[i];
}

/*----------------------------------------------------------------------------*/
int main(int argc, char **argv)
{

	double *matA, *vetX, *vetB, *vetAux, *vetR, *vetD, *vetQ;
	int n, i, j;
	int iteracao = 0;
	double sigma_novo, sigma_0, alpha, sigma_velho, beta;
	double inicio, fim;

	if (argc != 2)
	{
		printf("%s <ordem_da_matriz>\n", argv[0]);
		exit(0);
	}

	n = atoi(argv[1]);

	matA = (double *)malloc(n * n * sizeof(double));
	vetX = (double *)malloc(n * sizeof(double));
	vetB = (double *)malloc(n * sizeof(double));
	vetAux = (double *)malloc(n * sizeof(double));
	vetR = (double *)malloc(n * sizeof(double));
	vetD = (double *)malloc(n * sizeof(double));
	vetQ = (double *)malloc(n * sizeof(double));

	inicializa(n, matA, vetX, vetB);

	inicio = tempoCorrente();

	multiplicacao_matriz_vetor(n, matA, vetX, vetAux);

	subtracao_vetor(n, vetB, vetAux, vetR);

	memcpy(vetD, vetR, n * sizeof(double));

	sigma_novo = produto_escalar(n, vetR, vetR);

	sigma_0 = sigma_novo;

	while ((iteracao < INTMAX) && (sigma_novo > ERRO))
	{
		multiplicacao_matriz_vetor(n, matA, vetD, vetQ);

		for (i = 0; i < n; i++)
			printf("Array Q: %.2f\n", vetQ[i]);

		alpha = sigma_novo / produto_escalar(n, vetD, vetQ);

		escalar_vetor(n, alpha, vetD, vetAux);

		soma_vetor(n, vetX, vetAux, vetX);

		escalar_vetor(n, alpha, vetQ, vetAux);

		subtracao_vetor(n, vetR, vetAux, vetR);

		sigma_velho = sigma_novo;
		sigma_novo = produto_escalar(n, vetR, vetR);

		beta = sigma_novo / sigma_velho;

		escalar_vetor(n, beta, vetD, vetAux);

		soma_vetor(n, vetR, vetAux, vetD);

		iteracao++;
	}
	fim = tempoCorrente();

	printf("Tempo %f  | Numero de Iteracoes: %i\n", fim - inicio, iteracao);

	free(matA);
	free(vetX);
	free(vetB);
	free(vetAux);
	free(vetR);
	free(vetD);
	free(vetQ);
}
