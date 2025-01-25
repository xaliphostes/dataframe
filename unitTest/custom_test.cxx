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
#include <dataframe/functional/filter.h>
#include <dataframe/functional/map.h>

// Define a custom type
struct Position {
    double x, y, z;

    // Optional: Operators for serie operations
    Position operator+(const Position &other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Position operator*(double scale) const {
        return {x * scale, y * scale, z * scale};
    }
};

std::ostream &operator<<(std::ostream &o, const Position &p) {
    o << "(" << p.x << ", " << p.y << ", " << p.z << ")";
    return o;
}

TEST(custom_test, CustomType) {
    // Direct instantiation with custom types
    df::GenSerie<Position> positions(
        1, {Position{1, 0, 0}, Position{0, 2, 0}, Position{0, 0, 3}});

    // Operations work naturally with the custom type
    auto doubled = df::make_map(
        [](const Position &p, uint32_t) { return p * 2.0; })(positions);
    df::print(doubled);

    // Filter based on custom type properties
    auto filtered = df::filter(
        [](const Position &p, uint32_t) {
            return (p.x * p.x + p.y * p.y + p.z * p.z) < 2.0;
        },
        positions);
    df::print(filtered);
}

RUN_TESTS();