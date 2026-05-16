#include <iostream>
#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TetDelMesher.h"

/**
 * @brief CLI application to read a mesh file and compute the convex hull of its points.
 */
int main(int argc, char* argv[]) {
    std::string inputFile;
    std::string outputFile = "output_hull";

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
            std::cout << "  -i, --input <file>   Input mesh file (STL, OBJ, etc.)" << std::endl;
            std::cout << "  -o, --output <name>  Output base filename (default: output_hull)" << std::endl;
            return 0;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: Input file is required. Use -i <file>." << std::endl;
        return 1;
    }

    std::cout << "Loading points from: " << inputFile << " ..." << std::endl;

    Assimp::Importer importer;
    // We only need the vertices, but joining identical ones is good for reducing point count.
    const aiScene* scene = importer.ReadFile(inputFile, aiProcess_JoinIdenticalVertices);

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error: Failed to load mesh. " << importer.GetErrorString() << std::endl;
        return 1;
    }

    TetDelMesher mesher;
    int totalPoints = 0;

    for (unsigned int m = 0; m < scene->mNumMeshes; ++m) {
        const aiMesh* mesh = scene->mMeshes[m];
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
            const aiVector3D& v = mesh->mVertices[i];
            mesher.addPoint(v.x, v.y, v.z);
            totalPoints++;
        }
    }

    std::cout << "Computing convex hull of " << totalPoints << " points ..." << std::endl;
    TetDelMesherConfig config;
    config.convexHull = true;
    config.quiet = true;
    mesher.setConfig(config);
    Mesh hullMesh = mesher.generate();

    std::cout << "Convex hull computed successfully!" << std::endl;
    std::cout << "  Hull Points: " << hullMesh.points.size() << std::endl;
    std::cout << "  Hull Faces:  " << hullMesh.trifaces.size() << std::endl;

    std::cout << "Saving hull to: " << outputFile << " (.node, .face)" << std::endl;
    hullMesh.save(outputFile);

    return 0;
}
