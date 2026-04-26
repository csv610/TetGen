#ifndef BENCH_UTILS_H
#define BENCH_UTILS_H

#define TETLIBRARY
#include "tetgen.h"
#include <chrono>
#include <vector>
#include <random>
#include <iostream>
#include <fstream>
#include <string>

struct BenchResult {
    std::string name;
    int input_size;
    double duration_ms;
    int output_tets;
};

inline void generate_random_points(int n, REAL* pointlist) {
    std::mt19937 gen(42);
    std::uniform_real_distribution<REAL> dis(-100.0, 100.0);
    for (int i = 0; i < n * 3; ++i) {
        pointlist[i] = dis(gen);
    }
}

inline void save_results_csv(const std::string& filename, const std::vector<BenchResult>& results) {
    std::ofstream file(filename);
    file << "Name,InputSize,DurationMS,OutputTets\n";
    for (const auto& r : results) {
        file << r.name << "," << r.input_size << "," << r.duration_ms << "," << r.output_tets << "\n";
    }
}

template<typename Func>
BenchResult profile(const std::string& name, int n, Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    int tets = f();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> diff = end - start;
    std::cout << name << " (" << n << " points): " << diff.count() << " ms" << std::endl;
    return {name, n, diff.count(), tets};
}

#endif
