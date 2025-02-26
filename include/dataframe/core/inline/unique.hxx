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

#include <algorithm>
#include <functional>
#include <type_traits>
#include <unordered_set>
#include <dataframe/core/execution_policy.h>
#include <vector>

namespace df {

namespace detail {

// Helper struct to create hash for any type if possible
template <typename T, typename = void> struct HashHelper {
    // Default case for types without std::hash specialization
    static constexpr bool canHash() { return false; }

    template <typename Container> static void addToSet(Container &, const T &) {
        // Do nothing for non-hashable types
    }

    template <typename Container> static bool contains(Container &, const T &) {
        return false;
    }
};

// Specialization for types with std::hash specialization
template <typename T>
struct HashHelper<
    T, std::void_t<decltype(std::declval<std::hash<T>>()(std::declval<T>()))>> {
    static constexpr bool canHash() { return true; }

    template <typename Container>
    static void addToSet(Container &container, const T &value) {
        container.insert(value);
    }

    template <typename Container>
    static bool contains(Container &container, const T &value) {
        return container.find(value) != container.end();
    }
};

// Implementation with hash-based approach (faster for hashable types)
template <typename T> Serie<T> unique_impl_hash(const Serie<T> &serie) {
    if (serie.empty()) {
        return serie;
    }

    std::unordered_set<T> seen;
    std::vector<T> result;
    result.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        const T &value = serie[i];
        if (seen.find(value) == seen.end()) {
            seen.insert(value);
            result.push_back(value);
        }
    }

    return Serie<T>(result);
}

// Implementation with vector search (slower but works for non-hashable types)
template <typename T> Serie<T> unique_impl_linear(const Serie<T> &serie) {
    if (serie.empty()) {
        return serie;
    }

    std::vector<T> result;
    result.reserve(serie.size());

    for (size_t i = 0; i < serie.size(); ++i) {
        const T &value = serie[i];
        bool found = false;

        for (size_t j = 0; j < result.size(); ++j) {
            if (value == result[j]) {
                found = true;
                break;
            }
        }

        if (!found) {
            result.push_back(value);
        }
    }

    return Serie<T>(result);
}

} // namespace detail

template <typename T>
Serie<T> unique(const Serie<T> &serie, ExecutionPolicy exec) {
    if (serie.empty()) {
        return serie;
    }

    // Choose implementation based on whether T is hashable
    if constexpr (detail::HashHelper<T>::canHash()) {
        return detail::unique_impl_hash(serie);
    } else {
        return detail::unique_impl_linear(serie);
    }
}

template <typename T, typename KeyFunc>
Serie<T> unique_by(const Serie<T> &serie, KeyFunc keyFn, ExecutionPolicy exec) {
    if (serie.empty()) {
        return serie;
    }

    // Get the return type of the key function
    using KeyType = std::invoke_result_t<KeyFunc, T>;

    // Choose hash-based or linear approach based on key type
    if constexpr (detail::HashHelper<KeyType>::canHash()) {
        std::unordered_set<KeyType> seen_keys;
        std::vector<T> result;
        result.reserve(serie.size());

        for (size_t i = 0; i < serie.size(); ++i) {
            const T &value = serie[i];
            KeyType key = keyFn(value);

            if (seen_keys.find(key) == seen_keys.end()) {
                seen_keys.insert(key);
                result.push_back(value);
            }
        }

        return Serie<T>(result);
    } else {
        // Fallback for non-hashable key types
        std::vector<KeyType> seen_keys;
        std::vector<T> result;
        result.reserve(serie.size());

        for (size_t i = 0; i < serie.size(); ++i) {
            const T &value = serie[i];
            KeyType key = keyFn(value);
            bool found = false;

            for (const auto &seen_key : seen_keys) {
                if (key == seen_key) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                seen_keys.push_back(key);
                result.push_back(value);
            }
        }

        return Serie<T>(result);
    }
}

template <typename T> auto bind_unique(ExecutionPolicy exec) {
    return [exec](const Serie<T> &serie) { return unique(serie, exec); };
}

template <typename KeyFunc>
auto bind_unique_by(KeyFunc keyFn, ExecutionPolicy exec) {
    return [keyFn, exec](const auto &serie) {
        return unique_by(serie, keyFn, exec);
    };
}

} // namespace df