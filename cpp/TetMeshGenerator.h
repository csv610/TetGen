#ifndef TET_MESH_GENERATOR_H
#define TET_MESH_GENERATOR_H

#include <vector>
#include <string>
#include <memory>

/**
 * @brief A high-level wrapper for the TetGen library.
 * 
 * Hides the complexity of tetgenio and tetgenbehavior structures.
 */
class TetMeshGenerator {
public:
    struct Point {
        double x, y, z;
    };

    struct Tetrahedron {
        int v[4];
    };

    struct Mesh {
        std::vector<Point> points;
        std::vector<Tetrahedron> tetrahedra;
    };

    TetMeshGenerator();
    ~TetMeshGenerator();

    // Geometry Input
    void addPoint(double x, double y, double z);
    void addFacet(const std::vector<int>& vertexIndices);
    void addHole(double x, double y, double z);

    // Configuration
    void setQuiet(bool quiet);
    void setQuality(double radiusEdgeRatio = 2.0, double minDihedralAngle = 0.0);
    void setMaxVolume(double volume);
    void setPLC(bool isPLC);
    void setConvexHull(bool enable);
    void setWeighted(bool enable);
    void setZeroIndex(bool enable);
    void setCheck(bool enable);
    void setVerbose(int level);
    
    /**
     * @brief Sets constraints for an isotropic mesh based on desired edge length.
     * 
     * Calculates the maximum volume based on the edge length of a regular tetrahedron
     * and sets a quality bound (radius-edge ratio) to 1.414.
     */
    void setIsotropic(double edgeLength);

    /**
     * @brief Allows setting any TetGen switch directly via a string.
     * Example: "pq1.414a.1" for quality and volume constraints.
     */
    void setCustomSwitches(const std::string& switches);

    // Generation
    Mesh generate();

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // TET_MESH_GENERATOR_H
