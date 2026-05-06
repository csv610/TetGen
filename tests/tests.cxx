#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <vector>
#include <set>
#include <cmath>
#include <random>
#include "../src/tetgen.h"
#include "TetMeshGenerator.h"
#include "MeshOptimizer.hpp"

TEST_CASE("Tetrahedron Basic", "[tetgen][basic]") {
    tetgenio in, out;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 4);
    CHECK(out.numberoftetrahedra == 1);
}

TEST_CASE("Cube Tetrahedralization", "[tetgen][basic]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1
    };
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 8);
    CHECK(out.numberoftetrahedra >= 5);
}

TEST_CASE("Empty Point List", "[tetgen][edge]") {
    tetgenio in, out;
    in.numberofpoints = 0;
    in.pointlist = nullptr;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 0);
}

TEST_CASE("Single Point", "[tetgen][edge]") {
    tetgenio in, out;
    in.numberofpoints = 1;
    in.pointlist = new REAL[3];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 1);
}

TEST_CASE("Two Points", "[tetgen][edge]") {
    tetgenio in, out;
    in.numberofpoints = 2;
    in.pointlist = new REAL[6];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 2);
}

TEST_CASE("Three Points (Collinear)", "[tetgen][edge]") {
    tetgenio in, out;
    in.numberofpoints = 3;
    in.pointlist = new REAL[9];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 2; in.pointlist[7] = 0; in.pointlist[8] = 0;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints == 3);
}

TEST_CASE("Four Points (Tetrahedron)", "[tetgen][basic]") {
    tetgenio in, out;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberoftetrahedra == 1);
    REQUIRE(out.numberoftetrahedra > 0);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            CHECK((out.tetrahedronlist[i * 4 + j] >= 0 && out.tetrahedronlist[i * 4 + j] < 4));
        }
    }
}

TEST_CASE("Convex Hull Simple", "[tetgen][convex_hull]") {
    tetgenio in, out;
    in.numberofpoints = 5;
    in.pointlist = new REAL[15];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    in.pointlist[12] = 0.1; in.pointlist[13] = 0.1; in.pointlist[14] = 0.1;
    tetrahedralize((char*)"Qc", &in, &out);
    CHECK(out.numberoftrifaces >= 4);
}

TEST_CASE("Convex Hull Cube", "[tetgen][convex_hull]") {
    tetgenio in, out;
    in.numberofpoints = 9;
    in.pointlist = new REAL[27];
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1,
        0.5,0.5,0.5
    };
    std::copy(coords, coords + 27, in.pointlist);
    tetrahedralize((char*)"Qc", &in, &out);
    std::set<int> hull_points;
    for (int i = 0; i < out.numberoftrifaces; i++) {
        hull_points.insert(out.trifacelist[i * 3]);
        hull_points.insert(out.trifacelist[i * 3 + 1]);
        hull_points.insert(out.trifacelist[i * 3 + 2]);
    }
    CHECK(hull_points.size() == 8);
}

TEST_CASE("Voronoi Basic", "[tetgen][voronoi]") {
    tetgenio in, out;
    tetgenbehavior behavior;
    behavior.voroout = 1;
    behavior.quiet = 1;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    tetrahedralize(&behavior, &in, &out);
    CHECK(out.numberofvpoints > 0);
    CHECK(out.numberofvedges > 0);
    CHECK(out.numberofvfacets > 0);
    CHECK(out.numberofvcells > 0);
}

TEST_CASE("Voronoi Multiple Points", "[tetgen][voronoi]") {
    tetgenio in, out;
    tetgenbehavior behavior;
    behavior.voroout = 1;
    behavior.quiet = 1;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    for (int i = 0; i < 8; i++) {
        in.pointlist[i * 3] = (i & 1) ? 1.0 : 0.0;
        in.pointlist[i * 3 + 1] = (i & 2) ? 1.0 : 0.0;
        in.pointlist[i * 3 + 2] = (i & 4) ? 1.0 : 0.0;
    }
    tetrahedralize(&behavior, &in, &out);
    CHECK(out.numberofvcells == 8);
}

TEST_CASE("PLC Cube", "[tetgen][plc]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    in.pointmarkerlist = new int[8];
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1
    };
    std::copy(coords, coords + 24, in.pointlist);
    for (int i = 0; i < 8; i++) in.pointmarkerlist[i] = 0;

    in.numberofedges = 12;
    in.edgelist = new int[24];
    in.edgemarkerlist = new int[12];
    int edges[] = {0,1, 1,2, 2,3, 3,0, 4,5, 5,6, 6,7, 7,4, 0,4, 1,5, 2,6, 3,7};
    std::copy(edges, edges + 24, in.edgelist);
    for (int i = 0; i < 12; i++) in.edgemarkerlist[i] = 1;

    in.numberoffacets = 6;
    in.facetlist = new tetgenio::facet[6];
    in.facetmarkerlist = new int[6];
    for (int i = 0; i < 6; i++) tetgenio::init(&in.facetlist[i]);

    auto set_facet = [&](int idx, const std::vector<int>& v, int m) {
        auto* f = &in.facetlist[idx];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[1];
        tetgenio::init(&f->polygonlist[0]);
        auto* p = &f->polygonlist[0];
        p->numberofvertices = v.size();
        p->vertexlist = new int[v.size()];
        std::copy(v.begin(), v.end(), p->vertexlist);
        in.facetmarkerlist[idx] = m;
    };
    set_facet(0, {0,1,2,3}, 1);
    set_facet(1, {4,5,6,7}, 1);
    set_facet(2, {0,1,5,4}, 1);
    set_facet(3, {1,2,6,5}, 1);
    set_facet(4, {2,3,7,6}, 1);
    set_facet(5, {3,0,4,7}, 1);

    tetrahedralize((char*)"Qp", &in, &out);
    CHECK(out.numberofpoints >= 8);
    CHECK(out.numberoftetrahedra >= 5);
}

TEST_CASE("PLC with Hole", "[tetgen][plc]") {
    tetgenio in, out;
    in.numberofpoints = 20;
    in.pointlist = new REAL[60];
    for (int i = 0; i < 20; i++) {
        double angle = i * 2 * M_PI / 20;
        in.pointlist[i * 3] = cos(angle);
        in.pointlist[i * 3 + 1] = sin(angle);
        in.pointlist[i * 3 + 2] = 0;
    }

    in.numberofholes = 1;
    in.holelist = new REAL[3];
    in.holelist[0] = 0; in.holelist[1] = 0; in.holelist[2] = 0;

    in.numberoffacets = 1;
    in.facetlist = new tetgenio::facet[1];
    tetgenio::init(&in.facetlist[0]);
    in.facetlist[0].numberofpolygons = 1;
    in.facetlist[0].polygonlist = new tetgenio::polygon[1];
    tetgenio::init(&in.facetlist[0].polygonlist[0]);
    in.facetlist[0].polygonlist[0].numberofvertices = 20;
    in.facetlist[0].polygonlist[0].vertexlist = new int[20];
    for (int i = 0; i < 20; i++) in.facetlist[0].polygonlist[0].vertexlist[i] = i;

    tetrahedralize((char*)"Qp", &in, &out);
    CHECK(out.numberoftetrahedra > 0);
}

TEST_CASE("Quality Mesh", "[tetgen][quality]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberoftetrahedra > 0);
}

TEST_CASE("Quality with Ratio", "[tetgen][quality]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Qq1.3", &in, &out);
    CHECK(out.numberoftetrahedra > 0);
}

TEST_CASE("Volume Constraint", "[tetgen][volume]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Qa0.01", &in, &out);
    CHECK(out.numberoftetrahedra > 0);
}

TEST_CASE("Refine Mesh", "[tetgen][refine]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Q", &in, &out);
    int initial_tets = out.numberoftetrahedra;
    REQUIRE(initial_tets > 0);

    tetgenio refine_in, refine_out;
    refine_in.numberofpoints = out.numberofpoints;
    refine_in.pointlist = new REAL[out.numberofpoints * 3];
    std::copy(out.pointlist, out.pointlist + out.numberofpoints * 3, refine_in.pointlist);
    refine_in.numberoftetrahedra = out.numberoftetrahedra;
    refine_in.tetrahedronlist = new int[out.numberoftetrahedra * 4];
    std::copy(out.tetrahedronlist, out.tetrahedronlist + out.numberoftetrahedra * 4, refine_in.tetrahedronlist);

    tetrahedralize((char*)"Qr", &refine_in, &refine_out);
    CHECK(refine_out.numberoftetrahedra > initial_tets);
}

TEST_CASE("Boundary Output", "[tetgen][output]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberoffaces > 0);
}

TEST_CASE("Neighbor Output", "[tetgen][output]") {
    tetgenio in, out;
    in.numberofpoints = 8;
    in.pointlist = new REAL[24];
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    std::copy(coords, coords + 24, in.pointlist);
    tetrahedralize((char*)"Qn", &in, &out);
    CHECK(out.numberoftetrahedra > 0);
    if (out.numberofneighbors > 0) {
        CHECK(out.numberofneighbors == out.numberoftetrahedra);
    }
}

TEST_CASE("TetMeshGenerator Basic", "[wrapper][basic]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.addFacet({0, 1, 2, 3});
    gen.addFacet({4, 5, 6, 7});
    gen.addFacet({0, 1, 5, 4});
    gen.addFacet({1, 2, 6, 5});
    gen.addFacet({2, 3, 7, 6});
    gen.addFacet({3, 0, 4, 7});
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 8);
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Points Only", "[wrapper][basic]") {
    TetMeshGenerator gen;
    for (int i = 0; i < 10; i++) {
        gen.addPoint(i * 0.1, i * 0.1, i * 0.1);
    }
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 10);
}

TEST_CASE("TetMeshGenerator Quality Settings", "[wrapper][quality]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setQuality(1.5, 10.0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Max Volume", "[wrapper][volume]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setMaxVolume(0.01);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Isotropic", "[wrapper][isotropic]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.addFacet({0,1,2,3});
    gen.addFacet({4,5,6,7});
    gen.addFacet({0,1,5,4});
    gen.addFacet({1,2,6,5});
    gen.addFacet({2,3,7,6});
    gen.addFacet({3,0,4,7});
    gen.setPLC(true);
    gen.setIsotropic(0.3);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() > 8);
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Convex Hull", "[wrapper][hull]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.addPoint(0.5, 0.5, 0.5);
    gen.setConvexHull(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 8);
}

TEST_CASE("TetMeshGenerator Holes", "[wrapper][holes]") {
    TetMeshGenerator gen;
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0);
        gen.addPoint(cos(angle), sin(angle), 1);
    }
    gen.addHole(0.1, 0.1, 0.5);
    gen.addHole(-0.1, -0.1, 0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Custom Switches", "[wrapper][advanced]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setCustomSwitches("Qq1.2a0.05");
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetMeshGenerator Zero Index", "[wrapper][advanced]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setZeroIndex(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    bool has_zero = false;
    for (const auto& t : mesh.tetrahedra) {
        for (int i = 0; i < 4; i++) {
            if (t.v[i] == 0) has_zero = true;
        }
    }
    CHECK(has_zero);
}

TEST_CASE("TetMeshGenerator Verbose", "[wrapper][advanced]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setVerbose(0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() == 1);
}

TEST_CASE("MeshOptimizer Laplacian Smoothing", "[optimizer][smoothing]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);
    REQUIRE(mesh.tetrahedra.size() > 0);

    size_t original_count = mesh.points.size();
    auto isFixed = [](const TetMeshGenerator::Point& p) {
        const double eps = 1e-6;
        return (p.x < eps || p.x > 1.0 - eps ||
                p.y < eps || p.y > 1.0 - eps ||
                p.z < eps || p.z > 1.0 - eps);
    };

    MeshOptimizer::smooth(mesh.points, mesh.tetrahedra, 2, isFixed);
    CHECK(mesh.points.size() == original_count);
}

TEST_CASE("MeshOptimizer ODT Relaxation", "[optimizer][odt]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);
    REQUIRE(mesh.tetrahedra.size() > 0);

    size_t original_count = mesh.points.size();
    auto isFixed = [](const TetMeshGenerator::Point& p) {
        const double eps = 1e-6;
        return (p.x < eps || p.x > 1.0 - eps ||
                p.y < eps || p.y > 1.0 - eps ||
                p.z < eps || p.z > 1.0 - eps);
    };

    MeshOptimizer::relaxODT(mesh.points, mesh.tetrahedra, 2, isFixed);
    CHECK(mesh.points.size() == original_count);
}

TEST_CASE("MeshOptimizer No Fixed Points", "[optimizer][smoothing]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);

    size_t original_count = mesh.points.size();
    MeshOptimizer::smooth(mesh.points, mesh.tetrahedra, 1, nullptr);
    CHECK(mesh.points.size() == original_count);
}

TEST_CASE("MeshOptimizer Zero Iterations", "[optimizer][smoothing]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);

    std::vector<TetMeshGenerator::Point> original_points = mesh.points;
    MeshOptimizer::smooth(mesh.points, mesh.tetrahedra, 0, nullptr);

    CHECK(mesh.points.size() == original_points.size());
    for (size_t i = 0; i < mesh.points.size(); i++) {
        CHECK(mesh.points[i].x == original_points[i].x);
        CHECK(mesh.points[i].y == original_points[i].y);
        CHECK(mesh.points[i].z == original_points[i].z);
    }
}

TEST_CASE("Point Validity Check", "[geometry]") {
    tetgenio in, out;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    tetrahedralize((char*)"Q", &in, &out);

    for (int i = 0; i < out.numberofpoints; i++) {
        CHECK(std::isfinite(out.pointlist[i * 3]));
        CHECK(std::isfinite(out.pointlist[i * 3 + 1]));
        CHECK(std::isfinite(out.pointlist[i * 3 + 2]));
    }
}

TEST_CASE("Tetrahedron Validity", "[geometry]") {
    tetgenio in, out;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;
    tetrahedralize((char*)"Q", &in, &out);
    REQUIRE(out.numberoftetrahedra > 0);

    for (int i = 0; i < out.numberoftetrahedra; i++) {
        for (int j = 0; j < 4; j++) {
            int v = out.tetrahedronlist[i * 4 + j];
            CHECK(v >= 0);
            CHECK(v < out.numberofpoints);
        }
    }
}

TEST_CASE("Mesh Connectivity", "[geometry]") {
    TetMeshGenerator gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setIsotropic(0.4);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.tetrahedra.size() > 0);

    for (const auto& t : mesh.tetrahedra) {
        std::set<int> vertices(t.v, t.v + 4);
        CHECK(vertices.size() == 4);
        for (int v : vertices) {
            CHECK(v >= 0);
            CHECK(v < static_cast<int>(mesh.points.size()));
        }
    }
}

TEST_CASE("Large Point Set", "[performance]") {
    tetgenio in, out;
    in.numberofpoints = 100;
    in.pointlist = new REAL[300];
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < 300; i++) {
        in.pointlist[i] = dist(rng);
    }
    tetrahedralize((char*)"Q", &in, &out);
    CHECK(out.numberofpoints >= 100);
    CHECK(out.numberoftetrahedra > 0);
}

TEST_CASE("Multiple Holes", "[tetgen][holes]") {
    TetMeshGenerator gen;
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0);
        gen.addPoint(cos(angle), sin(angle), 1);
    }
    gen.addHole(0.2, 0.0, 0.5);
    gen.addHole(-0.2, 0.0, 0.5);
    gen.addHole(0.0, 0.2, 0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Weighted Tetrahedralization", "[tetgen][weighted]") {
    tetgenio in, out;
    in.numberofpoints = 4;
    in.pointlist = new REAL[12];
    in.pointlist[0] = 0; in.pointlist[1] = 0; in.pointlist[2] = 0;
    in.pointlist[3] = 1; in.pointlist[4] = 0; in.pointlist[5] = 0;
    in.pointlist[6] = 0; in.pointlist[7] = 1; in.pointlist[8] = 0;
    in.pointlist[9] = 0; in.pointlist[10] = 0; in.pointlist[11] = 1;

    in.numberofpointsets = 1;
    in.pointsetlist = new REAL[4];
    in.pointsetlist[0] = 0.1;
    in.pointsetlist[1] = 0.1;
    in.pointsetlist[2] = 0.1;
    in.pointsetlist[3] = 0.1;

    tetrahedralize((char*)"Qw", &in, &out);
    CHECK(out.numberofpoints >= 4);
}

TEST_CASE("Mesh Preserves Input Points", "[geometry]") {
    TetMeshGenerator gen;
    std::vector<std::array<double, 3>> input_points = {
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
    };
    for (const auto& p : input_points) {
        gen.addPoint(p[0], p[1], p[2]);
    }
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();

    bool found_all_original = true;
    for (const auto& input : input_points) {
        bool found = false;
        for (const auto& mp : mesh.points) {
            if (std::abs(mp.x - input[0]) < 1e-6 &&
                std::abs(mp.y - input[1]) < 1e-6 &&
                std::abs(mp.z - input[2]) < 1e-6) {
                found = true;
                break;
            }
        }
        if (!found) found_all_original = false;
    }
    CHECK(found_all_original);
}