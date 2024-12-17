#include "TEST.h"
#include <dataframe/iterator.h>
#include <vector>

df::Serie s1(1, {1, 2, 3});          // Scalar serie
df::Serie s2(3, {1, 2, 3, 4, 5, 6}); // Vector serie

TEST(SerieIterator, BasicIteration) {
    // Using range-based for loop with scalar serie
    for (auto value : df::SerieIterator<double>(s1, 0)) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    // Using range-based for loop with vector serie
    for (auto value : df::SerieIterator<Array>(s2, 0)) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
}
TEST(SerieIterator, algo) {
    // Using with standard algorithms
    std::vector<double> values;
    std::copy(df::begin<double>(s1), df::end<double>(s1),
              std::back_inserter(values));
}

TEST(SerieIterator, forEachIter) {
    // Using the new forEachIter
    std::cout << "forEachIter s1" << std::endl;
    df::forEachIter([](const auto &value) { std::cout << value << "\n"; }, s1);
    std::cout << std::endl;

    std::cout << "forEachIter s2" << std::endl;
    df::forEachIter([](const auto &value) { std::cout << value << "\n"; }, s2);
    std::cout << std::endl;
}

RUN_TESTS()