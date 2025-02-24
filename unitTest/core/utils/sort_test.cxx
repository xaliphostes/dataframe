/*
 * Copyright (c) 2024-now fmaerten@gmail.com
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

#include "../../TEST.h"
#include <cmath>
#include <dataframe/map.h>
#include <dataframe/utils/sort.h>
#include <string>

using namespace df;

TEST(Sort, BasicAscending) {
    auto serie = Serie<double>({5.0, 2.0, 8.0, 1.0, 9.0});
    auto result = sort(serie);

    std::vector<double> expected{1.0, 2.0, 5.0, 8.0, 9.0};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Sort, BasicDescending) {
    auto serie = Serie<double>({5.0, 2.0, 8.0, 1.0, 9.0});
    auto result = sort(serie, SortOrder::DESCENDING);

    std::vector<double> expected{9.0, 8.0, 5.0, 2.0, 1.0};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Sort, EmptySerie) {
    auto serie = Serie<double>({});
    auto result = sort(serie);
    EXPECT_EQ(result.size(), 0);
}

TEST(Sort, SingleElement) {
    auto serie = Serie<double>({42.0});
    auto result = sort(serie);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 42.0);
}

TEST(Sort, DuplicateValues) {
    auto serie = Serie<double>({3.0, 1.0, 3.0, 2.0, 1.0});
    auto result = sort(serie);

    std::vector<double> expected{1.0, 1.0, 2.0, 3.0, 3.0};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Sort, CustomComparator) {
    auto serie = Serie<double>({-5.0, 2.0, -8.0, 1.0, -9.0});

    // Sort by absolute value
    auto result = sort(
        serie, [](double a, double b) { return std::abs(a) < std::abs(b); });

    std::vector<double> expected{1.0, 2.0, -5.0, -8.0, -9.0};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Sort, SortByKey) {
    struct Person {
        std::string name;
        int age;
        bool operator==(const Person &other) const {
            return name == other.name && age == other.age;
        }
    };

    Serie<Person> people(
        {Person{"Alice", 30}, Person{"Bob", 25}, Person{"Charlie", 35}});

    // Sort by age
    auto by_age = sort_by(people, [](const Person &p) { return p.age; });
    EXPECT_EQ(by_age[0].age, 25);
    EXPECT_EQ(by_age[1].age, 30);
    EXPECT_EQ(by_age[2].age, 35);

    // Sort by name
    auto by_name = sort_by(people, [](const Person &p) { return p.name; });
    EXPECT_EQ(by_name[0].name, "Alice");
    EXPECT_EQ(by_name[1].name, "Bob");
    EXPECT_EQ(by_name[2].name, "Charlie");
}

TEST(Sort, NaNHandling) {
    auto serie = Serie<double>({5.0, std::nan(""), 2.0, std::nan(""), 1.0});

    // NaN at end (default)
    auto result1 = sort_nan(serie);
    EXPECT_EQ(result1[0], 1.0);
    EXPECT_EQ(result1[1], 2.0);
    EXPECT_EQ(result1[2], 5.0);
    EXPECT_TRUE(std::isnan(result1[3]));
    EXPECT_TRUE(std::isnan(result1[4]));

    // NaN at beginning
    auto result2 = sort_nan(serie, SortOrder::ASCENDING, true);
    EXPECT_TRUE(std::isnan(result2[0]));
    EXPECT_TRUE(std::isnan(result2[1]));
    EXPECT_EQ(result2[2], 1.0);
    EXPECT_EQ(result2[3], 2.0);
    EXPECT_EQ(result2[4], 5.0);
}

TEST(Sort, AllNaN) {
    auto serie = Serie<double>({std::nan(""), std::nan(""), std::nan("")});
    auto result = sort_nan(serie);

    EXPECT_EQ(result.size(), 3);
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_TRUE(std::isnan(result[i]));
    }
}

TEST(Sort, MixedTypes) {
    auto serie = Serie<std::string>({"banana", "apple", "cherry"});
    auto result = sort(serie);

    std::vector<std::string> expected{"apple", "banana", "cherry"};
    COMPARE_SERIE_VECTOR(result, expected);
}

TEST(Sort, PipelineOperations) {
    auto serie = Serie<double>({5.0, 2.0, 8.0, 1.0, 9.0});

    // Basic sort through pipeline
    auto result1 = serie | bind_sort<double>();
    std::vector<double> expected1{1.0, 2.0, 5.0, 8.0, 9.0};
    COMPARE_SERIE_VECTOR(result1, expected1);

    // Custom comparator through pipeline
    auto result2 = serie | bind_sort_with([](double a, double b) {
                       return std::abs(a) < std::abs(b);
                   });
    COMPARE_SERIE_VECTOR(result2, expected1);

    // Chained operations
    auto result3 =
        serie | bind_sort<double>() | bind_map([](double x) { return x * 2; });
    std::vector<double> expected3{2.0, 4.0, 10.0, 16.0, 18.0};
    COMPARE_SERIE_VECTOR(result3, expected3);
}

TEST(Sort, StabilityTest) {
    struct Item {
        int key;
        std::string value;
        bool operator==(const Item &other) const {
            return key == other.key && value == other.value;
        }
    };

    Serie<Item> items({Item{2, "first"}, Item{1, "second"}, Item{2, "third"},
                       Item{1, "fourth"}});

    auto result = sort_by(items, [](const Item &item) { return item.key; });

    // Check if relative order of equal keys is preserved
    EXPECT_EQ(result[0].value, "second");
    EXPECT_EQ(result[1].value, "fourth");
    EXPECT_EQ(result[2].value, "first");
    EXPECT_EQ(result[3].value, "third");
}

// ========================= LARGE SERIES =========================

TEST(SortPerformance, LargeSerie_BasicSort) {
    const size_t size = 1'000'000;
    auto serie = RANDOM<double>(size, -1000.0, 1000.0);

    // Measure sorting time
    double time = TIMING([&]() {
        auto result = sort(serie);
        // Verify it's sorted
        for (size_t i = 1; i < result.size(); ++i) {
            EXPECT_TRUE(result[i - 1] <= result[i]);
        }
    });

    MSG("Sorting ", size, " elements took ", time, " ms");
}

TEST(SortPerformance, LargeSerie_NaN) {
    const size_t size = 1'000'000;
    auto serie = RANDOM<double>(size, -1000.0, 1000.0);

    // Insert NaN values at random positions
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> pos_dis(0, size - 1);
    const size_t nan_count = size / 10; // 10% NaN values

    for (size_t i = 0; i < nan_count; ++i) {
        size_t pos = pos_dis(gen);
        serie[pos] = std::nan("");
    }

    double time = TIMING([&]() {
        auto result = sort_nan(serie);
        // Verify non-NaN part is sorted
        size_t first_nan = 0;
        while (first_nan < result.size() && !std::isnan(result[first_nan])) {
            if (first_nan > 0) {
                EXPECT_TRUE(result[first_nan - 1] <= result[first_nan]);
            }
            ++first_nan;
        }
    });

    MSG("Sorting ", size, " elements with NaN took ", time, " ms");
}

TEST(SortPerformance, LargeSerie_CustomComparator) {
    const size_t size = 1'000'000;
    auto serie = RANDOM<double>(size, -1000.0, 1000.0);

    double time = TIMING([&]() {
        auto result = sort(serie, [](double a, double b) {
            return std::abs(a) < std::abs(b);
        });
        // Verify it's sorted by absolute value
        for (size_t i = 1; i < result.size(); ++i) {
            EXPECT_TRUE(std::abs(result[i - 1]) <= std::abs(result[i]));
        }
    });

    MSG("Sorting ", size, " elements with custom comparator took ", time,
        " ms");
}

TEST(SortPerformance, LargeSerie_KeyFunction) {
    struct ComplexData {
        double primary;
        double secondary;
        std::string label;
    };

    const size_t size = 100'000; // Smaller due to more complex data
    std::vector<ComplexData> data;
    data.reserve(size);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(-1000.0, 1000.0);

    for (size_t i = 0; i < size; ++i) {
        data.push_back(
            ComplexData{dis(gen), dis(gen), "Label" + std::to_string(i)});
    }

    Serie<ComplexData> serie(data);

    double time = TIMING([&]() {
        auto result =
            sort_by(serie, [](const ComplexData &d) { return d.primary; });
        // Verify it's sorted by primary key
        for (size_t i = 1; i < result.size(); ++i) {
            EXPECT_TRUE(result[i - 1].primary <= result[i].primary);
        }
    });

    MSG("Sorting ", size, " complex elements took ", time, " ms");
}

TEST(SortPerformance, PreSortedData) {
    const size_t size = 1'000'000;
    std::vector<double> data(size);
    for (size_t i = 0; i < size; ++i) {
        data[i] = static_cast<double>(i);
    }
    Serie<double> serie(data);

    // Sort already sorted data
    double time_sorted = TIMING([&]() { auto result = sort(serie); });
    MSG("Sorting ", size, " pre-sorted elements took ", time_sorted, " ms");

    // Sort reverse sorted data
    std::reverse(data.begin(), data.end());
    Serie<double> reverse_serie(data);
    double time_reverse = TIMING([&]() { auto result = sort(reverse_serie); });
    MSG("Sorting ", size, " reverse-sorted elements took ", time_reverse,
        " ms");
}

TEST(SortPerformance, PipelineWithLargeSerie) {
    const size_t size = 1'000'000;
    auto serie = RANDOM<double>(size, -1000.0, 1000.0);

    double time = TIMING([&]() {
        auto result = serie | bind_sort<double>() |
                      bind_map([](double x) { return x * 2; });

        // Verify result is sorted and transformed
        for (size_t i = 1; i < result.size(); ++i) {
            EXPECT_TRUE(result[i - 1] <= result[i]);
            // Check if values are doubled
            auto original = result[i] / 2.0;
            EXPECT_TRUE(original >= -1000.0 && original <= 1000.0);
        }
    });

    MSG("Pipeline sort+map on ", size, " elements took ", time, " ms");
}

RUN_TESTS();