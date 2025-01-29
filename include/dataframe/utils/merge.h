/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>

namespace df {

// Merge mode enum
enum class MergeMode {
    Concatenate, // Append series one after another
    Interleave   // Alternate elements from each series
};

namespace detail {

// Implementation details for merging two series
template <typename T>
auto merge_two(const Serie<T> &first, const Serie<T> &second,
               MergeMode mode) -> Serie<T> {
    std::vector<T> result;

    if (mode == MergeMode::Concatenate) {
        result.reserve(first.size() + second.size());

        for (size_t i = 0; i < first.size(); ++i) {
            result.push_back(first[i]);
        }

        for (size_t i = 0; i < second.size(); ++i) {
            result.push_back(second[i]);
        }
    } else { // Interleave mode
        result.reserve(first.size() + second.size());

        const size_t maxSize = std::max(first.size(), second.size());

        for (size_t i = 0; i < maxSize; ++i) {
            if (i < first.size()) {
                result.push_back(first[i]);
            }
            if (i < second.size()) {
                result.push_back(second[i]);
            }
        }
    }

    return Serie<T>(result);
}

} // namespace detail

// Primary interface for merging two series
template <typename T>
auto merge(const Serie<T> &first, const Serie<T> &second,
           MergeMode mode = MergeMode::Concatenate) -> Serie<T> {
    return detail::merge_two(first, second, mode);
}

// Variadic merge function for three or more series
template <typename T, typename... Args>
auto merge(const Serie<T> &first, const Serie<T> &second,
           const Serie<T> &third, const Args &...rest) -> Serie<T> {
    // First merge the first two series
    auto intermediate =
        detail::merge_two(first, second, MergeMode::Concatenate);

    // Handle the third series and any remaining ones
    if constexpr (sizeof...(rest) == 0) {
        return detail::merge_two(intermediate, third, MergeMode::Concatenate);
    } else {
        return merge(intermediate, third, rest...);
    }
}

// Variadic merge function with mode specification
template <typename T, typename... Args>
auto merge(MergeMode mode, const Serie<T> &first, const Serie<T> &second,
           const Args &...rest) -> Serie<T> {
    // First merge the first two series with specified mode
    auto intermediate = detail::merge_two(first, second, mode);

    if constexpr (sizeof...(Args) == 0) {
        return intermediate;
    } else {
        // Recursively merge with remaining series using the same mode
        return merge(mode, intermediate, rest...);
    }
}

// Bind function for merge operations
template <typename T>
auto bind_merge(const Serie<T> &second,
                MergeMode mode = MergeMode::Concatenate) {
    return [&second, mode](const Serie<T> &first) {
        return merge(first, second, mode);
    };
}

// Helper function for creating an interleaved merge
template <typename T, typename... Args>
auto interleave(const Serie<T> &first, const Serie<T> &second,
                const Args &...rest) -> Serie<T> {
    return merge(MergeMode::Interleave, first, second, rest...);
}

// Bind function for interleave operations
template <typename T> auto bind_interleave(const Serie<T> &second) {
    return [&second](const Serie<T> &first) {
        return merge(MergeMode::Interleave, first, second);
    };
}

MAKE_OP(merge);
MAKE_OP(interleave);

} // namespace df