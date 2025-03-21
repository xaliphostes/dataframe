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
#include <map>

namespace df {
namespace stats {

/**
 * Compute arithmetic average of a Serie
 * Works with scalar types and array types (vectors, matrices)
 *
 * @param serie Input Serie
 * @return Average value (same type as input elements)
 * @throws std::runtime_error if serie is empty
 */
template <typename T> T avg(const Serie<T> &serie);

/**
 * Return a Serie containing the average value(s)
 * Useful for pipeline operations
 */
template <typename T> Serie<T> avg_serie(const Serie<T> &serie);

/**
 * @brief Calculate the mean (average) of a Serie
 *
 * This function computes the arithmetic mean of all elements in the Serie.
 * It works with both scalar types and array types (vectors, matrices).
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @return Mean value (same type as input elements)
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Calculate mean of a numeric Serie
 * df::Serie<double> values{1.5, 2.5, 3.5, 4.5, 5.5};
 * double mean_val = df::mean(values);  // 3.5
 *
 * // For vector Serie, mean is calculated component-wise
 * df::Serie<Vector2> points{{1.0, 2.0}, {3.0, 4.0}, {5.0, 6.0}};
 * Vector2 mean_point = df::mean(points);  // {3.0, 4.0}
 * @endcode
 */
template <typename T> T mean(const Serie<T> &serie);

/**
 * @brief Calculate the variance of a Serie
 *
 * This function computes the variance of all elements in the Serie.
 * If the Serie contains only a single element, the variance is defined as 0.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @param population Whether to calculate population variance (true) or sample
 * variance (false)
 * @return Variance value as double for scalar types, or as array of doubles for
 * array types
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Calculate variance of a numeric Serie
 * df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
 * double var_pop = df::variance(values, true);   // Population variance
 * double var_sample = df::variance(values);      // Sample variance (default)
 * @endcode
 */
template <typename T>
auto variance(const Serie<T> &serie, bool population = false);

/**
 * @brief Calculate the standard deviation of a Serie
 *
 * This function computes the standard deviation of all elements in the Serie,
 * which is the square root of the variance.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @param population Whether to calculate population std dev (true) or sample
 * std dev (false)
 * @return Standard deviation value as double for scalar types, or as array of
 * doubles for array types
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Calculate standard deviation of a numeric Serie
 * df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
 * double std_pop = df::std_dev(values, true);   // Population standard
 * deviation double std_sample = df::std_dev(values);      // Sample standard
 * deviation (default)
 * @endcode
 */
template <typename T>
auto std_dev(const Serie<T> &serie, bool population = false);

/**
 * @brief Calculate the median of a Serie
 *
 * This function computes the median value of the Serie elements. The median is
 * the middle value when elements are sorted. For even-sized Series, the median
 * is the average of the two middle values.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @return Median value
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Calculate median of a numeric Serie
 * df::Serie<int> values1{5, 1, 8, 4, 3};
 * int median1 = df::median(values1);  // 4
 *
 * df::Serie<double> values2{2.5, 1.0, 8.5, 3.5, 4.5, 6.0};
 * double median2 = df::median(values2);  // 4.0 (average of 3.5 and 4.5)
 * @endcode
 *
 * @note For array types (vectors, matrices), component-wise median is
 * calculated
 */
template <typename T> auto median(Serie<T> serie);

/**
 * @brief Calculate a quantile of a Serie
 *
 * This function computes the specified quantile of the Serie elements.
 * A quantile is a value below which a certain proportion of observations fall.
 * For example, the 0.5 quantile is the median.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @param q Quantile to calculate (between 0 and 1)
 * @return Quantile value
 * @throws std::runtime_error if the Serie is empty or if q is outside [0,1]
 *
 * Example:
 * @code
 * // Calculate quartiles of a numeric Serie
 * df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
 * double q1 = df::quantile(values, 0.25);  // First quartile
 * double q2 = df::quantile(values, 0.5);   // Second quartile (median)
 * double q3 = df::quantile(values, 0.75);  // Third quartile
 * @endcode
 *
 * @note Linear interpolation is used for quantiles that fall between data
 * points
 * @note For array types (vectors, matrices), component-wise quantile is
 * calculated
 */
template <typename T> auto quantile(Serie<T> serie, double q);

/**
 * Calculate Interquartile Range (IQR)
 */
template <typename T> T iqr(const Serie<T> &serie);

/**
 * Return a Serie of boolean values indicating outliers
 * Using 1.5 * IQR criterion
 */
template <typename T> Serie<bool> isOutlier(const Serie<T> &serie);

/**
 * Return a Serie of boolean values indicating non-outliers
 */
template <typename T> Serie<bool> notOutlier(const Serie<T> &serie);

/**
 * @brief Calculate the mode of a Serie
 *
 * This function computes the mode, which is the most frequently occurring value
 * in the Serie. If multiple values occur with the same highest frequency, the
 * lowest value is returned.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @return Mode value
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Calculate mode of a numeric Serie
 * df::Serie<int> values{1, 2, 2, 3, 3, 3, 4, 4, 5};
 * int mode_val = df::mode(values);  // 3 (appears three times)
 * @endcode
 *
 * @note For array types, this function is not defined as the concept of mode
 *       for vector types is ambiguous.
 */
template <typename T> T mode(const Serie<T> &serie);

/**
 * @brief Calculate a vector of summary statistics for a Serie
 *
 * This function computes various summary statistics including count, min, max,
 * mean, median, standard deviation, and quartiles.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @return A map of statistic names to values
 * @throws std::runtime_error if the Serie is empty
 *
 * Example:
 * @code
 * // Get summary statistics for a numeric Serie
 * df::Serie<double> values{1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};
 * auto stats = df::summary(values);
 * // stats contains keys: count, min, q1, median, q3, max, mean, std_dev
 * @endcode
 */
template <typename T> auto summary(const Serie<T> &serie);

/**
 * @brief Calculate z-scores (standard scores) for a Serie
 *
 * This function computes the z-score for each element in the Serie.
 * A z-score measures how many standard deviations an element is from the mean.
 *
 * @tparam T Type of elements in the Serie
 * @param serie Input Serie
 * @param population Whether to use population (true) or sample (false) standard
 * deviation
 * @return Serie<double> of z-scores
 * @throws std::runtime_error if the Serie is empty or std_dev is zero
 *
 * Example:
 * @code
 * // Calculate z-scores for a numeric Serie
 * df::Serie<double> values{2.0, 4.0, 4.0, 4.0, 5.0, 5.0, 7.0, 9.0};
 * auto z_scores = df::z_score(values);
 * @endcode
 */
template <typename T>
Serie<double> z_score(const Serie<T> &serie, bool population = false);

/**
 * @brief Calculate the covariance between two Series
 * Cov(X,Y) = E[(X - μx)(Y - μy)]
 *
 * This function computes the covariance between two Series of the same length.
 * Covariance measures how much two variables change together.
 *
 * @tparam T Type of elements in the first Serie
 * @tparam U Type of elements in the second Serie
 * @param serie1 First input Serie
 * @param serie2 Second input Serie
 * @param population Whether to calculate population (true) or sample (false)
 * covariance
 * @return Covariance value
 * @throws std::runtime_error if either Serie is empty or they have different
 * lengths
 *
 * Example:
 * @code
 * // Calculate covariance between two Series
 * df::Serie<double> x{1.0, 2.0, 3.0, 4.0, 5.0};
 * df::Serie<double> y{5.0, 4.0, 3.0, 2.0, 1.0};
 * double cov = df::covariance(x, y);  // -2.5
 * @endcode
 */
template <typename T, typename U>
double covariance(const Serie<T> &serie1, const Serie<U> &serie2,
                  bool population = false);

/**
 * @brief Calculate the correlation coefficient between two Series
 *
 * This function computes the Pearson correlation coefficient between two
 * Series. Correlation measures the strength and direction of the linear
 * relationship between two variables.
 *
 * @tparam T Type of elements in the first Serie
 * @tparam U Type of elements in the second Serie
 * @param serie1 First input Serie
 * @param serie2 Second input Serie
 * @return Correlation coefficient (between -1 and 1)
 * @throws std::runtime_error if either Serie is empty, they have different
 * lengths, or if either Serie has zero standard deviation
 *
 * Example:
 * @code
 * // Calculate correlation between two Series
 * df::Serie<double> x{1.0, 2.0, 3.0, 4.0, 5.0};
 * df::Serie<double> y{5.0, 4.0, 3.0, 2.0, 1.0};
 * double corr = df::correlation(x, y);  // -1.0 (perfect negative correlation)
 * @endcode
 */
template <typename T, typename U>
double correlation(const Serie<T> &serie1, const Serie<U> &serie2);

// Helper functions for pipeline operations

template <typename T> auto bind_avg();
template <typename T> auto bind_mean();
template <typename T> auto bind_variance(bool population = false);
template <typename T> auto bind_std_dev(bool population = false);
template <typename T> auto bind_median();
template <typename T> auto bind_quantile(double q);
template <typename T> auto bind_iqr();
template <typename T> auto bind_isOutlier();
template <typename T> auto bind_notOutlier();
template <typename T> auto bind_mode();
template <typename T> auto bind_z_score(bool population = false);

} // namespace stats
} // namespace df

#include "inline/stats.hxx"