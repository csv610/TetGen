#include "utils.h"

int run_bench(int n) {
    tetgenio in, out;
    tetgenbehavior behavior;
    behavior.voroout = 1;
    behavior.quiet = 1;

    in.numberofpoints = n;
    in.pointlist = new REAL[n * 3];
    generate_random_points(n, in.pointlist);

    // Use the behavior object directly to enable Voronoi output
    tetrahedralize(&behavior, &in, &out);
    return out.numberoftetrahedra;
}

int main() {
    std::vector<BenchResult> results;
    results.push_back(profile("Voronoi Diagram", 10000, []() { return run_bench(10000); }));
    results.push_back(profile("Voronoi Diagram", 50000, []() { return run_bench(50000); }));
    for (int n = 100000; n <= 1000000; n += 100000) {
        results.push_back(profile("Voronoi Diagram", n, [n]() { return run_bench(n); }));
    }
    save_results_csv("bench_voronoi.csv", results);
    return 0;
}
