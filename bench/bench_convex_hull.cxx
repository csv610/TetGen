#include "utils.h"

int run_bench(int n) {
    tetgenio in, out;
    in.numberofpoints = n;
    in.pointlist = new REAL[n * 3];
    generate_random_points(n, in.pointlist);

    // -c: Convex hull only
    // -Q: Quiet
    tetrahedralize((char*)"cQ", &in, &out);
    return out.numberoftetrahedra;
}

int main() {
    std::vector<BenchResult> results;
    results.push_back(profile("Convex Hull", 10000, []() { return run_bench(10000); }));
    results.push_back(profile("Convex Hull", 50000, []() { return run_bench(50000); }));
    for (int n = 100000; n <= 1000000; n += 100000) {
        results.push_back(profile("Convex Hull", n, [n]() { return run_bench(n); }));
    }
    save_results_csv("bench_convex_hull.csv", results);
    return 0;
}
