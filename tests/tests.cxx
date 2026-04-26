#define TETLIBRARY
#include <iostream>
#include <cassert>
#include <vector>
#include "../src/tetgen.h"

void test_simple_tetrahedron() {
    std::cout << "Running test_simple_tetrahedron..." << std::endl;
    tetgenio in, out;

    in.numberofpoints = 4;
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // 0: (0,0,0)
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    // 1: (1,0,0)
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    // 2: (0,1,0)
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    // 3: (0,0,1)
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;

    // Run tetrahedralization with quiet switch "Q"
    tetrahedralize((char*)"Q", &in, &out);

    std::cout << "  Number of output points: " << out.numberofpoints << std::endl;
    std::cout << "  Number of output tetrahedra: " << out.numberoftetrahedra << std::endl;

    assert(out.numberofpoints == 4);
    assert(out.numberoftetrahedra == 1);

    std::cout << "test_simple_tetrahedron passed!" << std::endl;
}

void test_cube() {
    std::cout << "Running test_cube..." << std::endl;
    tetgenio in, out;

    in.numberofpoints = 8;
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // 0: (0,0,0)
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    // 1: (1,0,0)
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    // 2: (1,1,0)
    in.pointlist[6] = 1; in.pointlist[7] = 1; in.pointlist[8] = 0;
    // 3: (0,1,0)
    in.pointlist[9] = 0; in.pointlist[10] = 1; in.pointlist[11] = 0;
    // 4: (0,0,1)
    in.pointlist[12] = 0; in.pointlist[13] = 0; in.pointlist[14] = 1;
    // 5: (1,0,1)
    in.pointlist[15] = 1; in.pointlist[16] = 0; in.pointlist[17] = 1;
    // 6: (1,1,1)
    in.pointlist[18] = 1; in.pointlist[19] = 1; in.pointlist[20] = 1;
    // 7: (0,1,1)
    in.pointlist[21] = 0; in.pointlist[22] = 1; in.pointlist[23] = 1;

    tetrahedralize((char*)"Q", &in, &out);

    std::cout << "  Number of output points: " << out.numberofpoints << std::endl;
    std::cout << "  Number of output tetrahedra: " << out.numberoftetrahedra << std::endl;

    assert(out.numberofpoints == 8);
    // A cube can be triangulated into 5 or 6 tetrahedra.
    assert(out.numberoftetrahedra >= 5);

    std::cout << "test_cube passed!" << std::endl;
}

void test_convex_hull() {
    std::cout << "Running test_convex_hull..." << std::endl;
    tetgenio in, out;

    in.numberofpoints = 5;
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // 0: (0,0,0)
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    // 1: (1,0,0)
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    // 2: (0,1,0)
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    // 3: (0,0,1)
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    // 4: (0.1, 0.1, 0.1) - Inside the tetrahedron
    in.pointlist[12] = 0.1; in.pointlist[13] = 0.1; in.pointlist[14] = 0.1;

    // "c" switch for convex hull
    tetrahedralize((char*)"Qc", &in, &out);

    std::cout << "  Number of output points: " << out.numberofpoints << std::endl;
    
    assert(out.numberofpoints >= 4);
    
    std::cout << "test_convex_hull passed!" << std::endl;
}

void test_voronoi() {
    std::cout << "Running test_voronoi..." << std::endl;
    tetgenio in, out;
    tetgenbehavior behavior;
    behavior.voroout = 1;
    behavior.quiet = 1;

    in.numberofpoints = 4;
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // 0: (0,0,0)
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    // 1: (1,0,0)
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    // 2: (0,1,0)
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    // 3: (0,0,1)
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;

    // Use the behavior object directly to enable Voronoi output
    tetrahedralize(&behavior, &in, &out);

    std::cout << "  Number of Voronoi points: " << out.numberofvpoints << std::endl;
    std::cout << "  Number of Voronoi edges: " << out.numberofvedges << std::endl;
    std::cout << "  Number of Voronoi facets: " << out.numberofvfacets << std::endl;
    std::cout << "  Number of Voronoi cells: " << out.numberofvcells << std::endl;

    assert(out.numberofvpoints > 0);
    assert(out.numberofvedges > 0);
    assert(out.numberofvfacets > 0);
    assert(out.numberofvcells > 0);

    std::cout << "test_voronoi passed!" << std::endl;
}

void test_constrained_tetmesh() {
    std::cout << "Running test_constrained_tetmesh..." << std::endl;
    tetgenio in, out;

    in.numberofpoints = 8;
    in.pointlist = new REAL[in.numberofpoints * 3];
    // A cube
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 1; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 1; in.pointlist[11] = 0;
    in.pointlist[12] = 0; in.pointlist[13] = 0; in.pointlist[14] = 1;
    in.pointlist[15] = 1; in.pointlist[16] = 0; in.pointlist[17] = 1;
    in.pointlist[18] = 1; in.pointlist[19] = 1; in.pointlist[20] = 1;
    in.pointlist[21] = 0; in.pointlist[22] = 1; in.pointlist[23] = 1;

    // Define facets of the cube (6) + 1 internal facet = 7 facets
    in.numberoffacets = 7;
    in.facetlist = new tetgenio::facet[in.numberoffacets];
    in.facetmarkerlist = new int[in.numberoffacets];

    // CRITICAL: Initialize facets to NULL
    for (int i = 0; i < in.numberoffacets; i++) {
        tetgenio::init(&in.facetlist[i]);
    }

    auto set_facet = [&](int idx, const std::vector<int>& vertices, int marker) {
        tetgenio::facet *f = &in.facetlist[idx];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        // Initialize polygon to NULL
        tetgenio::init(&f->polygonlist[0]);
        
        tetgenio::polygon *p = &f->polygonlist[0];
        p->numberofvertices = vertices.size();
        p->vertexlist = new int[p->numberofvertices];
        for (size_t i = 0; i < vertices.size(); ++i) p->vertexlist[i] = vertices[i];
        in.facetmarkerlist[idx] = marker;
    };

    set_facet(0, {0, 1, 2, 3}, 1); // Bottom
    set_facet(1, {4, 5, 6, 7}, 1); // Top
    set_facet(2, {0, 1, 5, 4}, 1); // Front
    set_facet(3, {1, 2, 6, 5}, 1); // Right
    set_facet(4, {2, 3, 7, 6}, 1); // Back
    set_facet(5, {3, 0, 4, 7}, 1); // Left
    set_facet(6, {0, 2, 6}, 2);    // Internal diagonal

    // "p" switch for Constrained Delaunay Tetrahedralization (CDT)
    tetrahedralize((char*)"Qp", &in, &out);

    std::cout << "  Number of output points: " << out.numberofpoints << std::endl;
    std::cout << "  Number of output tetrahedra: " << out.numberoftetrahedra << std::endl;
    std::cout << "  Number of output trifaces (subfaces): " << out.numberoftrifaces << std::endl;

    assert(out.numberofpoints >= 8);
    assert(out.numberoftetrahedra >= 5);
    assert(out.numberoftrifaces >= 1);

    std::cout << "test_constrained_tetmesh passed!" << std::endl;
}

int main() {
    try {
        test_simple_tetrahedron();
        test_cube();
        test_convex_hull();
        test_voronoi();
        test_constrained_tetmesh();
        std::cout << "All tests passed!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown error" << std::endl;
        return 1;
    }
    return 0;
}
