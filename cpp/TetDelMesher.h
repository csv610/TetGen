#ifndef TET_DEL_MESHER_H
#define TET_DEL_MESHER_H

#include <vector>
#include <string>
#include <memory>
#include "Mesh.h"

/**
 * @brief A high-level wrapper for the TetGen library.
 * 
 * Hides the complexity of tetgenio and tetgenbehavior structures.
 */
class TetDelMesher {
public:
    TetDelMesher();
    ~TetDelMesher();

    // Geometry Input
    void addPoint(double x, double y, double z, int marker = 0);
    void addFacet(const std::vector<int>& vertexIndices, int marker = 0);
    void addHole(double x, double y, double z);
    
    // Add point attributes (weighted points)
    void addPointAttribute(int pointIdx, double value);

    /**
     * @brief Loads an existing mesh for refinement.
     */
    void refine(const Mesh& mesh);

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
    void setVoronoi(bool enable);
    
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

    /**
     * @brief Generates a Delaunay tetrahedral mesh from a surface mesh without inserting new vertices in the interior.
     * 
     * The surface mesh may be subdivided (Steiner points added on boundaries) to ensure a 
     * valid constrained Delaunay tetrahedralization.
     * 
     * @param surfaceMesh The input surface mesh (points and trifaces).
     * @return A tetrahedral mesh.
     */
    Mesh generateNoInterior(const Mesh& surfaceMesh);

private:
    struct Impl;
    std::unique_ptr<Impl> impl;
};

#endif // TET_DEL_MESHER_H
