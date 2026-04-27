#include "TetMeshGenerator.h"
#include <iostream>
#include <cassert>

int main() {
    TetMeshGenerator gen;
    
    // Create a simple box
    gen.addPoint(0, 0, 0);
    gen.addPoint(1, 0, 0);
    gen.addPoint(1, 1, 0);
    gen.addPoint(0, 1, 0);
    gen.addPoint(0, 0, 1);
    gen.addPoint(1, 0, 1);
    gen.addPoint(1, 1, 1);
    gen.addPoint(0, 1, 1);
    
    // Add facets for the box
    gen.addFacet({0, 1, 2, 3}); // Bottom
    gen.addFacet({4, 5, 6, 7}); // Top
    gen.addFacet({0, 1, 5, 4}); // Front
    gen.addFacet({2, 3, 7, 6}); // Back
    gen.addFacet({0, 3, 7, 4}); // Left
    gen.addFacet({1, 2, 6, 5}); // Right
    
    gen.setPLC(true);
    gen.setIsotropic(0.2); // Small edge length for isotropic mesh
    
    TetMeshGenerator::Mesh mesh = gen.generate();
    
    std::cout << "Points: " << mesh.points.size() << std::endl;
    std::cout << "Tetrahedra: " << mesh.tetrahedra.size() << std::endl;
    
    assert(mesh.points.size() > 8);
    assert(mesh.tetrahedra.size() > 0);
    
    std::cout << "Isotropic mesh generation test passed!" << std::endl;
    
    return 0;
}
