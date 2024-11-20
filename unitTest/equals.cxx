#include <dataframe/operations/math/equals.h>
#include <dataframe/Serie.h>

using namespace df;

int main()
{
    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 2, 3});
        bool equal = equals(s1, s2); // true
    }

    {
        Serie s1(1, {1.0, 2.0, 3.0});
        Serie s2(1, {1.0000001, 2.0, 3.0});
        bool equal = approximatelyEquals(s1, s2, 1e-6); // true
    }

    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 2, 3});
        Serie s3(1, {1, 2, 3});
        bool allEqual = equalsAll(s1, s2, s3); // true
    }

    // {
    //     Serie s1(1, {1.0, 2.0, 3.0});
    //     Serie s2(1, {1.0000001, 2.0, 3.0});
    //     Serie s3(1, {1.0000002, 2.0, 3.0});
    //     bool allEqual = approximatelyEqualsAll(s1, s2, s3); // true
    // }

    {
        Serie s1(1, {1, 2, 3});
        Serie s2(1, {1, 4, 3});
        auto mask = equalityMask(s1, s2); // Serie(1, {1, 0, 1})
    }

    {
        Serie s1(1, {1.0, 2.0, 3.0});
        Serie s2(1, {1.0000001, 4.0, 3.0});
        auto mask = approximateEqualityMask(s1, s2, 1e-6); // Serie(1, {1, 0, 1})
    }
}