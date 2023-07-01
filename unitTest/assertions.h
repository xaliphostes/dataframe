#pragma once
#include <iostream>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>

void assertCondition(bool cond, const String& msg="") {
    if ( !cond ) {
        if (msg.size()>0) {
            std::cout << "Condition failed (msg = " << msg << ")!" << std::endl ;
        }
        else {
            std::cout << "Condition failed!" << std::endl ;
        }
        exit(-1);
    }
}

template <typename T>
void assertEqual(const T &a, const T &b) {
    if (a != b) {
        std::cout << "Value " << a << " is NOT EQUAL to value " << b << std::endl ;
        exit(-1);
    }
}

void assertArrayEqual(const Array& serie, const Array& array) {
    if (serie.size() != array.size()) {
        std::cerr << "not same size" << std::endl ;
        exit(-1);
    }
    const auto& a = serie;
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
void shouldNotThrowError(CB&& cb) {
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
