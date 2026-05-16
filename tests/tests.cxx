#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <vector>
#include <set>
#include <cmath>
#include <random>
#include "TetDelMesher.h"
#include "MeshOptimizer.hpp"

TEST_CASE("Tetrahedron Basic", "[tetgen][basic]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() == 4);
    CHECK(mesh.tetrahedra.size() == 1);
}

TEST_CASE("Cube Tetrahedralization", "[tetgen][basic]") {
    TetDelMesher gen;
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1
    };
    for (int i = 0; i < 8; ++i) {
        gen.addPoint(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]);
    }
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() == 8);
    CHECK(mesh.tetrahedra.size() >= 5);
}

TEST_CASE("Empty Point List", "[tetgen][edge]") {
    TetDelMesher gen;
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() == 0);
}

TEST_CASE("Single Point", "[tetgen][edge]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    // TetGen might throw or return partial result. TetDelMesher catches and returns empty mesh or partial.
    // Based on TetDelMesher implementation, it returns whatever 'out' has.
    CHECK(mesh.points.size() <= 1);
}

TEST_CASE("Two Points", "[tetgen][edge]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() <= 2);
}

TEST_CASE("Three Points (Collinear)", "[tetgen][edge]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(2, 0, 0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() <= 3);
}

TEST_CASE("Four Points (Tetrahedron)", "[tetgen][basic]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() == 1);
    REQUIRE(mesh.tetrahedra.size() > 0);
    for (const auto& tet : mesh.tetrahedra) {
        for (int j = 0; j < 4; j++) {
            CHECK((tet.v[j] >= 0 && tet.v[j] < static_cast<int>(mesh.points.size())));
        }
    }
}

TEST_CASE("Convex Hull Simple", "[tetgen][convex_hull]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(0.1, 0.1, 0.1);
    gen.setConvexHull(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.trifaces.size() >= 4);
}

TEST_CASE("Convex Hull Cube", "[tetgen][convex_hull]") {
    TetDelMesher gen;
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1,
        0.5,0.5,0.5
    };
    for (int i = 0; i < 9; ++i) {
        gen.addPoint(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]);
    }
    gen.setConvexHull(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    std::set<int> hull_points;
    for (const auto& tri : mesh.trifaces) {
        hull_points.insert(tri.v[0]);
        hull_points.insert(tri.v[1]);
        hull_points.insert(tri.v[2]);
    }
    CHECK(hull_points.size() == 8);
}

TEST_CASE("Voronoi Basic", "[tetgen][voronoi]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setVoronoi(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.vpoints.size() > 0);
    CHECK(mesh.vedges.size() > 0);
    CHECK(mesh.vfacets.size() > 0);
    CHECK(mesh.vcells.size() > 0);
}

TEST_CASE("Voronoi Multiple Points", "[tetgen][voronoi]") {
    TetDelMesher gen;
    for (int i = 0; i < 8; i++) {
        gen.addPoint((i & 1) ? 1.0 : 0.0, (i & 2) ? 1.0 : 0.0, (i & 4) ? 1.0 : 0.0);
    }
    gen.setVoronoi(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.vcells.size() == 8);
}

TEST_CASE("PLC Cube", "[tetgen][plc]") {
    TetDelMesher gen;
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1
    };
    for (int i = 0; i < 8; ++i) {
        gen.addPoint(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2], 0);
    }

    gen.addFacet({0, 1, 2, 3}, 1);
    gen.addFacet({4, 5, 6, 7}, 1);
    gen.addFacet({0, 1, 5, 4}, 1);
    gen.addFacet({1, 2, 6, 5}, 1);
    gen.addFacet({2, 3, 7, 6}, 1);
    gen.addFacet({3, 0, 4, 7}, 1);

    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 8);
    CHECK(mesh.tetrahedra.size() >= 5);
}

TEST_CASE("PLC Cube (formerly Hole)", "[tetgen][plc]") {
    TetDelMesher gen;
    double coords[] = {
        0,0,0, 1,0,0, 1,1,0, 0,1,0,
        0,0,1, 1,0,1, 1,1,1, 0,1,1
    };
    for (int i = 0; i < 8; ++i) {
        gen.addPoint(coords[i * 3], coords[i * 3 + 1], coords[i * 3 + 2]);
    }

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
    CHECK(mesh.tetrahedra.size() >= 5);
}

TEST_CASE("Quality Mesh", "[tetgen][quality]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Quality with Ratio", "[tetgen][quality]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);
    
    gen.addFacet({0,1,2,3});
    gen.addFacet({4,5,6,7});
    gen.addFacet({0,1,5,4});
    gen.addFacet({1,2,6,5});
    gen.addFacet({2,3,7,6});
    gen.addFacet({3,0,4,7});

    gen.setQuality(1.3);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Volume Constraint", "[tetgen][volume]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);

    gen.addFacet({0,1,2,3});
    gen.addFacet({4,5,6,7});
    gen.addFacet({0,1,5,4});
    gen.addFacet({1,2,6,5});
    gen.addFacet({2,3,7,6});
    gen.addFacet({3,0,4,7});

    gen.setQuality();
    gen.setMaxVolume(0.01);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Refine Mesh", "[tetgen][refine]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    int initial_tets = mesh.tetrahedra.size();
    REQUIRE(initial_tets > 0);

    TetDelMesher refiner;
    refiner.refine(mesh);
    refiner.setCustomSwitches("Qra0.001");
    refiner.setQuiet(true);
    auto refine_out = refiner.generate();
    CHECK(refine_out.tetrahedra.size() > initial_tets);
}

TEST_CASE("Boundary Output", "[tetgen][output]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);
    gen.setCustomSwitches("Qf");
    auto mesh = gen.generate();
    CHECK(mesh.trifaces.size() > 0);
}

TEST_CASE("Neighbor Output", "[tetgen][output]") {
    TetDelMesher gen;
    double coords[] = {0,0,0, 1,0,0, 1,1,0, 0,1,0, 0,0,1, 1,0,1, 1,1,1, 0,1,1};
    for (int i = 0; i < 8; ++i) gen.addPoint(coords[i*3], coords[i*3+1], coords[i*3+2]);
    gen.setCustomSwitches("Qn");
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
    CHECK(!mesh.neighbors.empty());
}

TEST_CASE("TetDelMesher Basic", "[wrapper][basic]") {
    TetDelMesher gen;
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

TEST_CASE("TetDelMesher Points Only", "[wrapper][basic]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(0.5, 0.2, 0.3);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 5);
}

TEST_CASE("TetDelMesher Quality Settings", "[wrapper][quality]") {
    TetDelMesher gen;
    // Use 8 points (cube) to be safer for quality constraints
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    gen.setQuality(1.5, 10.0);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetDelMesher Max Volume", "[wrapper][volume]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setMaxVolume(0.01);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetDelMesher Isotropic", "[wrapper][isotropic]") {
    TetDelMesher gen;
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

TEST_CASE("TetDelMesher Convex Hull", "[wrapper][hull]") {
    TetDelMesher gen;
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

TEST_CASE("TetDelMesher Holes", "[wrapper][holes]") {
    TetDelMesher gen;
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0);
        gen.addPoint(cos(angle), sin(angle), 1);
    }
    // Add points for internal facet at z=0.5
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0.5);
    }
    gen.addFacet({0, 2, 4, 6}); // bottom
    gen.addFacet({1, 3, 5, 7}); // top
    gen.addFacet({8, 9, 10, 11}); // internal
    
    // Split sides
    gen.addFacet({0, 8, 9, 2}); gen.addFacet({8, 1, 3, 9});
    gen.addFacet({2, 9, 10, 4}); gen.addFacet({9, 3, 5, 10});
    gen.addFacet({4, 10, 11, 6}); gen.addFacet({10, 5, 7, 11});
    gen.addFacet({6, 11, 8, 0}); gen.addFacet({11, 7, 1, 8});

    gen.addHole(0.1, 0.1, 0.25);
    gen.addHole(-0.1, -0.1, 0.25);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetDelMesher Custom Switches", "[wrapper][advanced]") {
    TetDelMesher gen;
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
    gen.setCustomSwitches("Qpq1.2a0.05");
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("TetDelMesher Zero Index", "[wrapper][advanced]") {
    TetDelMesher gen;
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

TEST_CASE("TetDelMesher Verbose", "[wrapper][advanced]") {
    TetDelMesher gen;
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
    TetDelMesher gen;
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
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);
    REQUIRE(mesh.tetrahedra.size() > 0);

    size_t original_count = mesh.points.size();
    auto isFixed = [](const Mesh::Point& p) {
        const double eps = 1e-6;
        return (p.x < eps || p.x > 1.0 - eps ||
                p.y < eps || p.y > 1.0 - eps ||
                p.z < eps || p.z > 1.0 - eps);
    };

    MeshOptimizer::smooth(mesh.points, mesh.tetrahedra, 2, isFixed);
    CHECK(mesh.points.size() == original_count);
}

TEST_CASE("MeshOptimizer ODT Relaxation", "[optimizer][odt]") {
    TetDelMesher gen;
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
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);
    REQUIRE(mesh.tetrahedra.size() > 0);

    size_t original_count = mesh.points.size();
    auto isFixed = [](const Mesh::Point& p) {
        const double eps = 1e-6;
        return (p.x < eps || p.x > 1.0 - eps ||
                p.y < eps || p.y > 1.0 - eps ||
                p.z < eps || p.z > 1.0 - eps);
    };

    MeshOptimizer::relaxODT(mesh.points, mesh.tetrahedra, 2, isFixed);
    CHECK(mesh.points.size() == original_count);
}

TEST_CASE("MeshOptimizer No Fixed Points", "[optimizer][smoothing]") {
    TetDelMesher gen;
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
    TetDelMesher gen;
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
    gen.setIsotropic(0.5);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.points.size() > 8);

    std::vector<Mesh::Point> original_points = mesh.points;
    MeshOptimizer::smooth(mesh.points, mesh.tetrahedra, 0, nullptr);

    CHECK(mesh.points.size() == original_points.size());
    for (size_t i = 0; i < mesh.points.size(); i++) {
        CHECK(mesh.points[i].x == original_points[i].x);
        CHECK(mesh.points[i].y == original_points[i].y);
        CHECK(mesh.points[i].z == original_points[i].z);
    }
}

TEST_CASE("Point Validity Check", "[geometry]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setQuiet(true);
    auto mesh = gen.generate();

    for (const auto& p : mesh.points) {
        CHECK(std::isfinite(p.x));
        CHECK(std::isfinite(p.y));
        CHECK(std::isfinite(p.z));
    }
}

TEST_CASE("Tetrahedron Validity", "[geometry]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    REQUIRE(mesh.tetrahedra.size() > 0);

    for (const auto& tet : mesh.tetrahedra) {
        for (int j = 0; j < 4; j++) {
            int v = tet.v[j];
            CHECK(v >= 0);
            CHECK(v < static_cast<int>(mesh.points.size()));
        }
    }
}

TEST_CASE("Mesh Connectivity", "[geometry]") {
    TetDelMesher gen;
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
    TetDelMesher gen;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < 100; i++) {
        gen.addPoint(dist(rng), dist(rng), dist(rng));
    }
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 100);
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Multiple Holes", "[tetgen][holes]") {
    TetDelMesher gen;
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0);
        gen.addPoint(cos(angle), sin(angle), 1);
    }
    // Add points for internal facet at z=0.5
    for (int i = 0; i < 4; i++) {
        double angle = i * M_PI / 2;
        gen.addPoint(cos(angle), sin(angle), 0.5);
    }
    gen.addFacet({0, 2, 4, 6});
    gen.addFacet({1, 3, 5, 7});
    gen.addFacet({8, 9, 10, 11}); // internal
    
    // Split sides
    gen.addFacet({0, 8, 9, 2}); gen.addFacet({8, 1, 3, 9});
    gen.addFacet({2, 9, 10, 4}); gen.addFacet({9, 3, 5, 10});
    gen.addFacet({4, 10, 11, 6}); gen.addFacet({10, 5, 7, 11});
    gen.addFacet({6, 11, 8, 0}); gen.addFacet({11, 7, 1, 8});

    gen.addHole(0.2, 0.0, 0.25);
    gen.addHole(-0.2, 0.0, 0.25);
    gen.addHole(0.0, 0.2, 0.25);
    gen.setPLC(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.tetrahedra.size() > 0);
}

TEST_CASE("Weighted Tetrahedralization", "[tetgen][weighted]") {
    TetDelMesher gen;
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);

    for (int i = 0; i < 4; ++i) gen.addPointAttribute(i, 0.1);

    gen.setWeighted(true);
    gen.setQuiet(true);
    auto mesh = gen.generate();
    CHECK(mesh.points.size() >= 4);
}

TEST_CASE("Mesh Preserves Input Points", "[geometry]") {
    TetDelMesher gen;
    std::vector<std::array<double, 3>> input_points = {
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
    };
    for (const auto& p : input_points) {
        gen.addPoint(p[0], p[1], p[2]);
    }
    gen.addFacet({0, 1, 2, 3});
    gen.addFacet({4, 5, 6, 7});
    gen.addFacet({0, 1, 5, 4});
    gen.addFacet({1, 2, 6, 5});
    gen.addFacet({2, 3, 7, 6});
    gen.addFacet({3, 0, 4, 7});
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
