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

#include "../uv_mapping.h"

namespace df {

class MeshOptimizer {
  public:
    MeshOptimizer(const Mesh3D &mesh) : originalMesh_(mesh) {
        // Create working copy
        workingMesh_ = mesh;

        // Find border vertices
        findBorderVertices();

        // Compute target edge length from average
        computeTargetLength();
    }

    Mesh3D run(const OptimizeParams &params) {
        double maxMove = std::numeric_limits<double>::max();
        int iter = 0;

        while (maxMove > params.convergenceTol && iter < params.maxIterations) {
            maxMove = 0;

            // Update each non-border vertex
            auto &vertices = workingMesh_.vertices();
            for (size_t i = 0; i < vertices.size(); ++i) {
                if (borderVertices_.find(i) != borderVertices_.end()) {
                    continue;
                }

                Vector3 idealPos = computeIdealPosition(i);
                Vector3 projectedPos = projectToSurface(idealPos, i);
                Vector3 currentPos = vertices[i];

                Vector3 move = projectedPos - currentPos;
                double moveLength = length(move);
                maxMove = std::max(maxMove, moveLength);

                vertices[i] = currentPos + move * params.damping;
            }

            iter++;
        }

        return workingMesh_;
    }

  private:
    using TriList = std::vector<std::vector<size_t>>;

    void findBorderVertices() {
        TriList vertexTriangles(workingMesh_.vertexCount());

        // Build vertex-triangle adjacency
        const auto &triangles = workingMesh_.triangles();
        for (size_t i = 0; i < triangles.size(); ++i) {
            const auto &tri = triangles[i];
            vertexTriangles[tri[0]].push_back(i);
            vertexTriangles[tri[1]].push_back(i);
            vertexTriangles[tri[2]].push_back(i);
        }

        // A vertex is on border if it has an edge that appears only once
        for (size_t i = 0; i < vertexTriangles.size(); ++i) {
            if (isBorderVertex(i, vertexTriangles)) {
                borderVertices_.insert(i);
            }
        }
    }

    bool isBorderVertex(size_t vertexIndex, const TriList &vertexTriangles) {
        std::unordered_map<size_t, int> edgeCount;

        // Count occurrences of each edge connected to this vertex
        for (size_t triIdx : vertexTriangles[vertexIndex]) {
            const auto &tri = workingMesh_.triangles()[triIdx];
            for (int j = 0; j < 3; ++j) {
                if (tri[j] == vertexIndex) {
                    size_t next = tri[(j + 1) % 3];
                    size_t prev = tri[(j + 2) % 3];
                    edgeCount[next]++;
                    edgeCount[prev]++;
                }
            }
        }

        // If any edge appears only once, this is a border vertex
        for (const auto &[_, count] : edgeCount) {
            if (count == 1)
                return true;
        }
        return false;
    }

    void computeTargetLength() {
        double totalLength = 0;
        int edgeCount = 0;

        const auto &vertices = workingMesh_.vertices();
        const auto &triangles = workingMesh_.triangles();

        for (const auto &tri : triangles.data()) {
            for (int i = 0; i < 3; ++i) {
                Vector3 p1 = vertices[tri[i]];
                Vector3 p2 = vertices[tri[(i + 1) % 3]];
                totalLength += length(p2 - p1);
                edgeCount++;
            }
        }

        targetLength_ = totalLength / edgeCount;
    }

    Vector3 computeIdealPosition(size_t vertexIndex) {
        Vector3 avgPos{0, 0, 0};
        int neighborCount = 0;

        const auto &vertices = workingMesh_.vertices();
        const auto &triangles = workingMesh_.triangles();

        // Get one-ring neighbors
        for (size_t i = 0; i < triangles.size(); ++i) {
            const auto &tri = triangles[i];
            for (int j = 0; j < 3; ++j) {
                if (tri[j] == vertexIndex) {
                    // Add contribution from both other vertices in triangle
                    Vector3 p = vertices[tri[(j + 1) % 3]];
                    avgPos += p;
                    neighborCount++;

                    p = vertices[tri[(j + 2) % 3]];
                    avgPos += p;
                    neighborCount++;
                }
            }
        }

        if (neighborCount > 0) {
            avgPos = avgPos / neighborCount;

            // Project to sphere of radius targetLength_
            Vector3 center = vertices[vertexIndex];
            Vector3 direction = avgPos - center;
            return center + normalize(direction) * targetLength_;
        }

        return vertices[vertexIndex];
    }

    Vector3 projectToSurface(const Vector3 &point, size_t vertexIndex) {
        // Find closest triangle in original mesh
        const auto &origVertices = originalMesh_.vertices();
        const auto &origTriangles = originalMesh_.triangles();

        double minDist = std::numeric_limits<double>::max();
        Vector3 projected = point;

        for (size_t i = 0; i < origTriangles.size(); ++i) {
            const auto &tri = origTriangles[i];
            Vector3 p1 = origVertices[tri[0]];
            Vector3 p2 = origVertices[tri[1]];
            Vector3 p3 = origVertices[tri[2]];

            Vector3 normal = normalize(cross(p2 - p1, p3 - p1));
            double dist = dot(point - p1, normal);
            Vector3 projected_point = point - normal * dist;

            if (isPointInTriangle(projected_point, p1, p2, p3)) {
                if (std::abs(dist) < minDist) {
                    minDist = std::abs(dist);
                    projected = projected_point;
                }
            }
        }

        return projected;
    }

    bool isPointInTriangle(const Vector3 &p, const Vector3 &a, const Vector3 &b,
                           const Vector3 &c) {
        Vector3 v0 = c - a;
        Vector3 v1 = b - a;
        Vector3 v2 = p - a;

        double dot00 = dot(v0, v0);
        double dot01 = dot(v0, v1);
        double dot02 = dot(v0, v2);
        double dot11 = dot(v1, v1);
        double dot12 = dot(v1, v2);

        double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
        double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        double v = (dot00 * dot12 - dot01 * dot02) * invDenom;

        return (u >= 0) && (v >= 0) && (u + v <= 1);
    }

  private:
    Mesh3D originalMesh_;
    Mesh3D workingMesh_;
    std::unordered_set<size_t> borderVertices_;
    double targetLength_;
};

inline Mesh3D optimize(const Mesh3D &mesh, const OptimizeParams &params) {
    MeshOptimizer optimizer(mesh);
    return optimizer.run(params);
}

// ============================================================================

class UVSpaceOptimizer {
  public:
    static Serie<Vector2> optimizeUVMesh(const Mesh3D &mesh,
                                         const UVMapping &uvMapping) {
        const auto &triangles = mesh.triangles();
        auto uvCoords = uvMapping.uvCoords;
        const auto &seams = uvMapping.seams;

        double maxMove = std::numeric_limits<double>::max();
        int iter = 0;
        const int maxIter = 100;
        const double tol = 1e-6;
        const double damping = 0.5;

        while (maxMove > tol && iter < maxIter) {
            maxMove = 0;

            // For each vertex
            for (size_t i = 0; i < uvCoords.size(); ++i) {
                Vector2 sum{0, 0};
                int count = 0;

                // Find connected vertices through triangles
                for (size_t j = 0; j < triangles.size(); ++j) {
                    const auto &tri = triangles[j];
                    for (int k = 0; k < 3; ++k) {
                        if (tri[k] == i) {
                            sum += uvCoords[tri[(k + 1) % 3]];
                            sum += uvCoords[tri[(k + 2) % 3]];
                            count += 2;
                        }
                    }
                }

                if (count > 0) {
                    Vector2 target = sum / count;
                    Vector2 move = target - uvCoords[i];
                    maxMove = std::max(maxMove, length(move));
                    uvCoords[i] += move * damping;
                }
            }

            // Maintain seam consistency
            seams.forEach([&](const iVector2 &seam, size_t) {
                Vector2 avg = (uvCoords[seam[0]] + uvCoords[seam[1]]) * 0.5;
                uvCoords[seam[0]] = avg;
                uvCoords[seam[1]] = avg;
            });

            iter++;
        }

        return uvCoords;
    }

    static Mesh3D mapToSurface(const Mesh3D &origMesh,
                               const Serie<Vector2> &uvCoords) {
        Mesh3D result = origMesh;
        auto &vertices = result.vertices();
        auto &origVertices = origMesh.vertices();
        const auto &triangles = origMesh.triangles();

        // For each vertex
        for (size_t i = 0; i < vertices.size(); ++i) {
            const Vector2 &uv = uvCoords[i];

            // Find containing triangle in UV space
            for (size_t j = 0; j < triangles.size(); ++j) {
                const auto &tri = triangles[j];
                Vector2 uv1 = uvCoords[tri[0]];
                Vector2 uv2 = uvCoords[tri[1]];
                Vector2 uv3 = uvCoords[tri[2]];

                // Compute barycentric coordinates
                Vector2 v0 = uv2 - uv1;
                Vector2 v1 = uv3 - uv1;
                Vector2 v2 = uv - uv1;

                double d00 = dot(v0, v0);
                double d01 = dot(v0, v1);
                double d11 = dot(v1, v1);
                double d20 = dot(v2, v0);
                double d21 = dot(v2, v1);

                double denom = d00 * d11 - d01 * d01;
                double v = (d11 * d20 - d01 * d21) / denom;
                double w = (d00 * d21 - d01 * d20) / denom;
                double u = 1.0 - v - w;

                if (u >= 0 && v >= 0 && w >= 0) {
                    // Use barycentric coordinates to interpolate 3D position
                    vertices[i] = origVertices[tri[0]] * u +
                                  origVertices[tri[1]] * v +
                                  origVertices[tri[2]] * w;
                    break;
                }
            }
        }

        return result;
    }
};

inline Mesh3D optimizeLSCM(const Mesh3D &mesh) {
    // 1. Compute UV parameterization
    auto map = uvMapping(mesh);

    // 2. Optimize mesh in UV space
    auto optimizedUVs = UVSpaceOptimizer::optimizeUVMesh(mesh, map);

    // 3. Map back to 3D
    return UVSpaceOptimizer::mapToSurface(mesh, optimizedUVs);
}

} // namespace df
