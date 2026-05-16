#include "Mesh.h"
#include "tetgen.h"
#include <iostream>
#include <algorithm>

void Mesh::save(const std::string& filename) const {
    tetgenio out;

    // Points
    out.numberofpoints = points.size();
    if (out.numberofpoints > 0) {
        out.pointlist = new double[out.numberofpoints * 3];
        for (int i = 0; i < out.numberofpoints; ++i) {
            out.pointlist[i * 3] = points[i].x;
            out.pointlist[i * 3 + 1] = points[i].y;
            out.pointlist[i * 3 + 2] = points[i].z;
        }
        if (pointMarkers.size() == (size_t)out.numberofpoints) {
            out.pointmarkerlist = new int[out.numberofpoints];
            std::copy(pointMarkers.begin(), pointMarkers.end(), out.pointmarkerlist);
        }
    }

    // Tetrahedra
    out.numberoftetrahedra = tetrahedra.size();
    if (out.numberoftetrahedra > 0) {
        out.numberofcorners = 4;
        out.tetrahedronlist = new int[out.numberoftetrahedra * 4];
        for (int i = 0; i < out.numberoftetrahedra; ++i) {
            for (int j = 0; j < 4; ++j) {
                out.tetrahedronlist[i * 4 + j] = tetrahedra[i].v[j];
            }
        }
    }

    // Neighbors
    if (neighbors.size() == (size_t)out.numberoftetrahedra * 4) {
        out.neighborlist = new int[neighbors.size()];
        std::copy(neighbors.begin(), neighbors.end(), out.neighborlist);
    }

    // Trifaces
    out.numberoftrifaces = trifaces.size();
    if (out.numberoftrifaces > 0) {
        out.trifacelist = new int[out.numberoftrifaces * 3];
        for (int i = 0; i < out.numberoftrifaces; ++i) {
            for (int j = 0; j < 3; ++j) {
                out.trifacelist[i * 3 + j] = trifaces[i].v[j];
            }
        }
        if (trifaceMarkers.size() == (size_t)out.numberoftrifaces) {
            out.trifacemarkerlist = new int[out.numberoftrifaces];
            std::copy(trifaceMarkers.begin(), trifaceMarkers.end(), out.trifacemarkerlist);
        }
    }

    // Save using TetGen's methods
    char* fname = const_cast<char*>(filename.c_str());
    if (out.numberofpoints > 0) out.save_nodes(fname);
    if (out.numberoftetrahedra > 0) out.save_elements(fname);
    if (out.numberoftrifaces > 0) out.save_faces(fname);
    if (out.neighborlist != nullptr) out.save_neighbors(fname);
    
    // Note: Voronoi saving is more complex in TetGen and might need extra logic 
    // if we wanted to support it fully via tetgenio's native save.
    // For now, we save the primary mesh components.
}
