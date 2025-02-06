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

// curvature.hxx
#include <cmath>
#include <map>
#include <set>

namespace df {
    
namespace detail {

struct Edge {
    size_t v1, v2;
    Edge(size_t a, size_t b) : v1(std::min(a, b)), v2(std::max(a, b)) {}
    bool operator<(const Edge &other) const {
        return v1 < other.v1 || (v1 == other.v1 && v2 < other.v2);
    }
};

// Compute cotangent of angle at point p between vectors to a and b
double cotangent(const Vector3 &p, const Vector3 &a, const Vector3 &b) {
    Vector3 u = a - p;
    Vector3 v = b - p;
    double dot = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
    double u_norm = std::sqrt(u[0] * u[0] + u[1] * u[1] + u[2] * u[2]);
    double v_norm = std::sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    double sin_theta =
        std::sqrt(1 - (dot * dot) / (u_norm * u_norm * v_norm * v_norm));
    double cos_theta = dot / (u_norm * v_norm);
    return cos_theta / sin_theta;
}

// Compute vertex normal using weighted average of face normals
Vector3 compute_vertex_normal(const Positions3 &vertices,
                              const Triangles &triangles, size_t vertex_idx,
                              const std::vector<size_t> &vertex_triangles) {
    Vector3 normal = {0, 0, 0};

    for (size_t tri_idx : vertex_triangles) {
        const auto &tri = triangles[tri_idx];
        const auto &v0 = vertices[tri[0]];
        const auto &v1 = vertices[tri[1]];
        const auto &v2 = vertices[tri[2]];

        // Compute face normal
        Vector3 e1 = {v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2]};
        Vector3 e2 = {v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2]};
        Vector3 face_normal = {e1[1] * e2[2] - e1[2] * e2[1],
                               e1[2] * e2[0] - e1[0] * e2[2],
                               e1[0] * e2[1] - e1[1] * e2[0]};

        // Weight by face area
        double area = std::sqrt(face_normal[0] * face_normal[0] +
                                face_normal[1] * face_normal[1] +
                                face_normal[2] * face_normal[2]) /
                      2;
        normal[0] += face_normal[0] * area;
        normal[1] += face_normal[1] * area;
        normal[2] += face_normal[2] * area;
    }

    // Normalize
    double length = std::sqrt(normal[0] * normal[0] + normal[1] * normal[1] +
                              normal[2] * normal[2]);
    if (length > 1e-10) {
        normal[0] /= length;
        normal[1] /= length;
        normal[2] /= length;
    }

    return normal;
}

} // namespace detail

Dataframe surface_curvature(const Positions3 &vertices,
                            const Triangles &triangles) {
    size_t num_vertices = vertices.size();

    // Build vertex adjacency information
    std::vector<std::vector<size_t>> vertex_triangles(num_vertices);
    std::map<detail::Edge, std::vector<size_t>> edge_triangles;

    // Collect triangles incident to each vertex and edge
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &tri = triangles[i];
        vertex_triangles[tri[0]].push_back(i);
        vertex_triangles[tri[1]].push_back(i);
        vertex_triangles[tri[2]].push_back(i);

        edge_triangles[detail::Edge(tri[0], tri[1])].push_back(i);
        edge_triangles[detail::Edge(tri[1], tri[2])].push_back(i);
        edge_triangles[detail::Edge(tri[2], tri[0])].push_back(i);
    }

    // Compute vertex normals
    std::vector<Vector3> normals(num_vertices);
    for (size_t i = 0; i < num_vertices; ++i) {
        normals[i] = detail::compute_vertex_normal(vertices, triangles, i,
                                                   vertex_triangles[i]);
    }

    // Compute Laplace-Beltrami operator and mean curvature
    std::vector<double> mean_curvature(num_vertices, 0.0);
    std::vector<double> vertex_area(num_vertices, 0.0);

    for (const auto &[edge, tri_indices] : edge_triangles) {
        if (tri_indices.size() != 2)
            continue; // Skip boundary edges

        size_t v1 = edge.v1;
        size_t v2 = edge.v2;

        // Find opposite vertices in the two triangles
        const auto &tri1 = triangles[tri_indices[0]];
        const auto &tri2 = triangles[tri_indices[1]];

        size_t v3 = -1, v4 = -1;
        for (int j = 0; j < 3; ++j) {
            if (tri1[j] != v1 && tri1[j] != v2)
                v3 = tri1[j];
            if (tri2[j] != v1 && tri2[j] != v2)
                v4 = tri2[j];
        }

        // Compute cotangent weights
        double cot_alpha =
            detail::cotangent(vertices[v1], vertices[v2], vertices[v3]);
        double cot_beta =
            detail::cotangent(vertices[v2], vertices[v1], vertices[v4]);
        double weight = (cot_alpha + cot_beta) / 2.0;

        // Accumulate weighted Laplacian
        Vector3 diff = {vertices[v2][0] - vertices[v1][0],
                        vertices[v2][1] - vertices[v1][1],
                        vertices[v2][2] - vertices[v1][2]};

        mean_curvature[v1] +=
            weight * (diff[0] * normals[v1][0] + diff[1] * normals[v1][1] +
                      diff[2] * normals[v1][2]);
        mean_curvature[v2] -=
            weight * (diff[0] * normals[v2][0] + diff[1] * normals[v2][1] +
                      diff[2] * normals[v2][2]);
    }

    // Compute Voronoi areas for vertices
    for (size_t i = 0; i < triangles.size(); ++i) {
        const auto &tri = triangles[i];
        const auto &v1 = vertices[tri[0]];
        const auto &v2 = vertices[tri[1]];
        const auto &v3 = vertices[tri[2]];

        // Compute triangle area
        Vector3 e1 = {v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]};
        Vector3 e2 = {v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2]};
        Vector3 cross = {e1[1] * e2[2] - e1[2] * e2[1],
                         e1[2] * e2[0] - e1[0] * e2[2],
                         e1[0] * e2[1] - e1[1] * e2[0]};
        double area = std::sqrt(cross[0] * cross[0] + cross[1] * cross[1] +
                                cross[2] * cross[2]) /
                      2;

        // Distribute area to vertices (using barycentric coordinates for
        // simplicity)
        vertex_area[tri[0]] += area / 3;
        vertex_area[tri[1]] += area / 3;
        vertex_area[tri[2]] += area / 3;
    }

    // Normalize mean curvature by vertex areas
    std::vector<double> k1(num_vertices);
    std::vector<double> k2(num_vertices);
    std::vector<double> gaussian_curvature(num_vertices);

    for (size_t i = 0; i < num_vertices; ++i) {
        if (vertex_area[i] > 1e-10) {
            mean_curvature[i] /= 4 * vertex_area[i];

            // Estimate Gaussian curvature using angle defect
            double angle_sum = 0;
            for (size_t tri_idx : vertex_triangles[i]) {
                const auto &tri = triangles[tri_idx];
                size_t prev = tri[2], curr = tri[0], next = tri[1];
                if (curr != i) {
                    prev = tri[0];
                    curr = tri[1];
                    next = tri[2];
                    if (curr != i) {
                        prev = tri[1];
                        curr = tri[2];
                        next = tri[0];
                    }
                }

                Vector3 e1 = {vertices[prev][0] - vertices[curr][0],
                              vertices[prev][1] - vertices[curr][1],
                              vertices[prev][2] - vertices[curr][2]};
                Vector3 e2 = {vertices[next][0] - vertices[curr][0],
                              vertices[next][1] - vertices[curr][1],
                              vertices[next][2] - vertices[curr][2]};

                double e1_len =
                    std::sqrt(e1[0] * e1[0] + e1[1] * e1[1] + e1[2] * e1[2]);
                double e2_len =
                    std::sqrt(e2[0] * e2[0] + e2[1] * e2[1] + e2[2] * e2[2]);
                double dot = e1[0] * e2[0] + e1[1] * e2[1] + e1[2] * e2[2];

                angle_sum += std::acos(
                    std::max(-1.0, std::min(1.0, dot / (e1_len * e2_len))));
            }

            gaussian_curvature[i] = (2 * M_PI - angle_sum) / vertex_area[i];

            // Compute principal curvatures from mean and Gaussian curvature
            double H = mean_curvature[i];
            double K = gaussian_curvature[i];
            double discriminant = std::max(0.0, H * H - K);
            k1[i] = H + std::sqrt(discriminant);
            k2[i] = H - std::sqrt(discriminant);
        }
    }

    // Compute curvature tensor and principal directions
    using Matrix3 = std::array<double, 9>; // Row-major 3x3 matrix
    std::vector<Matrix3> curvature_tensors(num_vertices,
                                           {0, 0, 0, 0, 0, 0, 0, 0, 0});
    std::vector<Vector3> principal_dir1(num_vertices, {0, 0, 0});
    std::vector<Vector3> principal_dir2(num_vertices, {0, 0, 0});

    for (size_t i = 0; i < num_vertices; ++i) {
        if (vertex_area[i] <= 1e-10)
            continue;

        // Create local coordinate system
        const Vector3 &normal = normals[i];

        // Find least-squares tangent direction
        double xx = 0, xy = 0, xz = 0, yy = 0, yz = 0, zz = 0;
        for (size_t tri_idx : vertex_triangles[i]) {
            const auto &tri = triangles[tri_idx];
            for (int j = 0; j < 3; ++j) {
                if (tri[j] == i)
                    continue;
                Vector3 edge = {vertices[tri[j]][0] - vertices[i][0],
                                vertices[tri[j]][1] - vertices[i][1],
                                vertices[tri[j]][2] - vertices[i][2]};
                xx += edge[0] * edge[0];
                xy += edge[0] * edge[1];
                xz += edge[0] * edge[2];
                yy += edge[1] * edge[1];
                yz += edge[1] * edge[2];
                zz += edge[2] * edge[2];
            }
        }

        // Find tangent direction using largest eigenvector of covariance matrix
        Matrix3 covar = {xx, xy, xz, xy, yy, yz, xz, yz, zz};
        Vector3 tangent1 = {1, 0, 0}; // Initial guess

        // Power iteration to find dominant eigenvector
        for (int iter = 0; iter < 5; ++iter) {
            Vector3 next = {covar[0] * tangent1[0] + covar[1] * tangent1[1] +
                                covar[2] * tangent1[2],
                            covar[3] * tangent1[0] + covar[4] * tangent1[1] +
                                covar[5] * tangent1[2],
                            covar[6] * tangent1[0] + covar[7] * tangent1[1] +
                                covar[8] * tangent1[2]};
            double len = std::sqrt(next[0] * next[0] + next[1] * next[1] +
                                   next[2] * next[2]);
            if (len > 1e-10) {
                tangent1 = {next[0] / len, next[1] / len, next[2] / len};
            }
        }

        // Make tangent1 orthogonal to normal
        double dot = tangent1[0] * normal[0] + tangent1[1] * normal[1] +
                     tangent1[2] * normal[2];
        tangent1 = {tangent1[0] - dot * normal[0],
                    tangent1[1] - dot * normal[1],
                    tangent1[2] - dot * normal[2]};
        double len =
            std::sqrt(tangent1[0] * tangent1[0] + tangent1[1] * tangent1[1] +
                      tangent1[2] * tangent1[2]);
        if (len > 1e-10) {
            tangent1 = {tangent1[0] / len, tangent1[1] / len,
                        tangent1[2] / len};
        }

        // Compute second tangent direction
        Vector3 tangent2 = {normal[1] * tangent1[2] - normal[2] * tangent1[1],
                            normal[2] * tangent1[0] - normal[0] * tangent1[2],
                            normal[0] * tangent1[1] - normal[1] * tangent1[0]};

        // Compute curvature tensor in local tangent space
        double a = 0, b = 0, c = 0; // Tensor components in tangent basis

        for (const auto &[edge, tri_indices] : edge_triangles) {
            if (edge.v1 != i && edge.v2 != i)
                continue;
            if (tri_indices.size() != 2)
                continue; // Skip boundary

            size_t v_other = (edge.v1 == i) ? edge.v2 : edge.v1;

            // Project edge vector to tangent space
            Vector3 edge_vec = {vertices[v_other][0] - vertices[i][0],
                                vertices[v_other][1] - vertices[i][1],
                                vertices[v_other][2] - vertices[i][2]};

            double u = edge_vec[0] * tangent1[0] + edge_vec[1] * tangent1[1] +
                       edge_vec[2] * tangent1[2];
            double v = edge_vec[0] * tangent2[0] + edge_vec[1] * tangent2[1] +
                       edge_vec[2] * tangent2[2];

            // Find opposite vertices in the two triangles
            const auto &tri1 = triangles[tri_indices[0]];
            const auto &tri2 = triangles[tri_indices[1]];

            size_t v3 = -1, v4 = -1;
            for (int j = 0; j < 3; ++j) {
                if (tri1[j] != edge.v1 && tri1[j] != edge.v2)
                    v3 = tri1[j];
                if (tri2[j] != edge.v1 && tri2[j] != edge.v2)
                    v4 = tri2[j];
            }

            // Compute cotangent weights
            double cot_alpha =
                detail::cotangent(vertices[i], vertices[v_other], vertices[v3]);
            double cot_beta =
                detail::cotangent(vertices[i], vertices[v_other], vertices[v4]);
            double weight = (cot_alpha + cot_beta) / (2.0 * vertex_area[i]);

            // Accumulate tensor components
            a += weight * u * u;
            b += weight * u * v;
            c += weight * v * v;
        }

        // Solve eigenvalue problem for 2x2 matrix [a b; b c]
        double trace = a + c;
        double det = a * c - b * b;
        double discriminant = std::sqrt(std::max(0.0, trace * trace / 4 - det));
        double eval1 = trace / 2 + discriminant;
        double eval2 = trace / 2 - discriminant;

        // Compute eigenvectors in tangent space
        double evec1_u, evec1_v;
        if (std::abs(b) > 1e-10) {
            evec1_u = eval1 - c;
            evec1_v = b;
        } else {
            evec1_u = 1;
            evec1_v = 0;
        }
        double len1 = std::sqrt(evec1_u * evec1_u + evec1_v * evec1_v);
        if (len1 > 1e-10) {
            evec1_u /= len1;
            evec1_v /= len1;
        }

        // Principal directions in 3D
        principal_dir1[i] = {evec1_u * tangent1[0] + evec1_v * tangent2[0],
                             evec1_u * tangent1[1] + evec1_v * tangent2[1],
                             evec1_u * tangent1[2] + evec1_v * tangent2[2]};
        principal_dir2[i] = {-evec1_v * tangent1[0] + evec1_u * tangent2[0],
                             -evec1_v * tangent1[1] + evec1_u * tangent2[1],
                             -evec1_v * tangent1[2] + evec1_u * tangent2[2]};

        // Build 3x3 curvature tensor in global coordinates
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                curvature_tensors[i][j * 3 + k] =
                    eval1 * principal_dir1[i][j] * principal_dir1[i][k] +
                    eval2 * principal_dir2[i][j] * principal_dir2[i][k];
            }
        }
    }

    // Create and return Dataframe with results
    Dataframe results;
    results.add("mean_curvature", Serie<double>(mean_curvature));
    results.add("k1", Serie<double>(k1));
    results.add("k2", Serie<double>(k2));
    results.add("gaussian_curvature", Serie<double>(gaussian_curvature));
    results.add("curvature_tensor", Serie<Matrix3>(curvature_tensors));
    results.add("principal_direction1", Serie<Vector3>(principal_dir1));
    results.add("principal_direction2", Serie<Vector3>(principal_dir2));

    return results;
}

} // namespace df