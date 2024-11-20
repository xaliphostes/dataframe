#pragma once
#include <iostream>
#include <cmath>
#include <dataframe/utils/utils.h>

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
