#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

/**
 * @brief Represents a 3D tetrahedral mesh and associated data.
 */
class Mesh {
public:
    struct Point {
        double x, y, z;
    };

    struct Tetrahedron {
        int v[4];
    };

    struct Triangle {
        int v[3];
    };

    struct VoronoiEdge {
        int v1, v2;
    };

    struct VoronoiFacet {
        std::vector<int> vertices;
    };

    std::vector<Point> points;
    std::vector<int> pointMarkers;
    std::vector<Tetrahedron> tetrahedra;
    std::vector<int> neighbors;
    std::vector<Triangle> trifaces;
    std::vector<int> trifaceMarkers;
    
    // Voronoi
    std::vector<Point> vpoints;
    std::vector<VoronoiEdge> vedges;
    std::vector<VoronoiFacet> vfacets;
    std::vector<std::vector<int>> vcells;

    /**
     * @brief Saves the mesh to files.
     * Supported formats: TetGen (.node, .ele, .face, etc.)
     * @param filename Base filename (without extension)
     */
    void save(const std::string& filename) const;
};

#endif // MESH_H
