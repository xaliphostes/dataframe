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
#include <algorithm>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <type_traits>
#include <vector>

namespace df {

// /**
//  * Calculate quantile value for a Serie
//  * @param serie Input Serie
//  * @param p Quantile value (0 to 1)
//  * @return Quantile value
//  */
// template <typename T> T quantile(const Serie<T> &serie, double p);

/**
 * Calculate first quartile (Q1)
 */
template <typename T> T q25(const Serie<T> &serie);

/**
 * Calculate median (Q2)
 */
template <typename T> T q50(const Serie<T> &serie);

/**
 * Calculate third quartile (Q3)
 */
template <typename T> T q75(const Serie<T> &serie);

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

// Enable pipe operations
// template <typename T> auto bind_quantile(double p);
template <typename T> auto bind_q25();
template <typename T> auto bind_q50();
template <typename T> auto bind_q75();
template <typename T> auto bind_iqr();
template <typename T> auto bind_isOutlier();
template <typename T> auto bind_notOutlier();

} // namespace df

#include "inline/quantile.hxx"
