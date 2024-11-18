#include <dataframe/algos/pipe.h>
#include <dataframe/algos/zip.h>
#include <dataframe/algos/map.h>
#include <dataframe/Serie.h>

// A map function for non-scalar Series:
void _1() {
    df::Serie s(3, {1,2,3, 4,5,6});  // 2 items of size 3
    auto result = map(s, [](const Array& v, uint32_t i) {
        Array out(v.size());
        for(size_t j = 0; j < v.size(); ++j) {
            out[j] = v[j] * 2;
        }
        return out;
    });
}

// A mapScalar function for scalar Series:
void _2() {
    df::Serie s(1, {1, 2, 3, 4});
    auto result = df::mapScalar(s, [](double v, uint32_t i) {
        return v * 2;
    });
}

// A makeMap function that creates reusable mapping functions for non-scalar Series:
void _3() {
    auto doubler = df::makeMap([](const Array& v, uint32_t) {
        Array out(v.size());
        for(size_t j = 0; j < v.size(); ++j) {
            out[j] = v[j] * 2;
        }
        return out;
    });

    df::Serie s1(3, {1,2,3, 4,5,6});
    df::Serie s2(3, {7,8,9, 10,11,12});

    auto result1 = doubler(s1);
    auto result2 = doubler(s2);
}

// A makeMapScalar function that creates reusable mapping functions for scalar Series:
void _4() {
    auto doubler = df::makeMapScalar([](double v, uint32_t) {
        return v * 2;
    });

    df::Serie s1(1, {1, 2, 3});
    df::Serie s2(1, {4, 5, 6});

    auto result1 = doubler(s1);
    auto result2 = doubler(s2);
}

// These functions can be combined with the pipe and zip functions:
void _5() {
    df::Serie s1(1, {1, 2, 3});
    df::Serie s2(2, {4,5, 6,7, 8,9});
    
    // Combine multiple operations
    auto result = df::pipe(
        df::zip(s1, s2),
        df::makeMap([](const Array& v, uint32_t) {
            Array out(v.size());
            for(size_t j = 0; j < v.size(); ++j) {
                out[j] = v[j] * 2;
            }
            return out;
        })
    );
    
    // Create reusable transformations
    auto pipeline = df::makePipe(
        df::makeMapScalar([](double v, uint32_t) { return v * 2; }),
        df::makeMapScalar([](double v, uint32_t) { return v + 1; })
    );
    
    auto transformed = pipeline(s1);
}

int main() {
    _1();
    _2();
    _3();
    _4();
    _5();
}
