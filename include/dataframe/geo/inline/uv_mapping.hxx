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

namespace df {

class MeshParametrizer {
  public:
    static Serie<iVector2> computeSeams(const Mesh3D &mesh) {
        const auto &vertices = mesh.vertices();
        const auto &triangles = mesh.triangles();

        // Build dual graph where vertices are triangles
        std::vector<std::vector<std::pair<size_t, double>>> dualGraph(
            triangles.size());

        // For each triangle
        for (size_t i = 0; i < triangles.size(); ++i) {
            const auto &tri1 = triangles[i];
            // Find adjacent triangles through edges
            for (size_t j = i + 1; j < triangles.size(); ++j) {
                const auto &tri2 = triangles[j];

                // Check if triangles share an edge
                int sharedVerts = 0;
                for (int k = 0; k < 3; ++k) {
                    for (int l = 0; l < 3; ++l) {
                        if (tri1[k] == tri2[l])
                            sharedVerts++;
                    }
                }

                if (sharedVerts == 2) {
                    // Triangles are adjacent - compute edge weight based on
                    // dihedral angle
                    Vector3 n1 =
                        triangleNormal(vertices[tri1[0]], vertices[tri1[1]],
                                       vertices[tri1[2]]);
                    Vector3 n2 =
                        triangleNormal(vertices[tri2[0]], vertices[tri2[1]],
                                       vertices[tri2[2]]);
                    double weight =
                        1.0 -
                        dot(n1, n2); // Higher weight for higher dihedral angle

                    dualGraph[i].push_back({j, weight});
                    dualGraph[j].push_back({i, weight});
                }
            }
        }

        // Run Prim's algorithm for MST
        std::vector<bool> inMST(triangles.size(), false);
        std::vector<std::pair<size_t, size_t>> mstEdges;

        inMST[0] = true;
        for (size_t i = 1; i < triangles.size(); ++i) {
            double minWeight = std::numeric_limits<double>::max();
            size_t u = 0, v = 0;

            for (size_t j = 0; j < triangles.size(); ++j) {
                if (!inMST[j])
                    continue;

                for (const auto &[neighbor, weight] : dualGraph[j]) {
                    if (!inMST[neighbor] && weight < minWeight) {
                        minWeight = weight;
                        u = j;
                        v = neighbor;
                    }
                }
            }

            inMST[v] = true;
            mstEdges.push_back({u, v});
        }

        // Extract seam edges (edges not in MST)
        std::vector<iVector2> seams;
        for (size_t i = 0; i < triangles.size(); ++i) {
            for (const auto &[neighbor, _] : dualGraph[i]) {
                if (neighbor < i)
                    continue; // Only process each edge once

                // Check if edge is in MST
                bool inMst = false;
                for (const auto &[u, v] : mstEdges) {
                    if ((u == i && v == neighbor) ||
                        (u == neighbor && v == i)) {
                        inMst = true;
                        break;
                    }
                }

                if (!inMst) {
                    // Find shared vertices between triangles
                    const auto &tri1 = triangles[i];
                    const auto &tri2 = triangles[neighbor];
                    for (int k = 0; k < 3; ++k) {
                        for (int l = 0; l < 3; ++l) {
                            if (tri1[k] == tri2[l]) {
                                seams.push_back(
                                    iVector2{tri1[k], tri1[(k + 1) % 3]});
                            }
                        }
                    }
                }
            }
        }

        return Serie<iVector2>(seams);
    }

    static UVMapping solveUVSystem(const Mesh3D &mesh,
                                   const Serie<iVector2> &seams) {
        const auto &vertices = mesh.vertices();
        const auto &triangles = mesh.triangles();

        size_t n = vertices.size() * 2;
        Eigen::SparseMatrix<double> A(n, n);
        Eigen::VectorXd b = Eigen::VectorXd::Zero(n);

        std::vector<Eigen::Triplet<double>> triplets;

        // For each triangle, add LSCM energy terms
        for (size_t i = 0; i < triangles.size(); ++i) {
            const auto &tri = triangles[i];
            Vector3 p0 = vertices[tri[0]];
            Vector3 p1 = vertices[tri[1]];
            Vector3 p2 = vertices[tri[2]];

            // Compute local basis
            Vector3 e1 = normalize(p1 - p0);
            Vector3 n = normalize(cross(e1, p2 - p0));
            Vector3 e2 = cross(n, e1);

            // Project vertices to local 2D coordinates
            double x1 = length(p1 - p0);
            double x2 = dot(p2 - p0, e1);
            double y2 = dot(p2 - p0, e2);

            // Build LSCM matrix for this triangle
            double area = 0.5 * x1 * y2;

            // Matrix entries for real part
            addLSCMTerms(triplets, tri[0], tri[1], tri[2], x1, x2, y2, area,
                         true);
            // Matrix entries for imaginary part
            addLSCMTerms(triplets, tri[0], tri[1], tri[2], x1, x2, y2, area,
                         false);
        }

        // Add seam constraints
        double seamWeight = 1000.0; // High weight for constraints
        seams.forEach([&](const iVector2 &seam, size_t) {
            int v1 = seam[0];
            int v2 = seam[1];

            // u coordinates should match
            triplets.push_back({2 * v1, 2 * v2, seamWeight});
            triplets.push_back({2 * v2, 2 * v1, seamWeight});

            // v coordinates should match
            triplets.push_back({2 * v1 + 1, 2 * v2 + 1, seamWeight});
            triplets.push_back({2 * v2 + 1, 2 * v1 + 1, seamWeight});
        });

        // Pin two vertices to fix translation and rotation
        triplets.push_back({0, 0, 1.0});
        triplets.push_back({1, 1, 1.0});
        triplets.push_back({2, 2, 1.0});

        A.setFromTriplets(triplets.begin(), triplets.end());

        // Solve system
        Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
        Eigen::VectorXd x = solver.compute(A).solve(b);

        // Convert to UV coordinates
        std::vector<Vector2> uvs;
        uvs.reserve(vertices.size());
        for (size_t i = 0; i < vertices.size(); ++i) {
            uvs.push_back({x[2 * i], x[2 * i + 1]});
        }

        UVMapping result;
        result.uvCoords = Serie<Vector2>(uvs);
        result.seams = seams;
        return result;
    }

  private:
    static UVMapping computeUVMapping(const Mesh3D &mesh) {
        // 1. Find seams using minimum spanning tree of dual graph
        auto seams = computeSeams(mesh);

        // 2. Setup linear system for LSCM (Least Squares Conformal Maps)
        return solveUVSystem(mesh, seams);
    }

    static Vector3 triangleNormal(const Vector3 &p1, const Vector3 &p2,
                                  const Vector3 &p3) {
        return normalize(cross(p2 - p1, p3 - p1));
    }

    static void addLSCMTerms(std::vector<Eigen::Triplet<double>> &triplets,
                             int v0, int v1, int v2, double x1, double x2,
                             double y2, double area, bool real) {
        double a = (-x1 * y2 + x2 * y2) / (2.0 * area);
        double b = (-x2) / (2.0 * area);
        double c = (x1) / (2.0 * area);
        double d = (-y2) / (2.0 * area);

        if (real) {
            triplets.push_back({2 * v0, 2 * v0, (a * a + d * d)});
            triplets.push_back({2 * v1, 2 * v1, (b * b)});
            triplets.push_back({2 * v2, 2 * v2, (c * c)});
            triplets.push_back({2 * v0, 2 * v1, a * b});
            triplets.push_back({2 * v1, 2 * v0, a * b});
            triplets.push_back({2 * v0, 2 * v2, a * c});
            triplets.push_back({2 * v2, 2 * v0, a * c});
            triplets.push_back({2 * v1, 2 * v2, b * c});
            triplets.push_back({2 * v2, 2 * v1, b * c});
        } else {
            triplets.push_back({2 * v0 + 1, 2 * v0 + 1, (a * a + d * d)});
            triplets.push_back({2 * v1 + 1, 2 * v1 + 1, (b * b)});
            triplets.push_back({2 * v2 + 1, 2 * v2 + 1, (c * c)});
            triplets.push_back({2 * v0 + 1, 2 * v1 + 1, a * b});
            triplets.push_back({2 * v1 + 1, 2 * v0 + 1, a * b});
            triplets.push_back({2 * v0 + 1, 2 * v2 + 1, a * c});
            triplets.push_back({2 * v2 + 1, 2 * v0 + 1, a * c});
            triplets.push_back({2 * v1 + 1, 2 * v2 + 1, b * c});
            triplets.push_back({2 * v2 + 1, 2 * v1 + 1, b * c});
        }
    }
};

inline UVMapping uvMapping(const Mesh3D &mesh) {
    // 1. Find seams using minimum spanning tree of dual graph
    auto seams = MeshParametrizer::computeSeams(mesh);

    // 2. Setup linear system for LSCM (Least Squares Conformal Maps)
    return MeshParametrizer::solveUVSystem(mesh, seams);
}

} // namespace df