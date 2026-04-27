#pragma once

#include "TetMeshGenerator.h"
#include <vector>
#include <functional>

/**
 * @brief Specialized class for tetrahedral mesh optimization and smoothing.
 */
class MeshOptimizer {
public:
    /// Returns true if the point should remain fixed (not moved).
    using FixedPredicate = std::function<bool(const TetMeshGenerator::Point&)>;

    static void smooth(std::vector<TetMeshGenerator::Point>& points, 
                      const std::vector<TetMeshGenerator::Tetrahedron>& tetrahedra, 
                      int iterations = 1,
                      FixedPredicate isFixed = nullptr);

    static void relaxODT(std::vector<TetMeshGenerator::Point>& points, 
                        const std::vector<TetMeshGenerator::Tetrahedron>& tetrahedra, 
                        int iterations = 1,
                        FixedPredicate isFixed = nullptr);
};
