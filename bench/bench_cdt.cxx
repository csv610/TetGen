#include "utils.h"
#include "TetDelMesher.h"

void add_cube(TetDelMesher& gen, double center_x, double center_y, double center_z, double side, int& start_node) {
    double h = side / 2.0;
    double coords[8][3] = {
        {center_x-h, center_y-h, center_z-h}, {center_x+h, center_y-h, center_z-h},
        {center_x+h, center_y+h, center_z-h}, {center_x-h, center_y+h, center_z-h},
        {center_x-h, center_y-h, center_z+h}, {center_x+h, center_y-h, center_z+h},
        {center_x+h, center_y+h, center_z+h}, {center_x-h, center_y+h, center_z+h}
    };
    for (int i = 0; i < 8; ++i) {
        gen.addPoint(coords[i][0], coords[i][1], coords[i][2]);
    }

    int faces[6][4] = {
        {0,1,2,3}, {4,5,6,7}, {0,1,5,4}, {1,2,6,5}, {2,3,7,6}, {3,0,4,7}
    };

    for (int i = 0; i < 6; ++i) {
        std::vector<int> f;
        for (int j = 0; j < 4; ++j) f.push_back(start_node + faces[i][j]);
        gen.addFacet(f);
    }
    start_node += 8;
}

int bench_cube() {
    TetDelMesher gen;
    int start_node = 0;
    add_cube(gen, 0, 0, 0, 10.0, start_node);
    TetDelMesherConfig config;
    config.isPLC = true;
    config.quiet = true;
    gen.setConfig(config);
    auto mesh = gen.generate();
    return static_cast<int>(mesh.tetrahedra.size());
}

int bench_shell() {
    TetDelMesher gen;
    int start_node = 0;
    add_cube(gen, 0, 0, 0, 10.0, start_node); // Outer
    add_cube(gen, 0, 0, 0, 5.0, start_node);  // Inner
    // Add a hole point in the center to make it a shell
    gen.addHole(0, 0, 0);
    
    TetDelMesherConfig config;
    config.isPLC = true;
    config.quiet = true;
    gen.setConfig(config);
    auto mesh = gen.generate();
    return static_cast<int>(mesh.tetrahedra.size());
}

int run_bench(int n) {
    TetDelMesher gen;
    std::vector<double> points(n * 3);
    generate_random_points(n, points.data());
    
    for (int i = 0; i < n; ++i) {
        gen.addPoint(points[i * 3], points[i * 3 + 1], points[i * 3 + 2]);
    }

    TetDelMesherConfig config;
    config.isPLC = true;
    config.quiet = true;
    gen.setConfig(config);
    auto mesh = gen.generate();
    return static_cast<int>(mesh.tetrahedra.size());
}

int main() {
    std::vector<BenchResult> results;
    results.push_back(profile("CDT Cube", 8, bench_cube));
    results.push_back(profile("CDT Spherical Shell (approx)", 16, bench_shell));
    
    results.push_back(profile("CDT Random Points", 10000, []() { return run_bench(10000); }));
    results.push_back(profile("CDT Random Points", 50000, []() { return run_bench(50000); }));
    for (int n = 100000; n <= 1000000; n += 100000) {
        results.push_back(profile("CDT Random Points", n, [n]() { return run_bench(n); }));
    }
    save_results_csv("bench/bench_cdt.csv", results);
    return 0;
}
