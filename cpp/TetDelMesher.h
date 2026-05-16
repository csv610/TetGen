#ifndef TET_DEL_MESHER_H
#define TET_DEL_MESHER_H

#include <vector>
#include <string>
#include <memory>
#include "Mesh.h"
#include "TetDelMesherConfig.h"

/**
 * @brief A high-level wrapper for the TetGen library.
 * 
 * Hides the complexity of tetgenio and tetgenbehavior structures.
 */
class TetDelMesher {
public:
    TetDelMesher();
    ~TetDelMesher();

    // Configuration
    void setConfig(const TetDelMesherConfig& config);
    TetDelMesherConfig getConfig() const;

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
