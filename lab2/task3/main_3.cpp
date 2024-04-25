#include <iostream>
#include <string.h>
#include <omp.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

//#pragma omp parallel num_threads(num_of_threads)
/*
int nthreads = omp_get_num_threads();
int threadid = omp_get_thread_num();
int items_per_thread = n / nthreads;
int lb = threadid * items_per_thread;
int ub = (threadid == nthreads - 1) ? (n - 1) : (lb + items_per_thread - 1);
*/

using namespace std;
const double EPSILON = 0.000001; // Погрешность
int num_of_threads = 0;

void printMatrix(double** A, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            cout << A[i][j] << "\t";
        }
        cout << endl;
    }
}

void new_solve1(double* A, double* x, int n, long long max_iters) {
    for (int i = 0; i < n; ++i) {
        x[i] = 0;
    }
    double error = 0, sum = 0; 
    int i = 0, j = 0;
    double t = omp_get_wtime();
    #pragma omp parallel shared(A, x, error) private(i, j, sum) num_threads (num_of_threads)
    {
        for (int iter = 0; iter < max_iters; ++iter) {
            error = 0;
            for (i = 0; i < n; ++i) {
                sum = 0;
                for (j = 0; j < n; ++j) {
                sum += A[i * n + j] * x[j];
            }
            error += ((sum - A[i * n + n]) * (sum - A[i * n + n])) / (A[i * n + n] * A[i * n + n]);
            x[i] -= 0.01 * (sum - A[i * n + n]);
            }
            if (error < EPSILON * EPSILON) {
                break;
            }
        }
    }
    t = omp_get_wtime() - t;
    cout << "No using for: " << t << "\n";
}

void new_solve2(double* A, double* x, int n, long long max_iters) {
    for (int i = 0; i < n; ++i) {
        x[i] = 0;
    }
    double error = 0, sum = 0; 
    int i = 0, j = 0;
    double t = omp_get_wtime();
    for (int iter = 0; iter < max_iters; ++iter) {
        error = 0;
        #pragma omp parallel for private(i, j, sum) shared(A, x, error) num_threads(num_of_threads)
        for (i = 0; i < n; ++i) {
            sum = 0;
            for (j = 0; j < n; ++j) {
                sum += A[i * n + j] * x[j];
            }
            error += ((sum - A[i * n + n]) * (sum - A[i * n + n])) / (A[i * n + n] * A[i * n + n]);
            x[i] -= 0.01 * (sum - A[i * n + n]);
        }
        if (error < EPSILON * EPSILON) {
            break;
        }
    }
    t = omp_get_wtime() - t;
    cout << "Using for: "<< t << "\n";
}

int main(int argc, char **argv) {
    //num_of_threads = atoi(argv[1]);
    num_of_threads = 16;
    int n = 2200;
    double* A = new double [n * n + n];

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n + 1; ++j) {
            A[i * n + j] = 1;
        }
        A[i * n + i] = 2;
        A[i * n + n] = n + 1;
    }
    //printMatrix(A, n);

    double* x = new double[n] {0};

    long long maxIterations = 3000;

    new_solve2(A, x, n, maxIterations);
    new_solve1(A, x, n, maxIterations);
    
    delete[] A;
    delete[] x;

    return 0;
}