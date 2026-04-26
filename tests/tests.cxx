#include <catch2/catch_test_macros.hpp>
#include <vector>
#include <set>
#include "../src/tetgen.h"

TEST_CASE("Simple Tetrahedron", "[tetgen]") {
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

    CHECK(out.numberofpoints == 4);
    CHECK(out.numberoftetrahedra == 1);
}

TEST_CASE("Cube Tetrahedralization", "[tetgen]") {
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

    CHECK(out.numberofpoints == 8);
    // A cube can be triangulated into 5 or 6 tetrahedra.
    CHECK(out.numberoftetrahedra >= 5);
}

TEST_CASE("Simple Convex Hull", "[tetgen][convex_hull]") {
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

    CHECK(out.numberofpoints >= 4);
}

TEST_CASE("Cube Convex Hull", "[tetgen][convex_hull]") {
    tetgenio in, out;

    in.numberofpoints = 9; // 8 cube vertices + 1 center point
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // Cube vertices
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 1; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 1; in.pointlist[11] = 0;
    in.pointlist[12] = 0; in.pointlist[13] = 0; in.pointlist[14] = 1;
    in.pointlist[15] = 1; in.pointlist[16] = 0; in.pointlist[17] = 1;
    in.pointlist[18] = 1; in.pointlist[19] = 1; in.pointlist[20] = 1;
    in.pointlist[21] = 0; in.pointlist[22] = 1; in.pointlist[23] = 1;
    
    // 8: Center point (0.5, 0.5, 0.5)
    in.pointlist[24] = 0.5; in.pointlist[25] = 0.5; in.pointlist[26] = 0.5;

    // "c" switch for convex hull
    tetrahedralize((char*)"Qc", &in, &out);

    std::set<int> hull_points;
    for (int i = 0; i < out.numberoftrifaces; i++) {
        hull_points.insert(out.trifacelist[i * 3]);
        hull_points.insert(out.trifacelist[i * 3 + 1]);
        hull_points.insert(out.trifacelist[i * 3 + 2]);
    }

    CHECK(hull_points.size() == 8);
    for (int i = 0; i < 8; i++) {
        CHECK(hull_points.count(i) > 0);
    }
    CHECK(hull_points.count(8) == 0);
}

TEST_CASE("Voronoi Diagram", "[tetgen][voronoi]") {
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

    CHECK(out.numberofvpoints > 0);
    CHECK(out.numberofvedges > 0);
    CHECK(out.numberofvfacets > 0);
    CHECK(out.numberofvcells > 0);
}

TEST_CASE("Constrained TetMesh", "[tetgen][cdt]") {
    tetgenio in, out;

    in.numberofpoints = 8;
    in.pointlist = new REAL[in.numberofpoints * 3];
    in.pointmarkerlist = new int[in.numberofpoints];
    
    // A cube
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 1; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 1; in.pointlist[11] = 0;
    in.pointlist[12] = 0; in.pointlist[13] = 0; in.pointlist[14] = 1;
    in.pointlist[15] = 1; in.pointlist[16] = 0; in.pointlist[17] = 1;
    in.pointlist[18] = 1; in.pointlist[19] = 1; in.pointlist[20] = 1;
    in.pointlist[21] = 0; in.pointlist[22] = 1; in.pointlist[23] = 1;

    for (int i = 0; i < 8; i++) in.pointmarkerlist[i] = 0;

    // Define 12 edges of the cube
    in.numberofedges = 12;
    in.edgelist = new int[in.numberofedges * 2];
    in.edgemarkerlist = new int[in.numberofedges];

    auto set_edge = [&](int idx, int v1, int v2, int marker) {
        in.edgelist[idx * 2] = v1;
        in.edgelist[idx * 2 + 1] = v2;
        in.edgemarkerlist[idx] = marker;
    };

    set_edge(0, 0, 1, 1); set_edge(1, 1, 2, 1); set_edge(2, 2, 3, 1); set_edge(3, 3, 0, 1); // Bottom
    set_edge(4, 4, 5, 1); set_edge(5, 5, 6, 1); set_edge(6, 6, 7, 1); set_edge(7, 7, 4, 1); // Top
    set_edge(8, 0, 4, 1); set_edge(9, 1, 5, 1); set_edge(10, 2, 6, 1); set_edge(11, 3, 7, 1); // Verticals

    in.numberoffacets = 6; // Just the cube faces
    in.facetlist = new tetgenio::facet[in.numberoffacets];
    in.facetmarkerlist = new int[in.numberoffacets];

    for (int i = 0; i < in.numberoffacets; i++) {
        tetgenio::init(&in.facetlist[i]);
    }

    auto set_facet = [&](int idx, const std::vector<int>& vertices, int marker) {
        tetgenio::facet *f = &in.facetlist[idx];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
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

    tetrahedralize((char*)"Qp", &in, &out);

    CHECK(out.numberofpoints >= 8);
    CHECK(out.numberoftetrahedra >= 5);
}
