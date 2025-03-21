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
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/math/bounds.h>
#include <dataframe/utils/meta.h>
#include <dataframe/utils/utils.h>
#include <stdexcept>
#include <type_traits>

namespace df {

namespace stats {

namespace detail {

// // Helper to determine if we're dealing with an array type
// template <typename T> struct is_array : std::false_type {};

// template <typename T, std::size_t N>
// struct is_array<std::array<T, N>> : std::true_type {};

// Mean implementation for scalar types
template <typename T>
inline T compute_avg_impl(
    const Serie<T> &serie,
    typename std::enable_if<!details::is_std_array<T>::value>::type * =
        nullptr) {

    if (serie.empty()) {
        throw std::runtime_error("Cannot compute avg of empty serie");
    }

    T sum = T{};
    for (size_t i = 0; i < serie.size(); ++i) {
        sum += serie[i];
    }
    return sum / static_cast<T>(serie.size());
}

// Mean implementation for array types (vectors, matrices)
template <typename T>
inline T compute_avg_impl(
    const Serie<T> &serie,
    typename std::enable_if<details::is_std_array<T>::value>::type * =
        nullptr) {

    if (serie.empty()) {
        throw std::runtime_error("Cannot compute avg of empty serie");
    }

    T sum{}; // Zero-initialize array
    for (size_t i = 0; i < serie.size(); ++i) {
        for (size_t j = 0; j < details::array_dimension<T>::value; ++j) {
            sum[j] += serie[i][j];
        }
    }

    // Component-wise division
    for (size_t j = 0; j < details::array_dimension<T>::value; ++j) {
        sum[j] /= static_cast<typename T::value_type>(serie.size());
    }

    return sum;
}

} // namespace detail

template <typename T> inline T avg(const Serie<T> &serie) {
    return detail::compute_avg_impl(serie);
}

template <typename T> inline Serie<T> avg_serie(const Serie<T> &serie) {
    return Serie<T>({avg(serie)});
}

template <typename T> inline auto bind_avg() {
    return [](const Serie<T> &serie) { return avg(serie); };
}

template <typename T> inline T mean(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate mean of an empty Serie");
    }

    // Use existing avg function if available
    return avg(serie);
}

template <typename T>
inline auto variance(const Serie<T> &serie, bool population) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate variance of an empty Serie");
    }

    if (serie.size() == 1) {
        // Variance of a single element is defined as 0
        if constexpr (std::is_arithmetic_v<T>) {
            return 0.0;
        } else if constexpr (details::is_std_array_v<T>) {
            // For array types, return array of zeros
            using ElementType = typename T::value_type;
            using ResultType = std::array<double, std::tuple_size_v<T>>;
            ResultType result;
            result.fill(0.0);
            return result;
        } else {
            throw std::runtime_error(
                "Unsupported type for variance calculation");
        }
    }

    // Calculate mean
    T mean_value = mean(serie);

    // Calculate sum of squared differences
    if constexpr (std::is_arithmetic_v<T>) {
        // For scalar types
        double sum_sq_diff = serie.reduce(
            [&mean_value](double acc, const T &value, size_t) {
                double diff = static_cast<double>(value) -
                              static_cast<double>(mean_value);
                return acc + diff * diff;
            },
            0.0);

        // Divide by n for population variance or (n-1) for sample variance
        double divisor = population ? serie.size() : (serie.size() - 1);
        return sum_sq_diff / divisor;
    } else if constexpr (details::is_std_array_v<T>) {
        // For array types (Vector2, Vector3, etc.)
        constexpr size_t N = std::tuple_size_v<T>;
        using ElementType = typename T::value_type;
        using ResultType = std::array<double, N>;

        ResultType sum_sq_diff;
        sum_sq_diff.fill(0.0);

        serie.forEach([&sum_sq_diff, &mean_value](const T &value, size_t) {
            for (size_t i = 0; i < N; ++i) {
                double diff = static_cast<double>(value[i]) -
                              static_cast<double>(mean_value[i]);
                sum_sq_diff[i] += diff * diff;
            }
        });

        // Divide by n for population variance or (n-1) for sample variance
        double divisor = population ? serie.size() : (serie.size() - 1);
        ResultType result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = sum_sq_diff[i] / divisor;
        }

        return result;
    } else {
        throw std::runtime_error("Unsupported type for variance calculation");
    }
}

template <typename T>
inline auto std_dev(const Serie<T> &serie, bool population) {
    if (serie.empty()) {
        throw std::runtime_error(
            "Cannot calculate standard deviation of an empty Serie");
    }

    // Calculate variance
    auto var = variance(serie, population);

    if constexpr (std::is_arithmetic_v<T>) {
        // For scalar types, return square root of variance
        return std::sqrt(var);
    } else if constexpr (details::is_std_array_v<T>) {
        // For array types, return element-wise square root
        constexpr size_t N = std::tuple_size_v<T>;
        using ResultType = std::array<double, N>;

        ResultType result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = std::sqrt(var[i]);
        }

        return result;
    } else {
        throw std::runtime_error(
            "Unsupported type for standard deviation calculation");
    }
}

template <typename T> inline auto median(Serie<T> serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate median of an empty Serie");
    }

    if constexpr (std::is_arithmetic_v<T>) {
        // For scalar types, sort and find middle element(s)
        auto data = serie.asArray();
        std::sort(data.begin(), data.end());

        size_t size = data.size();
        if (size % 2 == 1) {
            // Odd number of elements, return the middle one
            return static_cast<double>(data[size / 2]);
        } else {
            // Even number of elements, return average of the two middle ones
            return (static_cast<double>(data[size / 2 - 1]) +
                    static_cast<double>(data[size / 2])) /
                   2.0;
        }
    } else if constexpr (details::is_std_array_v<T>) {
        // For array types, calculate component-wise median
        constexpr size_t N = std::tuple_size_v<T>;
        using ElementType = typename T::value_type;
        using ResultType = std::array<double, N>;

        ResultType result;

        for (size_t i = 0; i < N; ++i) {
            // Extract i-th component from each element
            std::vector<ElementType> component_values;
            component_values.reserve(serie.size());

            serie.forEach([&component_values, i](const T &value, size_t) {
                component_values.push_back(value[i]);
            });

            // Sort the component values
            std::sort(component_values.begin(), component_values.end());

            // Calculate median of this component
            size_t size = component_values.size();
            if (size % 2 == 1) {
                // Odd number of elements
                result[i] = static_cast<double>(component_values[size / 2]);
            } else {
                // Even number of elements
                result[i] =
                    (static_cast<double>(component_values[size / 2 - 1]) +
                     static_cast<double>(component_values[size / 2])) /
                    2.0;
            }
        }

        return result;
    } else {
        throw std::runtime_error("Unsupported type for median calculation");
    }
}

template <typename T> inline auto quantile(Serie<T> serie, double q) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate quantile of an empty Serie");
    }

    if (q < 0.0 || q > 1.0) {
        throw std::runtime_error("Quantile value must be between 0 and 1");
    }

    if constexpr (std::is_arithmetic_v<T>) {
        // For scalar types
        auto data = serie.asArray();
        std::sort(data.begin(), data.end());

        size_t size = data.size();

        if (size == 1) {
            return static_cast<double>(data[0]);
        }

        double pos = q * (size - 1);
        size_t idx_lower = static_cast<size_t>(pos);
        size_t idx_upper = std::min(idx_lower + 1, size - 1);
        double weight = pos - idx_lower;

        // Linear interpolation between the two nearest values
        return static_cast<double>(data[idx_lower]) * (1.0 - weight) +
               static_cast<double>(data[idx_upper]) * weight;
    } else if constexpr (details::is_std_array_v<T>) {
        // For array types, calculate component-wise quantile
        constexpr size_t N = std::tuple_size_v<T>;
        using ElementType = typename T::value_type;
        using ResultType = std::array<double, N>;

        ResultType result;

        for (size_t i = 0; i < N; ++i) {
            // Extract i-th component from each element
            std::vector<ElementType> component_values;
            component_values.reserve(serie.size());

            serie.forEach([&component_values, i](const T &value, size_t) {
                component_values.push_back(value[i]);
            });

            // Sort the component values
            std::sort(component_values.begin(), component_values.end());

            size_t size = component_values.size();

            if (size == 1) {
                result[i] = static_cast<double>(component_values[0]);
                continue;
            }

            double pos = q * (size - 1);
            size_t idx_lower = static_cast<size_t>(pos);
            size_t idx_upper = std::min(idx_lower + 1, size - 1);
            double weight = pos - idx_lower;

            // Linear interpolation between the two nearest values
            result[i] =
                static_cast<double>(component_values[idx_lower]) *
                    (1.0 - weight) +
                static_cast<double>(component_values[idx_upper]) * weight;
        }

        return result;
    } else {
        throw std::runtime_error("Unsupported type for quantile calculation");
    }
}

template <typename T> inline T iqr(const Serie<T> &serie) {
    return quantile(serie, 0.75) - quantile(serie, 0.25);
}

template <typename T> inline Serie<bool> isOutlier(const Serie<T> &serie) {
    const T q1 = quantile(serie, 0.25);
    const T q3 = quantile(serie, 0.75);
    const T iqr_value = q3 - q1;
    const T lower_bound = q1 - T{1.5} * iqr_value;
    const T upper_bound = q3 + T{1.5} * iqr_value;

    return serie.map([lower_bound, upper_bound](const T &value, size_t) {
        return value < lower_bound || value > upper_bound;
    });
}

template <typename T> inline Serie<bool> notOutlier(const Serie<T> &serie) {
    return isOutlier(serie).map(
        [](const bool &value, size_t) { return !value; });
}

template <typename T> inline T mode(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate mode of an empty Serie");
    }

    if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, std::string>) {
        // Count occurrences of each value
        std::map<T, size_t> counts;
        serie.forEach([&counts](const T &value, size_t) { counts[value]++; });

        // Find value with highest frequency
        T mode_value = serie[0];
        size_t max_count = 0;

        for (const auto &[value, count] : counts) {
            if (count > max_count) {
                mode_value = value;
                max_count = count;
            }
        }

        return mode_value;
    } else {
        throw std::runtime_error("Mode is not defined for this type");
    }
}

template <typename T> inline auto summary(const Serie<T> &serie) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate summary of an empty Serie");
    }

    if constexpr (std::is_arithmetic_v<T>) {
        std::map<std::string, double> summary_stats;

        // Count
        summary_stats["count"] = static_cast<double>(serie.size());

        // Min and Max
        auto [min_val, max_val] = bounds(serie);
        summary_stats["min"] = static_cast<double>(min_val);
        summary_stats["max"] = static_cast<double>(max_val);

        // Quartiles
        summary_stats["q1"] = quantile(serie, 0.25);
        summary_stats["median"] = median(serie);
        summary_stats["q3"] = quantile(serie, 0.75);

        // Mean and StdDev
        summary_stats["mean"] = static_cast<double>(mean(serie));
        summary_stats["std_dev"] = std_dev(serie);

        return summary_stats;
    } else {
        throw std::runtime_error(
            "Summary statistics are only defined for scalar types");
    }
}

template <typename T>
inline Serie<double> z_score(const Serie<T> &serie, bool population) {
    if (serie.empty()) {
        throw std::runtime_error("Cannot calculate z-scores of an empty Serie");
    }

    if constexpr (std::is_arithmetic_v<T>) {
        // Calculate mean and standard deviation
        double mean_val = static_cast<double>(mean(serie));
        double sd = std_dev(serie, population);

        if (sd == 0) {
            throw std::runtime_error(
                "Standard deviation is zero, z-scores are undefined");
        }

        // Compute z-scores
        return serie.map([mean_val, sd](const T &value, size_t) {
            return (static_cast<double>(value) - mean_val) / sd;
        });
    } else {
        throw std::runtime_error("Z-scores are only defined for scalar types");
    }
}

template <typename T, typename U>
inline double covariance(const Serie<T> &serie1, const Serie<U> &serie2,
                         bool population) {
    if (serie1.empty() || serie2.empty()) {
        throw std::runtime_error("Cannot calculate covariance of empty Series");
    }

    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same length for covariance calculation");
    }

    if constexpr (std::is_arithmetic_v<T> && std::is_arithmetic_v<U>) {
        // Calculate means
        double mean1 = static_cast<double>(mean(serie1));
        double mean2 = static_cast<double>(mean(serie2));

        // Calculate sum of products of deviations
        double sum_product = 0.0;
        for (size_t i = 0; i < serie1.size(); ++i) {
            double dev1 = static_cast<double>(serie1[i]) - mean1;
            double dev2 = static_cast<double>(serie2[i]) - mean2;
            sum_product += dev1 * dev2;
        }

        // Divide by n for population covariance or (n-1) for sample covariance
        double divisor = population ? serie1.size() : (serie1.size() - 1);
        return sum_product / divisor;
    } else {
        throw std::runtime_error("Covariance is only defined for scalar types");
    }
}

template <typename T, typename U>
inline double correlation(const Serie<T> &serie1, const Serie<U> &serie2) {
    if (serie1.empty() || serie2.empty()) {
        throw std::runtime_error(
            "Cannot calculate correlation of empty Series");
    }

    if (serie1.size() != serie2.size()) {
        throw std::runtime_error(
            "Series must have the same length for correlation calculation");
    }

    if constexpr (std::is_arithmetic_v<T> && std::is_arithmetic_v<U>) {
        // Calculate standard deviations and covariance
        double sd1 = std_dev(serie1);
        double sd2 = std_dev(serie2);
        double cov = covariance(serie1, serie2);

        if (sd1 == 0 || sd2 == 0) {
            throw std::runtime_error(
                "Standard deviation is zero, correlation is undefined");
        }

        return cov / (sd1 * sd2);
    } else {
        throw std::runtime_error(
            "Correlation is only defined for scalar types");
    }
}

// ----------------------------------------------------------------------------
// Helper function to create a bound version of mean for pipeline operations
// ----------------------------------------------------------------------------

template <typename T> inline auto bind_mean() {
    return [](const Serie<T> &serie) { return Serie<T>{mean(serie)}; };
}

// Helper function to create a bound version of variance for pipeline operations
template <typename T> inline auto bind_variance(bool population) {
    return [population](const Serie<T> &serie) {
        auto var = variance(serie, population);
        if constexpr (std::is_arithmetic_v<T>) {
            return Serie<double>{var};
        } else {
            return Serie<std::decay_t<decltype(var)>>{var};
        }
    };
}

// Helper function to create a bound version of std_dev for pipeline operations
template <typename T> inline auto bind_std_dev(bool population) {
    return [population](const Serie<T> &serie) {
        auto sd = std_dev(serie, population);
        if constexpr (std::is_arithmetic_v<T>) {
            return Serie<double>{sd};
        } else {
            return Serie<std::decay_t<decltype(sd)>>{sd};
        }
    };
}

// Helper function to create a bound version of median for pipeline operations
template <typename T> inline auto bind_median() {
    return [](const Serie<T> &serie) {
        auto med = median(serie);
        if constexpr (std::is_arithmetic_v<T>) {
            return Serie<double>{med};
        } else {
            return Serie<std::decay_t<decltype(med)>>{med};
        }
    };
}

// Helper function to create a bound version of quantile for pipeline operations
template <typename T> inline auto bind_quantile(double q) {
    return [q](const Serie<T> &serie) {
        auto quant = quantile(serie, q);
        if constexpr (std::is_arithmetic_v<T>) {
            return Serie<double>{quant};
        } else {
            return Serie<std::decay_t<decltype(quant)>>{quant};
        }
    };
}

template <typename T> inline auto bind_iqr() {
    return [](const Serie<T> &serie) { return iqr(serie); };
}

template <typename T> inline auto bind_isOutlier() {
    return [](const Serie<T> &serie) { return isOutlier(serie); };
}

template <typename T> inline auto bind_notOutlier() {
    return [](const Serie<T> &serie) { return notOutlier(serie); };
}

// Helper function to create a bound version of mode for pipeline operations
template <typename T> inline auto bind_mode() {
    return [](const Serie<T> &serie) { return Serie<T>{mode(serie)}; };
}

// Helper function to create a bound version of z_score for pipeline operations
template <typename T> inline auto bind_z_score(bool population) {
    return [population](const Serie<T> &serie) {
        return z_score(serie, population);
    };
}

} // namespace stats
} // namespace df