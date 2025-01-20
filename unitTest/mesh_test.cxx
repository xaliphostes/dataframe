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

#include "TEST.h"
#include <dataframe/functional/algebra/norm.h>
#include <dataframe/functional/forEach.h>
#include <dataframe/functional/geo/Mesh.h>
#include <dataframe/functional/geo/normals.h>

void exportTs(const df::geo::Mesh &mesh, std::ostream &out = std::cout) {
    out << "GOCAD TSurf 1.0\nHEADER {\n  name: surface\n}" << std::endl;

    if (mesh.vertexAttributes().size() > 0) {
        out << std::endl << "PROPERTIES";
        std::vector<int> sizes;
        for (auto &attribute : mesh.vertexAttributes()) {
            out << " " << attribute.first;
            sizes.push_back(attribute.second.itemSize());
        }
        out << std::endl;
        out << "ESIZES";
        for (auto i : sizes) {
            out << " " << i;
        }
        out << std::endl << std::endl;
    }

    df::forEach(
        [&](const df::geo::Mesh::Vertex &v, uint32_t index) {
            out << "VRTX " << index << " " << v[0] << " " << v[1] << " " << v[2]
                << std::endl;

            for (auto &attribute : mesh.vertexAttributes()) {
                // TODO
            }
        },
        mesh.vertices());

    df::forEach(
        [&](const df::geo::Mesh::Element &e, uint32_t) {
            out << "TRGL " << e[0] << " " << e[1] << " " << e[2] << std::endl;
        },
        mesh.indices());

    out << "END" << std::endl;
}

TEST(Mesh, basic) {

    auto indices = df::geo::Indices(3, {0, 1, 2, 0, 2, 3});
    auto positions =
        df::geo::Positions(3, {0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1});

    df::geo::Mesh mesh(indices, positions);

    mesh.addVertexAttribute(
        "color", df::geo::Attribute(3, {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1}));
    mesh.addElementAttribute("normals", df::geo::normals(positions, indices));
    mesh.addVertexAttribute("norm2", df::algebra::norm2(positions));
    mesh.addVertexAttribute("norm", df::algebra::norm(positions));

    mesh.print();
    std::cout << std::endl;

    std::cout << "---------------------------" << std::endl;
    exportTs(mesh);
    std::cout << "---------------------------" << std::endl << std::endl;
}

RUN_TESTS()