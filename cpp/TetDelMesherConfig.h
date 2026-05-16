#ifndef TET_DEL_MESHER_CONFIG_H
#define TET_DEL_MESHER_CONFIG_H

#include <string>

/**
 * @brief Configuration settings for the TetDelMesher.
 */
struct TetDelMesherConfig {
    bool quiet = true;            // -Q
    
    // Quality settings (-q)
    bool useQuality = false;
    double radiusEdgeRatio = 2.0;
    double minDihedralAngle = 0.0;
    
    // Volume settings (-a)
    bool useMaxVolume = false;
    double maxVolume = 0.0;
    
    bool isPLC = true;           // -p
    bool preserveSurface = false; // -Y
    bool refineMesh = false;     // -r
    bool coarsenMesh = false;    // -R
    bool weighted = false;        // -w
    bool convexHull = false;     // -c
    bool detectSelfIntersections = false; // -d
    bool zeroIndex = true;        // -z
    bool check = false;           // -C
    int verbose = 0;              // -V
    bool voronoi = false;         // -v

    // Optimization and Precision
    int optimizationLevel = 3;    // -O (maps to opt_max_flip_level)
    double tolerance = 1e-8;      // -T
    bool useExactArithmetic = true; // maps to NOT -X
    bool mergeCoplanar = true;    // maps to NOT -M

    // Output suppression/formatting
    bool outputFaces = false;     // -f
    bool outputEdges = false;     // -e
    bool outputNeighbors = false; // -n
    bool outputVtk = false;       // -k
    bool outputMedit = false;     // -g
    bool suppressBoundaryInfo = false; // -B
    bool suppressNodeFile = false;     // -N
    bool suppressElementFile = false;  // -E
    bool suppressIterationNumbers = true; // -I
    
    std::string customSwitches = "";

    /**
     * @brief Creates a configuration for an isotropic mesh.
     * @param edgeLength Desired edge length.
     */
    static TetDelMesherConfig Isotropic(double edgeLength) {
        TetDelMesherConfig config;
        config.isPLC = true;
        config.quiet = true;
        config.useQuality = true;
        config.radiusEdgeRatio = 1.414;
        config.useMaxVolume = true;
        // Volume of a regular tetrahedron with side length L: V = L^3 / (6 * sqrt(2))
        config.maxVolume = (edgeLength * edgeLength * edgeLength) / (6.0 * 1.4142135623730951);
        return config;
    }
};

#endif // TET_DEL_MESHER_CONFIG_H
