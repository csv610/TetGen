#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <string>
#include <numbers>
#include "TetDelMesher.h"

/**
 * @brief CLI application to generate random points in a sphere and compute their convex hull.
 */
int main(int argc, char* argv[]) {
    double radius = 1.0;
    int numPoints = 1000;
    std::string output = "sphere_hull";

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
            std::cout << "  -o, --output <name>  Output base filename (default: sphere_hull)" << std::endl;
            return 0;
        } else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    std::cout << "Generating " << numPoints << " points in a sphere of radius " << radius << "..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(0.0, 1.0);

    TetDelMesher mesher;
    for (int i = 0; i < numPoints; ++i) {
        double u = dis(gen);
        double v = dis(gen);
        double w = dis(gen);

        double phi = 2.0 * std::numbers::pi * u;
        double cosTheta = 2.0 * v - 1.0;
        double sinTheta = std::sqrt(std::max(0.0, 1.0 - cosTheta * cosTheta));
        double r = radius * std::pow(w, 1.0 / 3.0);

        double x = r * sinTheta * std::cos(phi);
        double y = r * sinTheta * std::sin(phi);
        double z = r * cosTheta;

        mesher.addPoint(x, y, z);
    }

    std::cout << "Computing convex hull..." << std::endl;
    mesher.setConvexHull(true);
    mesher.setQuiet(true);
    Mesh mesh = mesher.generate();

    std::cout << "Convex hull computed successfully!" << std::endl;
    std::cout << "  Original Points: " << numPoints << std::endl;
    std::cout << "  Hull Points: " << mesh.points.size() << std::endl;
    std::cout << "  Hull Faces:  " << mesh.trifaces.size() << std::endl;

    std::cout << "Saving hull to: " << output << " (.node, .face)" << std::endl;
    mesh.save(output);

    return 0;
}
