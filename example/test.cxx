/*
 * Copyright (c) 2023 fmaerten@gmail.com
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
#include <iostream>
#include <cmath>
#include "../src/Serie.h"
#include "../src/Dataframe.h"
#include "../src/math.h"
#include "../src/utils.h"

void test1()
{
    std::cerr << "=====> test 1" << std::endl;

    df::Dataframe dataframe;
    dataframe.add("positions", df::Serie(3, Array({0, 1, 3, 2, 7, 8, 7, 6, 9, 7, 4, 3, 2, 8, 5}))); // vertices
    dataframe.add("indices", df::Serie(3, Array({0, 1, 2, 2, 3, 4, 7, 8, 5})));                     // triangles

    dataframe["positions"].dump();
    dataframe["indices"].dump();
    std::cerr << std::endl;

    dataframe["positions"]
        .map([](const Array &t, uint32_t i)
             {
            double norm = std::sqrt(t[0] * t[0] + t[1] * t[1] + t[2] * t[2]) ;
            return Array({norm}) ; })
        .forEach([](const Array &v, uint32_t i)
                 { std::cerr << "[" << i << "]: " << v[0] << std::endl; });

    std::cerr << std::endl;
}

void test2()
{
    std::cerr << "=====> test 2" << std::endl;

    df::Serie a(2, Array({1, 2, 3, 4}));
    df::Serie b(2, Array({4, 3, 2, 1}));
    df::Serie c(2, Array({2, 2, 1, 1}));

    Array alpha{2, 3, 4};

    try
    {
        df::info("weightedSum 1");
        df::weigthedSum(df::Series({a, b, c}), alpha).dump();

        df::info("add");
        df::add(df::Series({a, b})).dump();

        df::info("dot");
        df::dot(a, b).dump();

        df::info("negate");
        df::negate(a).dump();

        df::info("add(negate)");
        df::add(df::Series({a,df::negate(a)})).dump();

        df::info("weightedSum 2 throw");
        df::weigthedSum(df::Series({a, b}), alpha).dump();
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
    }

    std::cerr << std::endl;
}

void test_except()
{
    std::cerr << "=====> test except" << std::endl;

    df::Serie a(2, Array({1, 2, 3, 4}));
    df::Serie b(2, Array({4, 3, 2, 1, 3, 3}));

    try
    {
        df::add(df::Series({a, b})).dump();
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
    }

    // -------------------------

    df::Serie c(3, Array({4, 3, 2, 1, 3, 3}));

    try
    {
        df::add(df::Series({a, c})).dump();
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
    }
}

int main()
{
    test1();
    test2();
    test_except();
}
