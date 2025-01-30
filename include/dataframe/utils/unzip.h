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
#include <dataframe/meta.h>
#include <tuple>
#include <type_traits>

namespace df {

namespace details {
// Helper to extract a component from a tuple Serie
template <std::size_t I, typename Tuple>
auto extract_component(const Serie<Tuple> &serie) {
    return serie.map([](const Tuple &t, size_t) { return std::get<I>(t); });
}

// Helper to generate all component series
template <typename Tuple, std::size_t... Is>
auto unzip_impl(const Serie<Tuple> &serie, std::index_sequence<Is...>) {
    return std::make_tuple(extract_component<Is>(serie)...);
}
} // namespace details

// Main unzip function
template <typename Tuple> auto unzip(const Serie<Tuple> &serie) {
    constexpr size_t tuple_size =
        std::tuple_size_v<typename Serie<Tuple>::value_type>;
    return details::unzip_impl(serie, std::make_index_sequence<tuple_size>{});
}

} // namespace df