/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#include "../../TEST.h"
#include <dataframe/core/zip.h>
#include <dataframe/utils/print.h>

using namespace df;

TEST(Serie, PrintMultipleSeries) {
    MSG("Testing printing multiple series");

    // Create test series
    Serie<double> s1{1.23456, 2.34567, 3.45678};
    Serie<double> s2{10.1234, 20.2345, 30.3456};
    Serie<double> s3{100.123, 200.234, 300.345};

    std::cout << "\nTest 1: Print single series with default precision\n";
    print(s1);

    std::cout << "\nTest 2: Print single series with custom precision\n";
    print(s1, 2);

    std::cout << "\nTest 3: Print multiple series with default precision\n";
    print(std::vector<Serie<double>>{s1, s2, s3});

    std::cout << "\nTest 4: Print multiple series with custom precision\n";
    print(std::vector<Serie<double>>{s1, s2, s3}, 2);

    // Test with integer series
    Serie<int> i1{1, 2, 3};
    Serie<int> i2{10, 20, 30};
    std::cout << "\nTest 5: Print multiple integer series\n";
    print(std::vector<Serie<int>>{i1, i2});

    // Test pipeline operation
    std::cout << "\nTest 6: Print using pipeline operation\n";
    s1 | bind_print<double>(3);
}

TEST(Serie, PrintZippedSeries) {
    MSG("Testing printing of zipped series");

    // Create test series
    Serie<double> s1{1.23456, 2.34567, 3.45678};
    Serie<int> s2{10, 20, 30};
    Serie<float> s3{100.123f, 200.234f, 300.345f};

    print(s1);

    std::cout << "\nTest 1: Print zipped series with default precision\n";
    auto zipped = zip(s1, s2, s3);
    print(zipped);

    std::cout << "\nTest 2: Print zipped series with custom precision\n";
    print(zipped, 2);

    std::cout << "\nTest 3: Print using pipeline operation\n";
    zipped | bind_print_zipped<double, int, float>(3);

    // Test with different combinations
    std::cout << "\nTest 4: Print two series zipped\n";
    print(zip(s1, s2), 3);

    // Test with expressions
    std::cout << "\nTest 5: Print zipped series with expressions\n";
    auto scaled_s1 = s1.map([](double x, size_t) { return x * 2; });
    print(zip(s1, scaled_s1), 3);
}

RUN_TESTS()
