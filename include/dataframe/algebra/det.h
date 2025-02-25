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
#include <dataframe/Serie.h>
#include <dataframe/utils/utils.h>
#include <type_traits>

namespace df {

/**
 * Compute the determinant of a Serie containing arrays.
 * Works with:
 * - 2D vectors (array<T, 2>)
 * - 2D symmetric matrices (array<T, 3>)
 * - 3D vectors (array<T, 3>)
 * - 3D symmetric matrices (array<T, 6>)
 *
 * @param serie Input Serie containing arrays
 * @return Serie<T> Serie containing determinant values
 * @throws std::runtime_error if array size is not compatible
 */
template <typename T, std::size_t N>
Serie<T> det(const Serie<std::array<T, N>> &serie);

/**
 * @brief Enable pipe operation
 */
template <typename T, std::size_t N> auto bind_det();

} // namespace df

#include "inline/det.hxx"