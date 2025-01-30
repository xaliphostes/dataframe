#include "TEST.h"
#include <dataframe/Serie.h>
#include <dataframe/map.h>
#include <dataframe/utils/compose.h>

TEST(compose, identity) {
    df::Serie<int> serie{1, 2, 3};
    auto result = df::compose(serie);
    COMPARE_SERIES(result, serie);
}

TEST(compose, with_one_transform) {
    df::Serie<int> serie{1, 2, 3};
    auto result = df::compose(serie, [](const df::Serie<int> &s) {
        return s.map([](int x, size_t) { return x * 2; });
    });
    df::Serie<int> expected{2, 4, 6};
    COMPARE_SERIES(result, expected);
}

TEST(compose, with_two_transforms) {
    df::Serie<int> serie{1, 2, 3};
    auto result = df::compose(
        serie,
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x * 2; });
        },
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x + 1; });
        });
    df::Serie<int> expected{4, 6, 8}; // (x+1)*2
    COMPARE_SERIES(result, expected);
}

TEST(compose, with_three_transforms) {
    df::Serie<int> serie{1, 2, 3};
    auto result = df::compose(
        serie,
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x * x; });
        },
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x * 2; });
        },
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x + 1; });
        });
    df::Serie<int> expected{16, 36, 64};

    MSG("result")
    df::print(result);
    MSG("expected")
    df::print(expected);
    
    COMPARE_SERIES(result, expected);
}

TEST(compose, make_compose_single) {
    df::Serie<int> serie{1, 2, 3};
    auto transform = df::make_compose([](const df::Serie<int> &s) {
        return s.map([](int x, size_t) { return x * 2; });
    });

    auto result = transform(serie);
    df::Serie<int> expected{2, 4, 6};
    COMPARE_SERIES(result, expected);
}

TEST(compose, make_compose_multiple) {
    df::Serie<int> serie{1, 2, 3};
    auto transform = df::make_compose(
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x * 2; });
        },
        [](const df::Serie<int> &s) {
            return s.map([](int x, size_t) { return x + 1; });
        });

    auto result = transform(serie);
    df::Serie<int> expected{4, 6, 8}; // (x+1)*2
    COMPARE_SERIES(result, expected);
}

RUN_TESTS();