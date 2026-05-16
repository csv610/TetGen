#include "TetDelMesher.h"
#include "tetgen.h"
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream>

struct TetDelMesher::Impl {
    tetgenbehavior behavior;
    
    std::vector<double> points;
    std::vector<int> pointMarkers;
    std::vector<std::vector<int>> facets;
    std::vector<int> facetMarkers;
    std::vector<double> holes;
    std::vector<std::vector<double>> pointAttributes;
    std::vector<int> tetrahedra;

    Impl() {
        behavior.quiet = 1; // Default to quiet
    }
};

TetDelMesher::TetDelMesher() : impl(std::make_unique<Impl>()) {}
TetDelMesher::~TetDelMesher() = default;

void TetDelMesher::addPoint(double x, double y, double z, int marker) {
    impl->points.push_back(x);
    impl->points.push_back(y);
    impl->points.push_back(z);
    impl->pointMarkers.push_back(marker);
}

void TetDelMesher::addFacet(const std::vector<int>& vertexIndices, int marker) {
    impl->facets.push_back(vertexIndices);
    impl->facetMarkers.push_back(marker);
}

void TetDelMesher::addHole(double x, double y, double z) {
    impl->holes.push_back(x);
    impl->holes.push_back(y);
    impl->holes.push_back(z);
}

void TetDelMesher::addPointAttribute(int pointIdx, double value) {
    if (pointIdx >= (int)impl->pointAttributes.size()) {
        impl->pointAttributes.resize(pointIdx + 1);
    }
    impl->pointAttributes[pointIdx].push_back(value);
}

void TetDelMesher::refine(const Mesh& mesh) {
    impl->points.clear();
    impl->pointMarkers.clear();
    for (const auto& p : mesh.points) {
        impl->points.push_back(p.x);
        impl->points.push_back(p.y);
        impl->points.push_back(p.z);
    }
    if (mesh.pointMarkers.size() == mesh.points.size()) {
        impl->pointMarkers = mesh.pointMarkers;
    } else {
        impl->pointMarkers.assign(mesh.points.size(), 0);
    }

    impl->tetrahedra.clear();
    for (const auto& t : mesh.tetrahedra) {
        for (int i = 0; i < 4; ++i) {
            impl->tetrahedra.push_back(t.v[i]);
        }
    }
}

void TetDelMesher::setQuiet(bool quiet) {
    impl->behavior.quiet = quiet ? 1 : 0;
}

void TetDelMesher::setQuality(double radiusEdgeRatio, double minDihedralAngle) {
    impl->behavior.quality = 1;
    impl->behavior.minratio = radiusEdgeRatio;
    impl->behavior.mindihedral = minDihedralAngle;
}

void TetDelMesher::setMaxVolume(double volume) {
    impl->behavior.fixedvolume = 1;
    impl->behavior.maxvolume = volume;
}

void TetDelMesher::setPLC(bool isPLC) {
    impl->behavior.plc = isPLC ? 1 : 0;
}

void TetDelMesher::setConvexHull(bool enable) {
    impl->behavior.convex = enable ? 1 : 0;
}

void TetDelMesher::setWeighted(bool enable) {
    impl->behavior.weighted = enable ? 1 : 0;
}

void TetDelMesher::setZeroIndex(bool enable) {
    impl->behavior.zeroindex = enable ? 1 : 0;
}

void TetDelMesher::setCheck(bool enable) {
    impl->behavior.docheck = enable ? 1 : 0;
}

void TetDelMesher::setVerbose(int level) {
    impl->behavior.verbose = level;
}

void TetDelMesher::setVoronoi(bool enable) {
    impl->behavior.voroout = enable ? 1 : 0;
}

void TetDelMesher::setIsotropic(double edgeLength) {
    impl->behavior.quality = 1;
    impl->behavior.minratio = 1.414;
    impl->behavior.fixedvolume = 1;
    // Volume of a regular tetrahedron with side length L: V = L^3 / (6 * sqrt(2))
    impl->behavior.maxvolume = (edgeLength * edgeLength * edgeLength) / (6.0 * std::sqrt(2.0));
}

void TetDelMesher::setCustomSwitches(const std::string& switches) {
    char* s = const_cast<char*>(switches.c_str());
    impl->behavior.parse_commandline(s);
}

Mesh TetDelMesher::generate() {
    tetgenio in, out;

    // Prepare input points
    in.numberofpoints = impl->points.size() / 3;
    if (in.numberofpoints > 0) {
        in.pointlist = new double[impl->points.size()];
        std::copy(impl->points.begin(), impl->points.end(), in.pointlist);
        
        bool has_nonzero_marker = false;
        for (int m : impl->pointMarkers) if (m != 0) has_nonzero_marker = true;
        
        if (has_nonzero_marker) {
            in.pointmarkerlist = new int[in.numberofpoints];
            std::copy(impl->pointMarkers.begin(), impl->pointMarkers.end(), in.pointmarkerlist);
        }
    }

    // Prepare input tetrahedra (for refinement)
    if (!impl->tetrahedra.empty()) {
        in.numberoftetrahedra = impl->tetrahedra.size() / 4;
        in.tetrahedronlist = new int[impl->tetrahedra.size()];
        std::copy(impl->tetrahedra.begin(), impl->tetrahedra.end(), in.tetrahedronlist);
    }

    // Prepare point attributes
    if (!impl->pointAttributes.empty()) {
        int max_attrs = 0;
        for (const auto& attrs : impl->pointAttributes) {
            max_attrs = std::max(max_attrs, (int)attrs.size());
        }
        if (max_attrs > 0) {
            in.numberofpointattributes = max_attrs;
            in.pointattributelist = new double[in.numberofpoints * max_attrs];
            std::fill(in.pointattributelist, in.pointattributelist + in.numberofpoints * max_attrs, 0.0);
            for (int i = 0; i < (int)impl->pointAttributes.size() && i < in.numberofpoints; ++i) {
                for (int j = 0; j < (int)impl->pointAttributes[i].size() && j < max_attrs; ++j) {
                    in.pointattributelist[i * max_attrs + j] = impl->pointAttributes[i][j];
                }
            }
        }
    }

    // Prepare input facets
    if (!impl->facets.empty()) {
        in.numberoffacets = impl->facets.size();
        in.facetlist = new tetgenio::facet[in.numberoffacets];
        in.facetmarkerlist = new int[in.numberoffacets];

        for (int i = 0; i < in.numberoffacets; ++i) {
            tetgenio::facet* f = &in.facetlist[i];
            tetgenio::init(f);
            f->numberofpolygons = 1;
            f->polygonlist = new tetgenio::polygon[1];
            tetgenio::init(&f->polygonlist[0]);

            tetgenio::polygon* p = &f->polygonlist[0];
            p->numberofvertices = impl->facets[i].size();
            p->vertexlist = new int[p->numberofvertices];
            std::copy(impl->facets[i].begin(), impl->facets[i].end(), p->vertexlist);
            
            in.facetmarkerlist[i] = impl->facetMarkers[i];
        }
    }

    // Prepare holes
    if (!impl->holes.empty()) {
        in.numberofholes = impl->holes.size() / 3;
        in.holelist = new double[impl->holes.size()];
        std::copy(impl->holes.begin(), impl->holes.end(), in.holelist);
    }

    // Run tetrahedralization
    try {
        tetgenbehavior b_copy = impl->behavior;
        tetrahedralize(&b_copy, &in, &out);
    } catch (int e) {
        if (!impl->behavior.quiet) {
            std::cerr << "TetGen exception: " << e << std::endl;
        }
        return Mesh();
    } catch (...) {
        if (!impl->behavior.quiet) {
            std::cerr << "Unknown exception in TetGen" << std::endl;
        }
        return Mesh();
    }

    // Collect results
    Mesh result;
    if (out.pointlist != nullptr) {
        for (int i = 0; i < out.numberofpoints; ++i) {
            result.points.push_back({out.pointlist[i * 3], out.pointlist[i * 3 + 1], out.pointlist[i * 3 + 2]});
        }
    }
    if (out.pointmarkerlist != nullptr) {
        for (int i = 0; i < out.numberofpoints; ++i) {
            result.pointMarkers.push_back(out.pointmarkerlist[i]);
        }
    }

    if (out.tetrahedronlist != nullptr) {
        for (int i = 0; i < out.numberoftetrahedra; ++i) {
            Mesh::Tetrahedron tet;
            for (int j = 0; j < 4; ++j) {
                tet.v[j] = out.tetrahedronlist[i * 4 + j];
            }
            result.tetrahedra.push_back(tet);
        }
    }
    
    if (out.neighborlist != nullptr) {
        for (int i = 0; i < out.numberoftetrahedra * 4; ++i) {
            result.neighbors.push_back(out.neighborlist[i]);
        }
    }

    if (out.trifacelist != nullptr) {
        for (int i = 0; i < out.numberoftrifaces; ++i) {
            Mesh::Triangle tri;
            for (int j = 0; j < 3; ++j) {
                tri.v[j] = out.trifacelist[i * 3 + j];
            }
            result.trifaces.push_back(tri);
        }
    }
    if (out.trifacemarkerlist != nullptr) {
        for (int i = 0; i < out.numberoftrifaces; ++i) {
            result.trifaceMarkers.push_back(out.trifacemarkerlist[i]);
        }
    }

    // Voronoi
    if (out.vpointlist != nullptr) {
        for (int i = 0; i < out.numberofvpoints; ++i) {
            result.vpoints.push_back({out.vpointlist[i * 3], out.vpointlist[i * 3 + 1], out.vpointlist[i * 3 + 2]});
        }
    }
    if (out.vedgelist != nullptr) {
        for (int i = 0; i < out.numberofvedges; ++i) {
            Mesh::VoronoiEdge edge;
            edge.v1 = out.vedgelist[i].v1;
            edge.v2 = out.vedgelist[i].v2;
            result.vedges.push_back(edge);
        }
    }
    if (out.vfacetlist != nullptr) {
        for (int i = 0; i < out.numberofvfacets; ++i) {
            tetgenio::vorofacet* vf = &out.vfacetlist[i];
            Mesh::VoronoiFacet facet;
            // vf->elist[0] is the number of edges
            if (vf->elist != nullptr) {
                for (int j = 0; j < vf->elist[0]; ++j) {
                    facet.vertices.push_back(vf->elist[j + 1]);
                }
            }
            result.vfacets.push_back(facet);
        }
    }
    if (out.vcelllist != nullptr) {
        for (int i = 0; i < out.numberofvcells; ++i) {
            std::vector<int> cell;
            int* c = out.vcelllist[i];
            if (c != nullptr) {
                // c[0] is the number of facets in this cell
                for (int j = 0; j < c[0]; ++j) {
                    cell.push_back(c[j + 1]);
                }
            }
            result.vcells.push_back(cell);
        }
    }

    return result;
}

Mesh TetDelMesher::generateNoInterior(const Mesh& surfaceMesh) {
    // 1. Clear current internal state
    impl->points.clear();
    impl->pointMarkers.clear();
    impl->facets.clear();
    impl->facetMarkers.clear();
    impl->holes.clear();
    impl->pointAttributes.clear();
    impl->tetrahedra.clear();

    // 2. Load points from surfaceMesh
    for (const auto& p : surfaceMesh.points) {
        addPoint(p.x, p.y, p.z);
    }
    // Load point markers if they match point count
    if (surfaceMesh.pointMarkers.size() == surfaceMesh.points.size()) {
        impl->pointMarkers = surfaceMesh.pointMarkers;
    } else {
        impl->pointMarkers.assign(surfaceMesh.points.size(), 0);
    }

    // 3. Load facets from surfaceMesh trifaces
    for (size_t i = 0; i < surfaceMesh.trifaces.size(); ++i) {
        const auto& tri = surfaceMesh.trifaces[i];
        std::vector<int> v = {tri.v[0], tri.v[1], tri.v[2]};
        int marker = 0;
        if (i < surfaceMesh.trifaceMarkers.size()) {
            marker = surfaceMesh.trifaceMarkers[i];
        }
        addFacet(v, marker);
    }

    // 4. Configure behavior for CDT without quality refinement
    tetgenbehavior b;
    b.quiet = impl->behavior.quiet;
    b.verbose = impl->behavior.verbose;
    b.plc = 1;         // -p: Piecewise Linear Complex
    b.quality = 0;     // No -q: No quality-driven Steiner points
    b.fixedvolume = 0; // No -a: No volume-driven Steiner points
    b.weighted = 0;
    b.zeroindex = impl->behavior.zeroindex;

    // Temporarily swap behavior
    tetgenbehavior oldBehavior = impl->behavior;
    impl->behavior = b;

    // 5. Generate
    Mesh result = generate();

    // 6. Restore original behavior
    impl->behavior = oldBehavior;

    return result;
}
