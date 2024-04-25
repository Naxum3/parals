#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

double wtime()
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((double)ts.tv_sec + (double)ts.tv_nsec * 1.e-9);
}

double func(double x){
    return pow(x, 2);
}

double integrate_omp(double (*func)(double), double a, double b, int n, int count)
{
    double h = (b - a) / n;
    double sum = 0.0;
    #pragma omp parallel num_threads(count)
    {
        int nthreads = omp_get_num_threads();
        int threadid = omp_get_thread_num();
        int items_per_thread = n / nthreads;
        int lb = threadid * items_per_thread;
        int ub = (threadid == nthreads - 1) ? (n - 1) : (lb + items_per_thread - 1);
        double sumloc = 0.0;
        for (int i = lb; i <= ub; i++){
            sumloc += func(a + h * (i + 0.5));
            #pragma omp atomic
            sum += sumloc;
        }
        sum *= h;
        return sum;
    }
}

const double PI = 3.14159265358979323846;
const double a = -4.0;
const double b = 4.0;
const int nsteps = 40000000;

double run_parallel(int count)
{
    double t = wtime();
    double res = integrate_omp(func, a, b, nsteps, count);
    t = wtime() - t;
    printf("Result (parallel): %.12f; error %.12f\n", res, fabs(res - sqrt(PI)));
    return t;
}

int main(int argc, char **argv)
{
    int num_pot[7] = {2,4,7,8,16,20,40};
    printf("Integration f(x) on [%.12f, %.12f], nsteps = %d\n", a, b, nsteps);
    double tserial = run_parallel(1);
    printf("Execution time (serial): %.6f\n", tserial);
    for (int i = 0; i < 7; i++){
        double tparallel = run_parallel(num_pot[i]);
        printf("Execution time (%d potocks): %.6f\n", num_pot[i], tparallel);
        printf("Speedup: %.2f\n", tserial / tparallel);
    }
    return 0;
}