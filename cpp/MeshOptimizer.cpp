#include "MeshOptimizer.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <set>

namespace {
    using Point = TetMeshGenerator::Point;

    double getVolume(const Point& a, const Point& b, const Point& c, const Point& d) {
        double ax = a.x - d.x, ay = a.y - d.y, az = a.z - d.z;
        double bx = b.x - d.x, by = b.y - d.y, bz = b.z - d.z;
        double cx = c.x - d.x, cy = c.y - d.y, cz = c.z - d.z;
        return std::abs(ax * (by * cz - bz * cy) + ay * (bz * cx - bx * cz) + az * (bx * cy - by * cx)) / 6.0;
    }

    Point getCircumcenter(const Point& a, const Point& b, const Point& c, const Point& d) {
        double x1 = b.x - a.x, y1 = b.y - a.y, z1 = b.z - a.z;
        double x2 = c.x - a.x, y2 = c.y - a.y, z2 = c.z - a.z;
        double x3 = d.x - a.x, y3 = d.y - a.y, z3 = d.z - a.z;

        double d11 = x1, d12 = y1, d13 = z1;
        double d21 = x2, d22 = y2, d23 = z2;
        double d31 = x3, d32 = y3, d33 = z3;

        double det = d11 * (d22 * d33 - d23 * d32) - d12 * (d21 * d33 - d23 * d31) + d13 * (d21 * d32 - d22 * d31);
        
        if (std::abs(det) < 1e-12) {
            // Degenerate case, return centroid
            return {(a.x + b.x + c.x + d.x) / 4.0, (a.y + b.y + c.y + d.y) / 4.0, (a.z + b.z + c.z + d.z) / 4.0};
        }

        double l1 = x1 * x1 + y1 * y1 + z1 * z1;
        double l2 = x2 * x2 + y2 * y2 + z2 * z2;
        double l3 = x3 * x3 + y3 * y3 + z3 * z3;

        double dx = (l1 * (d22 * d33 - d23 * d32) - d12 * (l2 * d33 - d23 * l3) + d13 * (l2 * d32 - d22 * l3)) / (2.0 * det);
        double dy = (d11 * (l2 * d33 - d23 * l3) - l1 * (d21 * d33 - d23 * d31) + d13 * (d21 * l3 - l2 * d31)) / (2.0 * det);
        double dz = (d11 * (d22 * l3 - l2 * d32) - d12 * (d21 * l3 - l2 * d31) + l1 * (d21 * d32 - d22 * d31)) / (2.0 * det);

        return {a.x + dx, a.y + dy, a.z + dz};
    }
}

void MeshOptimizer::smooth(std::vector<TetMeshGenerator::Point>& points, 
                         const std::vector<TetMeshGenerator::Tetrahedron>& tetrahedra, 
                         int iterations, 
                         FixedPredicate isFixed) {
    if (points.empty()) return;
    
    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<double> nx(points.size(), 0), ny(points.size(), 0), nz(points.size(), 0);
        std::vector<int> count(points.size(), 0);
        
        // Build neighbor connectivity and sum positions
        for (const auto& t : tetrahedra) {
            for (int i = 0; i < 4; ++i) {
                int s = t.v[i];
                for (int j = 1; j < 4; ++j) {
                    int neighbor = t.v[(i + j) % 4];
                    nx[s] += points[neighbor].x;
                    ny[s] += points[neighbor].y;
                    nz[s] += points[neighbor].z;
                    count[s]++;
                }
            }
        }
        
        for (size_t i = 0; i < points.size(); ++i) {
            if (isFixed && isFixed(points[i])) continue;
            if (count[i] > 0) {
                points[i].x = nx[i] / count[i];
                points[i].y = ny[i] / count[i];
                points[i].z = nz[i] / count[i];
            }
        }
    }
}

void MeshOptimizer::relaxODT(std::vector<TetMeshGenerator::Point>& points, 
                           const std::vector<TetMeshGenerator::Tetrahedron>& tetrahedra, 
                           int iterations, 
                           FixedPredicate isFixed) {
    if (points.empty() || tetrahedra.empty()) return;

    for (int iter = 0; iter < iterations; ++iter) {
        std::vector<double> scx(points.size(), 0), scy(points.size(), 0), scz(points.size(), 0);
        std::vector<double> sa(points.size(), 0);

        for (const auto& t : tetrahedra) {
            const auto& p0 = points[t.v[0]];
            const auto& p1 = points[t.v[1]];
            const auto& p2 = points[t.v[2]];
            const auto& p3 = points[t.v[3]];

            double vol = getVolume(p0, p1, p2, p3);
            Point cc = getCircumcenter(p0, p1, p2, p3);

            for (int j = 0; j < 4; ++j) {
                int vid = t.v[j];
                scx[vid] += cc.x * vol;
                scy[vid] += cc.y * vol;
                scz[vid] += cc.z * vol;
                sa[vid] += vol;
            }
        }

        for (size_t i = 0; i < points.size(); ++i) {
            if (isFixed && isFixed(points[i])) continue;
            if (sa[i] > 1e-12) {
                points[i].x = scx[i] / sa[i];
                points[i].y = scy[i] / sa[i];
                points[i].z = scz[i] / sa[i];
            }
        }
    }
}
