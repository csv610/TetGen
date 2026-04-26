#include "utils.h"

void add_cube(tetgenio& in, REAL center_x, REAL center_y, REAL center_z, REAL side) {
    int start_node = in.numberofpoints;
    int n = in.numberofpoints + 8;
    REAL* new_points = new REAL[n * 3];
    if (in.pointlist) {
        std::copy(in.pointlist, in.pointlist + in.numberofpoints * 3, new_points);
        delete[] in.pointlist;
    }
    in.pointlist = new_points;
    
    REAL h = side / 2.0;
    REAL coords[8][3] = {
        {center_x-h, center_y-h, center_z-h}, {center_x+h, center_y-h, center_z-h},
        {center_x+h, center_y+h, center_z-h}, {center_x-h, center_y+h, center_z-h},
        {center_x-h, center_y-h, center_z+h}, {center_x+h, center_y-h, center_z+h},
        {center_x+h, center_y+h, center_z+h}, {center_x-h, center_y+h, center_z+h}
    };
    for (int i = 0; i < 8; ++i) {
        in.pointlist[(start_node + i) * 3 + 0] = coords[i][0];
        in.pointlist[(start_node + i) * 3 + 1] = coords[i][1];
        in.pointlist[(start_node + i) * 3 + 2] = coords[i][2];
    }
    in.numberofpoints = n;

    int start_facet = in.numberoffacets;
    int nf = in.numberoffacets + 6;
    tetgenio::facet* new_facets = new tetgenio::facet[nf];
    if (in.facetlist) {
        // Note: Simple copy doesn't work well with pointers inside, but here we are adding new ones
        std::copy(in.facetlist, in.facetlist + in.numberoffacets, new_facets);
        delete[] in.facetlist;
    }
    in.facetlist = new_facets;

    int faces[6][4] = {
        {0,1,2,3}, {4,5,6,7}, {0,1,5,4}, {1,2,6,5}, {2,3,7,6}, {3,0,4,7}
    };

    for (int i = 0; i < 6; ++i) {
        tetgenio::facet* f = &in.facetlist[start_facet + i];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[1];
        f->numberofholes = 0;
        f->holelist = NULL;
        tetgenio::polygon* p = &f->polygonlist[0];
        p->numberofvertices = 4;
        p->vertexlist = new int[4];
        for (int j = 0; j < 4; ++j) {
            p->vertexlist[j] = start_node + faces[i][j];
        }
    }
    in.numberoffacets = nf;
}

int bench_cube() {
    tetgenio in, out;
    add_cube(in, 0, 0, 0, 10.0);
    tetrahedralize((char*)"pQ", &in, &out);
    return out.numberoftetrahedra;
}

int bench_shell() {
    tetgenio in, out;
    add_cube(in, 0, 0, 0, 10.0); // Outer
    add_cube(in, 0, 0, 0, 5.0);  // Inner
    // Add a hole point in the center to make it a shell
    in.numberofholes = 1;
    in.holelist = new REAL[3];
    in.holelist[0] = 0; in.holelist[1] = 0; in.holelist[2] = 0;
    
    tetrahedralize((char*)"pQ", &in, &out);
    return out.numberoftetrahedra;
}

int run_bench(int n) {
    tetgenio in, out;
    in.numberofpoints = n;
    in.pointlist = new REAL[n * 3];
    generate_random_points(n, in.pointlist);

    // -p: Tetrahedralize a piecewise linear complex (PLC).
    // -Q: Quiet
    tetrahedralize((char*)"pQ", &in, &out);
    return out.numberoftetrahedra;
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
    save_results_csv("bench_cdt.csv", results);
    return 0;
}
