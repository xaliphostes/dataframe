/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cmath>
#include <dataframe/geo/mesh/mesh_optimizer.h>
#include <dataframe/geo/mesh/uv_mapping.h>
#include <dataframe/utils/print.h>
#include <fstream>
#include <iostream>

// Vector arithmetic operators
Vector3 operator-(const Vector3 &a, const Vector3 &b) {
    Vector3 result;
    for (size_t i = 0; i < 3; ++i) {
        result[i] = a[i] - b[i];
    }
    return result;
}

inline void exportGocadTS(const df::Mesh3D &mesh, const std::string &filename,
                          const std::string &name = "mesh") {
    std::ofstream file(filename);

    file << "GOCAD TSurf 1\n";
    file << "HEADER {\n";
    file << "name:" << name << "\n";
    file << "}\n";
    file << "PROPERTIES FRIC\n";
    file << "ESIZES 1\n";
    file << "TFACE\n";

    // Write vertices
    mesh.vertices().forEach([&](const Vector3 &v, size_t index) {
        file << "PVRTX " << index << " " << v[0] << " " << v[1] << " " << v[2]
             << " 0\n"; // Last value is property (FRIC)
    });

    // Write triangles
    mesh.triangles().forEach([&](const iVector3 &t, size_t index) {
        file << "TRGL " << t[0] << " " << t[1] << " " << t[2] << "\n";
    });

    file << "END\n";
}

int main() {
    // Create hemisphere mesh
    std::vector<Vector3> vertices;
    std::vector<iVector3> triangles;

    const int rings = 10;
    const int segments = 16;
    const double radius = 1.0;

    vertices.push_back({0, 0, radius}); // Top vertex

    // Generate rings
    for (uint ring = 1; ring < rings; ring++) {
        double phi = M_PI * 0.5 * (double)ring / rings;
        for (uint seg = 0; seg < segments; seg++) {
            double theta = 2.0 * M_PI * (double)seg / segments;
            vertices.push_back({radius * sin(phi) * cos(theta),
                                radius * sin(phi) * sin(theta),
                                radius * cos(phi)});
        }
    }

    // Top triangles
    for (uint seg = 0; seg < segments; seg++) {
        triangles.push_back({0, seg + 1, (seg + 1) % segments + 1});
    }

    // Ring triangles
    for (uint ring = 1; ring < rings - 1; ring++) {
        uint ringStart = 1 + (ring - 1) * segments;
        uint nextStart = ringStart + segments;
        for (uint seg = 0; seg < segments; seg++) {
            uint next = (seg + 1) % segments;
            triangles.push_back(
                {ringStart + seg, nextStart + seg, ringStart + next});
            triangles.push_back(
                {ringStart + next, nextStart + seg, nextStart + next});
        }
    }

    df::Mesh3D mesh(vertices, triangles);
    exportGocadTS(mesh, "hemisphere.ts");

    // Optimize mesh
    df::OptimizeParams params;
    params.maxIterations = 50;
    params.damping = 0.3;

    df::Mesh3D optimizedMesh = df::optimize(mesh, params);
    exportGocadTS(optimizedMesh, "hemisphere_optimized.ts");

    // Also try LSCM optimization
    df::Mesh3D lscmOptimizedMesh = df::optimizeLSCM(mesh);
    exportGocadTS(lscmOptimizedMesh, "hemisphere_lscm_optimized.ts");

    // UV mapping for testing
    df::UVMapping uvMap = df::uvMapping(mesh);

    return 0;
}
