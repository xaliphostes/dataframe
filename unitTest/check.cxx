
#include "assertions.h"
#include <dataframe/Serie.h>
#include <dataframe/functional/conditional/check.h>
#include <dataframe/functional/math/normalize.h>
#include <iostream>

using namespace df;

int main()
{
    Serie s1(1, { 1, -2, 3 });
    auto isNegative = check(s1, [](double v, uint32_t) { return v < 0; });
    assertSerieEqual(isNegative, Array{0, 1, 0});

    // Pour une Serie vectorielle
    Serie s2(3, { 1, 2, 3, -4, 5, 6 });
    auto firstIsNegative = check(s2, [](const Array& v, uint32_t) { return v[0] < 0; });
    assertSerieEqual(firstIsNegative, Array{0, 1});

    auto checkFirstNegative = make_check([](const auto& v, uint32_t) {
        if constexpr (std::is_same_v<std::decay_t<decltype(v)>, double>) {
            return v < 0;
        } else {
            return v[0] < 0;
        }
    });
}