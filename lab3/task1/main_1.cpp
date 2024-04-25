#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <cmath>
#include <chrono>

int num_of_threads = 0; 

void calculate(const double* matrix, const double* vector, double* result, int start, int end, int n) {
    for (int i = start; i < end; ++i) {
        double sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += matrix[i * n + j] * vector[j];
        }
        result[i] = sum;
    }
}

void matrix_init(double* matrix, int start, int end, int n){
    for (int i = start; i < end; ++i){
        for(int j = 0; j < n; ++j){
            matrix[i * n + j] = i + j;
        }
    }
}

void run_parallel(int n){
    std::vector<std::thread> threads;
    int chunk_size = std::ceil(static_cast<double>(n) / num_of_threads);

    //parrallel matirx init
    double* matrix = new double[n * n];
    for (int i = 0; i < num_of_threads; ++i){
        int start = i * chunk_size;
        int end = std::min((i + 1) * chunk_size, n);
        threads.push_back(std::thread(matrix_init, matrix, start, end, n));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    //parrallel matirx init

    double* vector = new double[n];
    for (int i = 0; i < n; ++i) {
        vector[i] = i;
    }

    double* result = new double[n]{0};
    threads.clear();
    //get time and calculate
    auto start_time = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_of_threads; ++i) {
        int start = i * chunk_size;
        int end = std::min((i + 1) * chunk_size, n);
        threads.push_back(std::thread(calculate, matrix, vector, result, start, end, n));
    }

    for (auto& thread : threads) {
        thread.join();
    }
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    //get time and calculate

    delete[] matrix;
    delete[] vector;
    delete[] result;
}

int main(int argc, char **argv) {
    int n = 20000;
    num_of_threads = atoi(argv[1]);
    //num_of_threads = 1;
    run_parallel(n);
    return 0;
}
