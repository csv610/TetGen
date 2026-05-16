#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <numbers>
#include "TetDelMesher.h"

/**
 * @brief CLI application to generate random points in a sphere and create a Delaunay mesh.
 */
int main(int argc, char* argv[]) {
    double radius = 1.0;
    int numPoints = 1000;
    std::string output = "sphere_mesh";

    // Simple command line parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-r" || arg == "--radius") && i + 1 < argc) {
            radius = std::stod(argv[++i]);
        } else if ((arg == "-n" || arg == "--num") && i + 1 < argc) {
            numPoints = std::stoi(argv[++i]);
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            output = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -r, --radius <val>   Radius of the sphere (default: 1.0)" << std::endl;
            std::cout << "  -n, --num <val>      Number of points to generate (default: 1000)" << std::endl;
            std::cout << "  -o, --output <name>  Output base filename (default: sphere_mesh)" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    std::cout << "Generating " << numPoints << " points in a sphere of radius " << radius << "..." << std::endl;

    // Use <random> for high-quality uniform sampling in a sphere
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    TetDelMesher mesher;
    for (int i = 0; i < numPoints; ++i) {
        double u = dis(gen);
        double v = dis(gen);
        double w = dis(gen);

        // Uniform distribution in a sphere
        double phi = 2.0 * std::numbers::pi * u;
        double cosTheta = 2.0 * v - 1.0;
        double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));
        double r = radius * std::pow(w, 1.0 / 3.0);

        double x = r * sinTheta * std::cos(phi);
        double y = r * sinTheta * std::sin(phi);
        double z = r * cosTheta;

        mesher.addPoint(x, y, z);
    }

    std::cout << "Generating Delaunay tetrahedralization..." << std::endl;
    // Set quiet mode off to see TetGen output if desired, or keep it quiet for a clean CLI.
    // Let's keep it quiet but print our own status.
    TetDelMesherConfig config;
    config.quiet = true;
    mesher.setConfig(config);
    Mesh mesh = mesher.generate();

    std::cout << "Mesh generated successfully!" << std::endl;
    std::cout << "  Nodes: " << mesh.points.size() << std::endl;
    std::cout << "  Tetrahedra: " << mesh.tetrahedra.size() << std::endl;

    std::cout << "Saving mesh to: " << output << " (.node, .ele)" << std::endl;
    mesh.save(output);

    return 0;
}
