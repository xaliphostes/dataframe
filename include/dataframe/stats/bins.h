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

namespace df {

/**
 * Bin data from a Serie into specified number of bins with automatic range
 * detection
 * @param serie Input serie to bin
 * @param nb Number of bins
 * @return Serie containing the count in each bin
 */
template <typename T> Serie<size_t> bins(const Serie<T> &serie, uint nb);

/**
 * Bin data from a Serie into specified number of bins with given range
 * @param serie Input serie to bin
 * @param nb Number of bins
 * @param min Minimum value for binning range
 * @param max Maximum value for binning range
 * @return Serie containing the count in each bin
 */
template <typename T>
Serie<size_t> bins(const Serie<T> &serie, uint nb, T min, T max);

// Enable pipe operation
template <typename T> auto bind_bins(uint nb);

template <typename T> auto bind_bins(uint nb, T min, T max);

} // namespace df

#include "inline/bins.hxx"