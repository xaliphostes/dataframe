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

namespace df {

template <typename T>
auto weightedSum(const std::vector<Serie<T>> &series,
                 const ArrayType<T> &weights)
    -> details::isArithmeticSerie<T, T> {

    if (series.size() != weights.size()) {
        throw std::runtime_error(
            "Number of series must match number of weights");
    }

    if (series.empty()) {
        return Serie<T>();
    }

    // Check that all series have the same size
    const size_t expectedSize = series[0].size();
    for (const auto &s : series) {
        if (s.size() != expectedSize) {
            throw std::runtime_error("All series must have the same size");
        }
    }

    // Initialize result with first weighted series
    Serie<T> result = series[0].map(
        [w = weights[0]](const T &value, size_t) { return value * w; });

    // Add remaining weighted series
    for (size_t i = 1; i < series.size(); ++i) {
        auto weighted = series[i].map(
            [w = weights[i]](const T &value, size_t) { return value * w; });
        result = add(result, weighted);
    }

    return result;
}

// Overload for initializer_list input
template <typename T>
auto weightedSum(const std::initializer_list<Serie<T>> &series,
                 const ArrayType<T> &weights)
    -> details::isArithmeticSerie<T, T> {
    return weightedSum(std::vector<Serie<T>>(series.begin(), series.end()),
                       weights);
}

// WeightedSum with series weights
template <typename T>
auto weightedSum(const std::vector<Serie<T>> &series,
                 const std::vector<Serie<T>> &weights)
    -> details::isArithmeticSerie<T, T> {

    if (series.size() != weights.size()) {
        throw std::runtime_error(
            "Number of series must match number of weight series");
    }

    if (series.empty()) {
        return Serie<T>();
    }

    // Check that all series and weights have the same size
    const size_t expectedSize = series[0].size();
    for (const auto &s : series) {
        if (s.size() != expectedSize) {
            throw std::runtime_error("All series must have the same size");
        }
    }
    for (const auto &w : weights) {
        if (w.size() != expectedSize) {
            throw std::runtime_error(
                "All weight series must have the same size as the data series");
        }
    }

    // Initialize result with first weighted series
    Serie<T> result = mult(series[0], weights[0]);

    // Add remaining weighted series
    for (size_t i = 1; i < series.size(); ++i) {
        result = add(result, mult(series[i], weights[i]));
    }

    return result;
}

// Overload for initializer_list input with series weights
template <typename T>
auto weightedSum(const std::initializer_list<Serie<T>> &series,
                 const std::initializer_list<Serie<T>> &weights)
    -> details::isArithmeticSerie<T, T> {
    return weightedSum(std::vector<Serie<T>>(series.begin(), series.end()),
                       std::vector<Serie<T>>(weights.begin(), weights.end()));
}

// Bind functions for pipeline operations
template <typename T> struct weighted_sum_binder_scalar {
    std::vector<Serie<T>> series;
    ArrayType<T> weights;

    weighted_sum_binder_scalar(const std::vector<Serie<T>> &s,
                               const ArrayType<T> &w)
        : series(s), weights(w) {}

    Serie<T> operator()(const Serie<T> &first) const {
        std::vector<Serie<T>> all_series{first};
        all_series.insert(all_series.end(), series.begin(), series.end());
        return weightedSum(all_series, weights);
    }
};

template <typename T> struct weighted_sum_binder_series {
    std::vector<Serie<T>> series;
    std::vector<Serie<T>> weights;

    weighted_sum_binder_series(const std::vector<Serie<T>> &s,
                               const std::vector<Serie<T>> &w)
        : series(s), weights(w) {}

    Serie<T> operator()(const Serie<T> &first) const {
        std::vector<Serie<T>> all_series{first};
        all_series.insert(all_series.end(), series.begin(), series.end());
        return weightedSum(all_series, weights);
    }
};

// Helper functions to create binders
template <typename T>
auto bind_weightedSum(const InitializerSeries<T> &series,
                      const ArrayType<T> &weights)
    -> InitializerBinderScalar<T> {
    return weighted_sum_binder_scalar<T>(
        std::vector<Serie<T>>(series.begin(), series.end()), weights);
}

template <typename T>
auto bind_weightedSum(const InitializerSeries<T> &series,
                      const InitializerSeries<T> &weights)
    -> InitializerBinderSeries<T> {
    return weighted_sum_binder_series<T>(
        std::vector<Serie<T>>(series.begin(), series.end()),
        std::vector<Serie<T>>(weights.begin(), weights.end()));
}

} // namespace df