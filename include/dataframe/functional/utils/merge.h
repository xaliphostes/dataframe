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

#pragma once
#include <dataframe/Serie.h>
#include <functional>
#include <stdexcept>
#include <type_traits>

namespace df {
namespace utils {

/**
 * If concatenate is false, interleave is used to merge the series
 */
template <typename T>
GenSerie<T> merge(const GenSerie<T> &s1, const GenSerie<T> &s2,
                  bool concatenate = true) {
    if (s1.itemSize() != s2.itemSize()) {
        throw std::invalid_argument("Series must have same itemSize");
    }

    if (concatenate) {
        GenSerie<T> result(s1.itemSize(), s1.count() + s2.count());
        uint32_t idx = 0;

        for (uint32_t i = 0; i < s1.count(); ++i, ++idx) {
            if (s1.itemSize() > 1) {
                result.setArray(idx, s1.array(i));
            } else {
                result.setValue(idx, s1.value(i));
            }
        }
        for (uint32_t i = 0; i < s2.count(); ++i, ++idx) {
            if (s2.itemSize() > 1) {
                result.setArray(idx, s2.array(i));
            } else {
                result.setValue(idx, s2.value(i));
            }
        }
        return result;
    } else { // Interleave
        GenSerie<T> result(s1.itemSize(), s1.count() + s2.count());
        uint32_t idx = 0;

        for (uint32_t i = 0; i < std::max(s1.count(), s2.count()); ++i) {
            if (i < s1.count()) {
                if (s1.itemSize() > 1) {
                    result.setArray(idx++, s1.array(i));
                } else {
                    result.setValue(idx++, s1.value(i));
                }
            }
            if (i < s2.count()) {
                if (s2.itemSize() > 1) {
                    result.setArray(idx++, s2.array(i));
                } else {
                    result.setValue(idx++, s2.value(i));
                }
            }
        }
        return result;
    }
}

/**
 * @brief Merge a vector of series either by concatenation or interleaving
 */
template <typename T>
GenSerie<T> merge(const std::vector<GenSerie<T>> &series,
                  bool concatenate = true) {
    if (series.empty()) {
        return GenSerie<T>();
    }
    if (series.size() == 1) {
        return series[0];
    }

    // Check all series have same itemSize
    uint32_t itemSize = series[0].itemSize();
    for (const auto &s : series) {
        if (s.itemSize() != itemSize) {
            throw std::invalid_argument("All series must have same itemSize");
        }
    }

    // Calculate total count
    uint32_t total_count = 0;
    for (const auto &s : series) {
        total_count += s.count();
    }

    GenSerie<T> result(itemSize, total_count);

    if (concatenate) {
        // Concatenate mode
        uint32_t idx = 0;
        for (const auto &s : series) {
            for (uint32_t i = 0; i < s.count(); ++i) {
                if (itemSize > 1) {
                    result.setArray(idx, s.array(i));
                } else {
                    result.setValue(idx, s.value(i));
                }
                ++idx;
            }
        }
    } else {
        // Interleave mode
        uint32_t max_count = 0;
        for (const auto &s : series) {
            max_count = std::max(max_count, s.count());
        }

        uint32_t idx = 0;
        for (uint32_t i = 0; i < max_count; ++i) {
            for (const auto &s : series) {
                if (i < s.count()) {
                    if (itemSize > 1) {
                        result.setArray(idx, s.array(i));
                    } else {
                        result.setValue(idx, s.value(i));
                    }
                    ++idx;
                }
            }
        }
    }

    return result;
}

// Variadic merge
template <typename T, typename... Series>
GenSerie<T> merge(bool concatenate, const GenSerie<T>& first, const Series&... series) {
    std::vector<GenSerie<T>> vec{first, series...};
    return merge(vec, concatenate);
}

/*
template <typename T, typename... Series>
GenSerie<T> merge(bool concatenate, const GenSerie<T> &first,
                  const Series &...series) {
    std::array<size_t, sizeof...(series) + 1> itemSizes = {
        first.itemSize(), series.itemSize()...};
    if (!std::equal(itemSizes.begin() + 1, itemSizes.end(),
                    itemSizes.begin())) {
        throw std::invalid_argument("All series must have same itemSize");
    }

    uint32_t total_count = (first.count() + ... + series.count());
    GenSerie<T> result(first.itemSize(), total_count);

    if (concatenate) {
        uint32_t idx = 0;
        auto copy_serie = [&idx, &result](const auto &s) {
            for (uint32_t i = 0; i < s.count(); ++i) {
                if (s.itemSize() > 1) {
                    result.setArray(idx++, s.array(i));
                } else {
                    result.setValue(idx++, s.value(i));
                }
            }
        };

        copy_serie(first);
        (copy_serie(series), ...);
    } else { // Interleave
        uint32_t max_count = std::max({first.count(), series.count()...});
        uint32_t idx = 0;

        for (uint32_t i = 0; i < max_count; ++i) {
            auto copy_if_exists = [&idx, &result, i](const auto &s) {
                if (i < s.count()) {
                    if (s.itemSize() > 1) {
                        result.setArray(idx++, s.array(i));
                    } else {
                        result.setValue(idx++, s.value(i));
                    }
                }
            };

            copy_if_exists(first);
            (copy_if_exists(series), ...);
        }
    }

    return result;
}
*/

template <typename T> auto make_merge(bool concatenate = true) {
    return [concatenate](const auto &...series) {
        static_assert(sizeof...(series) >= 2,
                      "make_merge requires at least 2 series");
        return merge<T>(concatenate, series...);
    };
}

template <typename T>
auto make_merge_vector(bool concatenate = true) {
    return [concatenate](const std::vector<GenSerie<T>>& series) {
        return merge(series, concatenate);
    };
}

} // namespace utils
} // namespace df
