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
#include <array>
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <stdexcept>

namespace df {

/**
 * Compute the inverse of a matrix stored as an array
 * Supports 1x1, 2x2, 3x3 matrices (both symmetric and non-symmetric storage)
 *
 * @param serie Input Serie containing matrices
 * @return Serie containing inverted matrices
 * @throws std::runtime_error if matrix is singular or dimension not supported
 */
template <typename T, size_t N>
Serie<std::array<T, N>> inv(const Serie<std::array<T, N>> &serie);

template <typename T, size_t N> auto bind_inv();
template <typename T, size_t N> auto bind_inv(const Serie<std::array<T, N>> &);

} // namespace df

#include "inline/inv.hxx"