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

#pragma once
#include <iostream>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <dataframe/types.h>

#define START_TEST(name) void name() { std::cerr << "Start " << #name  << " test" << std::endl;
#define END_TEST(name) std::cerr << #name  << " test OK\n" << std::endl;}

void message(const String &msg) {

}

void assertCondition(bool cond, const String &msg = "")
{
    if (!cond)
    {
        if (msg.size() > 0)
        {
            std::cout << "Condition failed (msg = " << msg << ")!" << std::endl;
        }
        else
        {
            std::cout << "Condition failed!" << std::endl;
        }
        exit(-1);
    }
}

template <typename T>
void assertEqual(const T &a, const T &b)
{
    if (a != b)
    {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b << std::endl;
        exit(-1);
    }
}

void assertDoubleEqual(double a, double b, double tol = 1e-7)
{
    if (std::fabs(a - b) > tol)
    {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b << std::endl;
        exit(-1);
    }
}

void assertArrayEqual(const Array &serie, const Array &array, double tol = 1e-7)
{
    if (serie.size() != array.size())
    {
        std::cerr << "not same size: a=" << serie.size() << ", b=" << array.size() << std::endl;
        exit(-1);
    }
    const auto &a = serie;
    for (uint32_t i = 0; i < a.size(); ++i)
    {
        if (std::fabs(a[i] - array[i])>tol)
        {
            std::cerr << "not same values: " << a[i] << " " << array[i] << ", diff=" << std::fabs(a[i] - array[i]) << std::endl;
            exit(-1);
        }
    }
}

void assertSerieEqual(const df::Serie &s1, const Array &s2, double tol = 1e-7)
{
    assertArrayEqual(s1.asArray(), s2, tol);
}

void assertSerieEqual(const df::Serie &s1, const df::Serie &s2, double tol = 1e-7)
{
    assertArrayEqual(s1.asArray(), s2.asArray(), tol);
}

template <typename CB>
void shouldThrowError(CB &&cb)
{
    try
    {
        cb();
        std::cerr << "not throwing an error!\n";
        exit(-1);
    }
    catch (std::invalid_argument &e)
    {
        // ok
        df::error(e.what());
    }
}

template <typename CB>
void shouldNotThrowError(CB &&cb)
{
    try
    {
        cb();
    }
    catch (std::invalid_argument &e)
    {
        df::error(e.what());
        exit(-1);
    }
}
