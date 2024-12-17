#pragma once
#include <dataframe/Serie.h>

namespace df {

/**
 * @brief An iterator for a Serie.
 * This implementation:
 * - Provides a random access iterator
 * - Supports both scalar and vector Series
 * - Works with STL algorithms
 * - Maintains type safety
 * - Offers seamless integration with modern C++ range-based for loops
 *
 * The iterator implementation allows for more idiomatic C++ code while
 * maintaining the efficiency of direct Serie access.
 * @example
 * ```c++
 * Serie s(1, {1, 2, 3});
 * for (auto v : s) {
 *     std::cerr << v << std::endl;
 * }
 * // Display:
 * // 1
 * // 2
 * // 3
 * ```
 * @example
 * ```c++
 * Serie s1(1, {1, 2, 3}); // Scalar serie
 * Serie s2(3, {1,2,3, 4,5,6}); // Vector serie
 *
 * // Using range-based for loop with scalar serie
 * for (auto value : df::SerieIterator<double>(s1, 0)) {
 *     std::cout << value << " ";
 * }
 *
 * // Using range-based for loop with vector serie
 * for (auto value : df::SerieIterator<Array>(s2, 0)) {
 *     std::cout << value << " ";
 * }
 *
 * // Using with standard algorithms
 * std::vector<double> values;
 * std::copy(df::begin<double>(s1), df::end<double>(s1),
 * std::back_inserter(values));
 *
 * // Using the new forEachIter
 * df::forEachIter([](const auto& value) {
 *     std::cout << value << "\n";
 * }, s1);
 * ```
 */
template <typename T> class SerieIterator {
  public:
    // Iterator traits
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    SerieIterator(Serie &serie, uint32_t pos) : serie_(serie), pos_(pos) {}

    // Add these methods to support range-based for loops
    SerieIterator begin() const { return SerieIterator(serie_, 0); }
    SerieIterator end() const { return SerieIterator(serie_, serie_.count()); }

    // Iterator operations
    T operator*() const {
        if constexpr (std::is_same_v<T, double>) {
            return serie_.template get<double>(pos_);
        } else {
            return serie_.template get<Array>(pos_);
        }
    }

    SerieIterator &operator++() {
        ++pos_;
        return *this;
    }
    SerieIterator operator++(int) {
        SerieIterator tmp = *this;
        ++pos_;
        return tmp;
    }
    SerieIterator &operator--() {
        --pos_;
        return *this;
    }
    SerieIterator operator--(int) {
        SerieIterator tmp = *this;
        --pos_;
        return tmp;
    }

    bool operator==(const SerieIterator &other) const {
        return pos_ == other.pos_;
    }
    bool operator!=(const SerieIterator &other) const {
        return pos_ != other.pos_;
    }

  private:
    Serie &serie_;
    uint32_t pos_;
};

// Iterator support methods for Serie
template <typename T = double> SerieIterator<T> begin(Serie &serie) {
    return SerieIterator<T>(serie, 0);
}

template <typename T = double> SerieIterator<T> end(Serie &serie) {
    return SerieIterator<T>(serie, serie.count());
}

// Enhanced forEach with iterator support
template <typename F> void forEachIter(F &&callback, Serie &serie) {
    if (serie.itemSize() == 1) {
        for (auto it = begin<double>(serie); it != end<double>(serie); ++it) {
            callback(*it);
        }
    } else {
        for (auto it = begin<Array>(serie); it != end<Array>(serie); ++it) {
            callback(*it);
        }
    }
}

} // namespace df
