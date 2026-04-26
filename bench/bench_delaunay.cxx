#include "utils.h"

int run_bench(int n) {
    tetgenio in, out;
    in.numberofpoints = n;
    in.pointlist = new REAL[n * 3];
    generate_random_points(n, in.pointlist);

    // Default: Delaunay tetrahedralization
    // -Q: Quiet
    tetrahedralize((char*)"Q", &in, &out);
    return out.numberoftetrahedra;
}

int main() {
    std::vector<BenchResult> results;
    results.push_back(profile("Delaunay Triangulation", 10000, []() { return run_bench(10000); }));
    results.push_back(profile("Delaunay Triangulation", 50000, []() { return run_bench(50000); }));
    for (int n = 100000; n <= 1000000; n += 100000) {
        results.push_back(profile("Delaunay Triangulation", n, [n]() { return run_bench(n); }));
    }
    save_results_csv("bench_delaunay.csv", results);
    return 0;
}
