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
 */

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/utils.h>
#include <type_traits>

namespace df {

/**
 * Compute the covariance between two Series
 * Cov(X,Y) = E[(X - μx)(Y - μy)]
 *
 * @param serie1 First Serie
 * @param serie2 Second Serie
 * @return Covariance value as the same type as the input Series
 * @throws std::runtime_error if Series are empty or have different sizes
 * 
 * @code
 * // Direct usage
 * auto cov = covariance(serie1, serie2);
 * auto sample_cov = sample_covariance(serie1, serie2);
 * 
 * // Pipeline usage
 * auto result = serie1 | bind_covariance(serie2);
 * auto sample_result = serie1 | bind_sample_covariance(serie2);
 * @endcode
 */
template <typename T>
T covariance(const Serie<T> &serie1, const Serie<T> &serie2);

/**
 * Compute the sample covariance between two Series
 * Uses (n-1) denominator instead of n
 */
template <typename T>
T sample_covariance(const Serie<T> &serie1, const Serie<T> &serie2);

// Enable pipe operation
template <typename T> auto bind_covariance(const Serie<T> &other);

template <typename T> auto bind_sample_covariance(const Serie<T> &other);

} // namespace df

#include "inline/covariance.hxx"