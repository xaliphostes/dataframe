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

#include "../../TEST.h"
#include <dataframe/geo/mesh/mesh.h>
#include <dataframe/core/pipe.h>

using namespace df;

TEST(mesh, construction_2d) {
    MSG("Testing 2D mesh construction");

    // Basic triangle
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    
    Mesh2D mesh(vertices, triangles);
    EXPECT_EQ(mesh.vertexCount(), 3);
    EXPECT_EQ(mesh.triangleCount(), 1);
    
    // Verify vertex access
    mesh.vertices().forEach([vertices](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, vertices[index], 1e-10);
    });

    EXPECT_ARRAY_EQ(mesh.triangles().asArray(), triangles.asArray());
}

TEST(mesh, construction_3d) {
    MSG("Testing 3D mesh construction");

    // Tetrahedron
    Serie<Vector3> vertices = {
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    Triangles triangles = {
        {0, 1, 2},
        {0, 1, 3},
        {1, 2, 3},
        {2, 0, 3}
    };
    
    Mesh3D mesh(vertices, triangles);
    EXPECT_EQ(mesh.vertexCount(), 4);
    EXPECT_EQ(mesh.triangleCount(), 4);
    
    mesh.vertices().forEach([vertices](const Vector3 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, vertices[index], 1e-10);
    });

    mesh.triangles().forEach([triangles](const iVector3 &v, size_t index) {
        EXPECT_ARRAY_EQ(v, triangles[index]);
    });
}

TEST(mesh, vertex_attributes) {
    MSG("Testing vertex attribute handling");

    // Create simple triangle mesh
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    // Add scalar attribute
    Serie<double> temperatures = {20.0, 25.0, 22.0};
    mesh.addVertexAttribute("temperature", temperatures);
    EXPECT_TRUE(mesh.hasVertexAttribute("temperature"));
    EXPECT_ARRAY_NEAR(mesh.vertexAttribute<double>("temperature").asArray(), 
                      temperatures.asArray(), 1e-10);

    // Add vector attribute
    Serie<Vector2> normals = {{0.0, 1.0}, {1.0, 0.0}, {-1.0, -1.0}};
    mesh.addVertexAttribute("normals", normals);
    EXPECT_TRUE(mesh.hasVertexAttribute("normals"));
    mesh.vertexAttribute<Vector2>("normals").forEach([normals](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, normals[index], 1e-10);
    });

    // Remove attribute
    mesh.removeVertexAttribute("temperature");
    EXPECT_FALSE(mesh.hasVertexAttribute("temperature"));
    EXPECT_TRUE(mesh.hasVertexAttribute("normals"));

    // Direct DataFrame access
    auto& vattr = mesh.vertexAttributes();
    Serie<double> weights = {1.0, 2.0, 3.0};
    vattr.add("weights", weights);
    EXPECT_TRUE(mesh.hasVertexAttribute("weights"));
}

TEST(mesh, triangle_attributes) {
    MSG("Testing triangle attribute handling");

    // Create simple triangle mesh
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    // Add scalar attribute
    Serie<double> areas = {0.5};
    mesh.addTriangleAttribute("area", areas);
    EXPECT_TRUE(mesh.hasTriangleAttribute("area"));
    EXPECT_ARRAY_NEAR(mesh.triangleAttribute<double>("area").asArray(), 
                      areas.asArray(), 1e-10);

    // Add vector attribute
    Serie<Vector2> centroids = {{0.33, 0.33}};
    mesh.addTriangleAttribute("centroid", centroids);
    EXPECT_TRUE(mesh.hasTriangleAttribute("centroid"));
    mesh.triangleAttribute<Vector2>("centroid").forEach([centroids](const Vector2 &v, size_t index) {
        EXPECT_ARRAY_NEAR(v, centroids[index], 1e-10);
    });

    // Direct DataFrame access
    auto& tattr = mesh.triangleAttributes();
    Serie<double> quality = {0.95};
    tattr.add("quality", quality);
    EXPECT_TRUE(mesh.hasTriangleAttribute("quality"));
}

TEST(mesh, error_cases) {
    MSG("Testing error handling");

    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles triangles = {{0, 1, 2}};
    Mesh2D mesh(vertices, triangles);

    // Invalid vertex indices
    Triangles invalid_triangles = {{0, 1, 3}};  // 3 is out of bounds
    EXPECT_THROW(Mesh2D(vertices, invalid_triangles), std::invalid_argument);

    // Wrong size vertex attribute
    Serie<double> wrong_size = {1.0, 2.0};  // Should be size 3
    EXPECT_THROW(mesh.addVertexAttribute("wrong", wrong_size), std::invalid_argument);

    // Wrong size triangle attribute
    Serie<double> wrong_tri_size = {1.0, 2.0};  // Should be size 1
    EXPECT_THROW(mesh.addTriangleAttribute("wrong", wrong_tri_size), std::invalid_argument);

    // Access non-existent attribute
    EXPECT_THROW(mesh.vertexAttribute<double>("nonexistent"), std::runtime_error);

    // Remove non-existent attribute
    EXPECT_THROW(mesh.removeVertexAttribute("nonexistent"), std::runtime_error);

    // Type mismatch
    Serie<double> values = {1.0, 2.0, 3.0};
    mesh.addVertexAttribute("test", values);
    EXPECT_THROW(mesh.vertexAttribute<Vector2>("test"), std::runtime_error);
}

TEST(mesh, empty_validation) {
    MSG("Testing empty mesh validation");

    // Empty vertex list
    Serie<Vector2> empty_vertices;
    Triangles triangles = {{0, 1, 2}};
    EXPECT_THROW(Mesh2D(empty_vertices, triangles).isValid(), std::invalid_argument);

    // Empty triangle list
    Serie<Vector2> vertices = {{0.0, 0.0}, {1.0, 0.0}, {0.0, 1.0}};
    Triangles empty_triangles;
    EXPECT_THROW(Mesh2D(vertices, empty_triangles).isValid(), std::invalid_argument);
}

RUN_TESTS()