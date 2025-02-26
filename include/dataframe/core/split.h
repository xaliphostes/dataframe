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

namespace df {

// Split a single series into n approximately equal parts
template <typename T>
std::vector<Serie<T>> split(size_t n, const Serie<T> &serie);

// Split multiple series into n approximately equal parts
template <typename T, typename... Ts>
auto split(size_t n, const Serie<T> &first, const Serie<Ts> &...rest);

// Helper function to create a bound split operation
template <typename T> auto bind_split(size_t n);

} // namespace df

#include "inline/split.hxx"