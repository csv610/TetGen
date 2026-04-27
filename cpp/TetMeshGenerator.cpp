#define TETLIBRARY
#include "TetMeshGenerator.h"
#include "tetgen.h"
#include <sstream>
#include <iomanip>
#include <cmath>

struct TetMeshGenerator::Impl {
    tetgenio in;
    tetgenbehavior behavior;
    
    std::vector<double> points;
    std::vector<std::vector<int>> facets;
    std::vector<double> holes;

    Impl() {
        behavior.quiet = 1; // Default to quiet
    }
};

TetMeshGenerator::TetMeshGenerator() : impl(std::make_unique<Impl>()) {}
TetMeshGenerator::~TetMeshGenerator() = default;

void TetMeshGenerator::addPoint(double x, double y, double z) {
    impl->points.push_back(x);
    impl->points.push_back(y);
    impl->points.push_back(z);
}

void TetMeshGenerator::addFacet(const std::vector<int>& vertexIndices) {
    impl->facets.push_back(vertexIndices);
}

void TetMeshGenerator::addHole(double x, double y, double z) {
    impl->holes.push_back(x);
    impl->holes.push_back(y);
    impl->holes.push_back(z);
}

void TetMeshGenerator::setQuiet(bool quiet) {
    impl->behavior.quiet = quiet ? 1 : 0;
}

void TetMeshGenerator::setQuality(double radiusEdgeRatio, double minDihedralAngle) {
    impl->behavior.quality = 1;
    impl->behavior.minratio = radiusEdgeRatio;
    impl->behavior.mindihedral = minDihedralAngle;
}

void TetMeshGenerator::setMaxVolume(double volume) {
    impl->behavior.fixedvolume = 1;
    impl->behavior.maxvolume = volume;
}

void TetMeshGenerator::setPLC(bool isPLC) {
    impl->behavior.plc = isPLC ? 1 : 0;
}

void TetMeshGenerator::setConvexHull(bool enable) {
    impl->behavior.convex = enable ? 1 : 0;
}

void TetMeshGenerator::setWeighted(bool enable) {
    impl->behavior.weighted = enable ? 1 : 0;
}

void TetMeshGenerator::setZeroIndex(bool enable) {
    impl->behavior.zeroindex = enable ? 1 : 0;
}

void TetMeshGenerator::setCheck(bool enable) {
    impl->behavior.docheck = enable ? 1 : 0;
}

void TetMeshGenerator::setVerbose(int level) {
    impl->behavior.verbose = level;
}

void TetMeshGenerator::setIsotropic(double edgeLength) {
    impl->behavior.quality = 1;
    impl->behavior.minratio = 1.414;
    impl->behavior.fixedvolume = 1;
    // Volume of a regular tetrahedron with side length L: V = L^3 / (6 * sqrt(2))
    impl->behavior.maxvolume = (edgeLength * edgeLength * edgeLength) / (6.0 * std::sqrt(2.0));
}

void TetMeshGenerator::setCustomSwitches(const std::string& switches) {
    char* s = const_cast<char*>(switches.c_str());
    impl->behavior.parse_commandline(s);
}

TetMeshGenerator::Mesh TetMeshGenerator::generate() {
    tetgenio out;

    // Prepare input points
    impl->in.numberofpoints = impl->points.size() / 3;
    impl->in.pointlist = new double[impl->points.size()];
    std::copy(impl->points.begin(), impl->points.end(), impl->in.pointlist);

    // Prepare input facets
    if (!impl->facets.empty()) {
        impl->in.numberoffacets = impl->facets.size();
        impl->in.facetlist = new tetgenio::facet[impl->in.numberoffacets];
        impl->in.facetmarkerlist = new int[impl->in.numberoffacets];

        for (int i = 0; i < impl->in.numberoffacets; ++i) {
            tetgenio::facet* f = &impl->in.facetlist[i];
            f->numberofpolygons = 1;
            f->polygonlist = new tetgenio::polygon[1];
            f->numberofholes = 0;
            f->holelist = nullptr;

            tetgenio::polygon* p = &f->polygonlist[0];
            p->numberofvertices = impl->facets[i].size();
            p->vertexlist = new int[p->numberofvertices];
            std::copy(impl->facets[i].begin(), impl->facets[i].end(), p->vertexlist);
            
            impl->in.facetmarkerlist[i] = 0;
        }
    }

    // Prepare holes
    if (!impl->holes.empty()) {
        impl->in.numberofholes = impl->holes.size() / 3;
        impl->in.holelist = new double[impl->holes.size()];
        std::copy(impl->holes.begin(), impl->holes.end(), impl->in.holelist);
    }

    // Run tetrahedralization
    tetrahedralize(&impl->behavior, &impl->in, &out);

    // Collect results
    Mesh result;
    for (int i = 0; i < out.numberofpoints; ++i) {
        result.points.push_back({out.pointlist[i * 3], out.pointlist[i * 3 + 1], out.pointlist[i * 3 + 2]});
    }

    for (int i = 0; i < out.numberoftetrahedra; ++i) {
        Tetrahedron tet;
        for (int j = 0; j < 4; ++j) {
            tet.v[j] = out.tetrahedronlist[i * 4 + j];
        }
        result.tetrahedra.push_back(tet);
    }

    return result;
}
