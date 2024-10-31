#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

//Valores originales
// #define N 50
// #define MIN_SIZE 2
// #define MAX_SIZE 5000
// #define MAX_NUM 10000000.0
#define BLOCK_SIZE 32

//Valores prueba
#define N 10
#define MIN_SIZE 2
#define MAX_SIZE 1000
#define MAX_NUM 10000000.0

int main() 
{
    srand((unsigned) time(NULL));
    int size = MIN_SIZE + (rand() % (MAX_SIZE - MIN_SIZE + 1));
    int iterator = 0;
    double time_spent = 0.0;

    double (*A)[size] __attribute__((aligned(64))) = malloc(size * size * sizeof(double));
    double (*B)[size] __attribute__((aligned(64))) = malloc(size * size * sizeof(double));
    double (*C)[size] __attribute__((aligned(64))) = malloc(size * size * sizeof(double));

    while (iterator++ < N) 
    {
        printf("Running iteration number: %d\n", iterator);

        #pragma omp parallel for collapse(2)
        for (int i = 0; i < size; i++) 
        {
            for (int j = 0; j < size; j++) 
            {
                A[i][j] = 1 + ((double)rand() / RAND_MAX) * MAX_NUM;
                B[i][j] = 1 + ((double)rand() / RAND_MAX) * MAX_NUM;
                C[i][j] = 0.0;
            }
        }

        clock_t begin_time = clock();

        #pragma omp parallel for collapse(2) schedule(dynamic)
        for (int i = 0; i < size; i += BLOCK_SIZE) 
        {
            for (int j = 0; j < size; j += BLOCK_SIZE) 
            {
                for (int k = 0; k < size; k += BLOCK_SIZE) 
                {
                    for (int ii = i; ii < i + BLOCK_SIZE && ii < size; ii++) 
                    {
                        for (int jj = j; jj < j + BLOCK_SIZE && jj < size; jj++) 
                        {
                            double sum = 0.0;
                            for (int kk = k; kk < k + BLOCK_SIZE && kk < size; kk++) 
                            {
                                sum += A[ii][kk] * B[kk][jj];
                            }
                            C[ii][jj] += sum;
                        }
                    }
                }
            }
        }

        clock_t end_time = clock();
        time_spent += (double)(end_time - begin_time) / CLOCKS_PER_SEC;
    }

    printf("Size of matrices: %d \n", size);
    printf("Average running time: %f seconds\n", (time_spent / N));

    free(A);
    free(B);
    free(C);

    return 0;
}
