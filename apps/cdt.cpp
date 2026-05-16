#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TetDelMesher.h"

/**
 * @brief CLI application to load a surface mesh via Assimp and generate a CDT.
 */
int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile = "output_cdt";

    // Simple command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputFile = argv[++i];
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " -i <input_mesh> [-o <output_prefix>]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -i, --input <file>   Input surface mesh file (STL, OBJ, etc.)" << std::endl;
            std::cout << "  -o, --output <name>  Output base filename (default: output_cdt)" << std::endl;
            return 0;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: Input file is required. Use -i <file>." << std::endl;
        return 1;
    }

    std::cout << "Loading mesh from: " << inputFile << " ..." << std::endl;

    Assimp::Importer importer;
    // aiProcess_Triangulate: Ensure all faces are triangles.
    // aiProcess_JoinIdenticalVertices: Merge duplicate vertices to form a closed complex.
    const aiScene* scene = importer.ReadFile(inputFile, 
        aiProcess_Triangulate | 
        aiProcess_JoinIdenticalVertices | 
        aiProcess_SortByPType);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error: Failed to load mesh. " << importer.GetErrorString() << std::endl;
        return 1;
    }

    TetDelMesher mesher;
    int vertexOffset = 0;
    int totalInputFaces = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        
        // Add vertices
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& v = mesh->mVertices[i];
            mesher.addPoint(v.x, v.y, v.z);
        }

        // Add facets (triangles)
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
            const aiFace& face = mesh->mFaces[i];
            if (face.mNumIndices == 3) {
                std::vector<int> indices = {
                    vertexOffset + static_cast<int>(face.mIndices[0]),
                    vertexOffset + static_cast<int>(face.mIndices[1]),
                    vertexOffset + static_cast<int>(face.mIndices[2])
                };
                mesher.addFacet(indices);
                totalInputFaces++;
            }
        }
        vertexOffset += mesh->mNumVertices;
    }

    std::cout << "Input Summary:" << std::endl;
    std::cout << "  Points: " << vertexOffset << std::endl;
    std::cout << "  Triangles: " << totalInputFaces << std::endl;

    std::cout << "Generating Constrained Delaunay Tetrahedralization..." << std::endl;
    TetDelMesherConfig config;
    config.isPLC = true;
    config.quiet = true;
    mesher.setConfig(config);
    Mesh outputMesh = mesher.generate();

    std::cout << "CDT generated successfully!" << std::endl;
    std::cout << "  Nodes: " << outputMesh.points.size() << std::endl;
    std::cout << "  Tetrahedra: " << outputMesh.tetrahedra.size() << std::endl;

    std::cout << "Saving mesh to: " << outputFile << " (.node, .ele)" << std::endl;
    outputMesh.save(outputFile);

    return 0;
}
