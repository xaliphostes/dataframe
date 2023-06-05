#pragma once
#include <iostream>
#include "../src/Serie.h"

void assertEqual(double a, double b) {
    if (a != b) {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b << std::endl ;
        exit(-1);
    }
}

void assertArrayEqual(const df::Serie& serie, const Array& array) {
    if (serie.size() != array.size()) {
        std::cerr << "not same size" << std::endl ;
        exit(-1);
    }
    auto a = serie.asArray();
    for (uint32_t i=0; i<a.size(); ++i) {
        if (a[i] != array[i]) {
            std::cerr << "not same size: " << a[i] << " " << array[i] << std::endl ;
            exit(-1);
        }
    }
}

template <typename CB>
void shouldThrowError(CB&& cb) {
    try
    {
        cb();
        exit(-1);
    }
    catch (std::invalid_argument &e)
    {
        // ok
        df::error(e.what());
    }
}
