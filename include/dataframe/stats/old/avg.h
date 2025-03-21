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
#include <dataframe/utils/meta.h>
#include <dataframe/utils/utils.h>
#include <type_traits>

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

template <typename T> auto bind_avg();

} // namespace stats
} // namespace df

#include "inline/avg.hxx"