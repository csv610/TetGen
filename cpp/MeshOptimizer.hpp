#pragma once

#include "Mesh.h"
#include <vector>
#include <functional>

/**
 * @brief Specialized class for tetrahedral mesh optimization and smoothing.
 */
class MeshOptimizer {
public:
    /// Returns true if the point should remain fixed (not moved).
    using FixedPredicate = std::function<bool(const Mesh::Point&)>;

    static void smooth(std::vector<Mesh::Point>& points, 
                      const std::vector<Mesh::Tetrahedron>& tetrahedra, 
                      int iterations = 1,
                      FixedPredicate isFixed = nullptr);

    static void relaxODT(std::vector<Mesh::Point>& points, 
                        const std::vector<Mesh::Tetrahedron>& tetrahedra, 
                        int iterations = 1,
                        FixedPredicate isFixed = nullptr);
};
