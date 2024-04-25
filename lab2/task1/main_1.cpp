#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double cpuSecond()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

void matrix_vector_product(double *a, double *b, double *c, size_t m, size_t n)
{
    for (int i = 0; i < m; i++)
    {
        c[i] = 0.0;
        for (int j = 0; j < n; j++)
            c[i] += a[i * n + j] * b[j];
    }
}

void matrix_vector_product_omp(double *a, double *b, double *c, size_t m, size_t n, int count)
{
#pragma omp parallel num_threads(count)
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);
        for (int i = lb; i <= ub; i++)
        {
            c[i] = 0.0;
            for (int j = 0; j < n; j++)
                c[i] += a[i * n + j] * b[j];
        }
    }
}

float run_serial(size_t n, size_t m)
{
    double *a, *b, *c;
    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }

    for (size_t i = 0; i < m; i++)
    {
        for (size_t j = 0; j < n; j++)
            a[i * n + j] = i + j;
    }

    for (size_t j = 0; j < n; j++)
        b[j] = j;

    double t = cpuSecond();
    matrix_vector_product(a, b, c, m, n);
    t = cpuSecond() - t;

    printf("Elapsed time (serial): %.6f sec.\n", t);
    free(a);
    free(b);
    free(c);
    return t;
}

float run_parallel(size_t n, size_t m, int count)
{
    double *a, *b, *c;

    a = (double*)malloc(sizeof(*a) * m * n);
    b = (double*)malloc(sizeof(*b) * n);
    c = (double*)malloc(sizeof(*c) * m);

    if (a == NULL || b == NULL || c == NULL)
    {
        free(a);
        free(b);
        free(c);
        printf("Error allocate memory!\n");
        exit(1);
    }
    #pragma omp parallel num_threads(count)
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = m / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (m - 1) : (lb + items_per_thread - 1);

        for (size_t i = 0; i < m; i++)
        {
            for (size_t j = 0; j < n; j++)
                a[i * n + j] = i + j;
        }



        for (size_t j = 0; j < n; j++)
            b[j] = j;
    }

    double t = cpuSecond();
    matrix_vector_product_omp(a, b, c, m, n, count);
    t = cpuSecond() - t;

    printf("Elapsed time (%d parallel): %.6f sec.\n", count, t);
    free(a);
    free(b);
    free(c);
    return t;
}

int main(int argc, char *argv[])
{
    int num_pot[7] = {2,4,7,8,16,20,40};
    int matrixM[2] = {20000, 40000};
    int matrixN[2] = {20000, 40000};
    float result[16];
    printf("20000x20000\n");
    result[0] = run_serial(matrixM[0], matrixN[0]);
    for (int i = 0; i < 7; i++){
        result[i+1] = result[0]/run_parallel(matrixM[0], matrixN[0], num_pot[i]);

    } 
    printf("40000x40000\n");
    result[8] = run_serial(matrixM[1], matrixN[1]);
    for (int i = 0; i < 7; i++){
        result[i+9] = result[8]/run_parallel(matrixM[1], matrixN[1], num_pot[i]);

    } 
    for (int i = 0; i < 16; i++){
        printf("%.6f ", result[i]);
        if (i == 7){
            printf("\n");
        }
    }
    // size_t M = 20000;
    // size_t N = 20000;
    // int count = 4;
    // if (argc > 1)
    //     M = atoi(argv[1]);
    // if (argc > 2)
    //     N = atoi(argv[2]);
    // run_serial(M, N);
    // run_parallel(M, N, count);
    // return 0;
}