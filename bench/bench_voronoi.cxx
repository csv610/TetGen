#include "utils.h"
#include "TetDelMesher.h"

int run_bench(int n) {
    TetDelMesher gen;
    std::vector<double> points(n * 3);
    generate_random_points(n, points.data());
    
    for (int i = 0; i < n; ++i) {
        gen.addPoint(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
    }

    gen.setVoronoi(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    return static_cast<int>(mesh.tetrahedra.size());
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
