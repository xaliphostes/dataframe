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
 */

namespace df {
namespace nan {

template <typename T> class NanOperations {
  public:
    // Set NaN at specific indices
    static Serie<T> set_nan_at(const Serie<T> &serie,
                               const std::vector<size_t> &indices) {
        std::vector<T> result(serie.data());
        for (size_t idx : indices) {
            if (idx < result.size()) {
                result[idx] = std::numeric_limits<T>::quiet_NaN();
            }
        }
        return Serie<T>(result);
    }

    // Set NaN where condition is true
    template <typename F>
    static Serie<T> set_nan_where(const Serie<T> &serie, F &&condition) {
        std::vector<T> result(serie.data());
        for (size_t i = 0; i < result.size(); ++i) {
            if (condition(serie[i], i)) {
                result[i] = std::numeric_limits<T>::quiet_NaN();
            }
        }
        return Serie<T>(result);
    }

    // Get indices of NaN values
    static std::vector<size_t> find_nan(const Serie<T> &serie) {
        std::vector<size_t> nan_indices;
        for (size_t i = 0; i < serie.size(); ++i) {
            if (std::isnan(serie[i])) {
                nan_indices.push_back(i);
            }
        }
        return nan_indices;
    }

    // Count NaN values
    static size_t count_nan(const Serie<T> &serie) {
        return std::count_if(serie.data().begin(), serie.data().end(),
                             [](const T &val) { return std::isnan(val); });
    }

    // Check if any value is NaN
    static bool has_nan(const Serie<T> &serie) {
        return std::any_of(serie.data().begin(), serie.data().end(),
                           [](const T &val) { return std::isnan(val); });
    }
};

// Helper functions
template <typename T>
inline Serie<T> set_at(const Serie<T> &serie,
                       const std::vector<size_t> &indices) {
    return NanOperations<T>::set_nan_at(serie, indices);
}

template <typename T, typename F>
inline Serie<T> set_where(const Serie<T> &serie, F &&condition) {
    return NanOperations<T>::set_nan_where(serie, std::forward<F>(condition));
}

template <typename T> inline std::vector<size_t> find(const Serie<T> &serie) {
    return NanOperations<T>::find_nan(serie);
}

template <typename T> inline size_t count(const Serie<T> &serie) {
    return NanOperations<T>::count_nan(serie);
}

template <typename T> inline bool has(const Serie<T> &serie) {
    return NanOperations<T>::has_nan(serie);
}

} // namespace nan
} // namespace df