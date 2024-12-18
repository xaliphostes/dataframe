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
#include <dataframe/Serie.h>
#include <dataframe/functional/algebra/eigen.h>
#include <dataframe/functional/math/equals.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/utils/apply.h>
#include <dataframe/functional/utils/print.h>
#include <dataframe/functional/utils/zip.h>

df::Serie someOperation(const df::Serie &s1, const df::Serie &s2) {
    return df::utils::zip(s1, s2);
};

TEST(pipe, basic) {
    df::Serie s(1, {1, 2, 3, 4, 5});
    df::Serie solution(1, {6, 8, 10});

    // Using pipe directly
    auto result1 = pipe(

        s,

        [](const df::Serie &s) {
            return s.map([](double v, uint32_t) { return v * 2; });
        },

        [](const df::Serie &s) {
            return s.filter([](double v, uint32_t) { return v > 4; });
        }

    );
    assertCondition(df::math::equals(solution, result1));
}

// Creating and using a reusable pipeline
TEST(pipe, make1) {
    df::Serie s(1, {1, 2, 3, 4, 5});
    df::Serie solution(1, {6, 8, 10});

    auto doubleAndFilter = df::make_pipe(
        [](const df::Serie &s) {
            return s.map([](double v, uint32_t) { return v * 2; });
        },
        [](const df::Serie &s) {
            return s.filter([](double v, uint32_t) { return v > 4; });
        });

    auto result2 = doubleAndFilter(s);
    assertCondition(df::math::equals(solution, result2));
}

TEST(pipe, make2) {
    df::Serie s(1, {1, 2, 3, 4, 5});
    df::Serie solution(1, {6, 8, 10});

    auto doubleAndFilter = df::make_pipe(
        df::make_map([](double v, uint32_t) { return v * 2; }),
        df::make_filter([](double v, uint32_t) { return v > 4; }));

    auto result2 = doubleAndFilter(s);
    assertCondition(df::math::equals(solution, result2));

    df::Serie s2(2, {5, 6, 7, 8});
    auto result3 = doubleAndFilter(s2);
    df::utils::print(result3);
}

// ---------------------------------------------------

TEST(pipe, serie) {
    df::Serie serie(6, {1, 2, 3, 4, 5, 6});

    auto result = df::pipe(

        serie,

        [](const df::Serie &s) { return df::algebra::eigenSystem(s); },

        [](const auto &tuple) { return std::get<0>(tuple); }

    );

    assertEqual<int>(result.itemSize(), 3);
    assertEqual<int>(result.count(), 1);
    assertEqual<int>(result.dimension(), 3);
    assertSerieEqual(result, df::Serie(3, {11.3448, 0.170914, -0.515728}),
                     1e-4);
}

TEST(pipe, pair) {
    df::Serie serie1(1, {1, 2, 3});
    df::Serie serie2(2, {1, 2, 3, 4, 5, 6});
    auto result = df::pipe(

        std::make_pair(serie1, serie2),

        [](const auto &pair) {
            const auto &[s1, s2] = pair;
            return someOperation(s1, s2);
        }

    );

    assertEqual<int>(result.itemSize(), 3);
    assertEqual<int>(result.count(), 3);
    assertEqual<int>(result.dimension(), 3);
    assertSerieEqual(result, df::Serie(3, {1, 1, 2, 2, 3, 4, 3, 5, 6}));
}

// With anything else (a new struct for example)
TEST(pipe, anything) {
    df::Serie positions(3, {0, 0, 0, 1, 0, 0, 2, 0, 0, 3, 0, 0});
    df::Serie stress(6, {1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6,
                         1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6});

    struct Data {
        df::Serie stress;
        df::Serie positions;
    };

    auto result = df::pipe(

        Data{stress, positions},

        [](const Data &data) {
            return someOperation(data.stress, data.positions);
        }

    );

    assertEqual<int>(result.itemSize(), 9);
    assertEqual<int>(result.count(), 4);
    assertEqual<int>(result.dimension(), 3);
    assertSerieEqual(
        result,
        df::Serie(9, {1, 2, 3, 4, 5, 6, 0, 0, 0, 1, 2, 3, 4, 5, 6, 1, 0, 0,
                      1, 2, 3, 4, 5, 6, 2, 0, 0, 1, 2, 3, 4, 5, 6, 3, 0, 0}));
}

RUN_TESTS()