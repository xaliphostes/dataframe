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

#include <dataframe/Serie.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

void init_map(py::module &m) {
    // Single Serie map
    m.def(
        "map",
        [](const pybind11::function &callback, const df::Serie &serie) {
            if (serie.itemSize() == 1) {
                // Handle scalar Serie -> could return scalar or vector
                return df::map(
                    [callback](double value, uint32_t index) {
                        pybind11::object result = callback(value, index);
                        if (pybind11::isinstance<pybind11::float_>(result) ||
                            pybind11::isinstance<pybind11::int_>(result)) {
                            return result.cast<double>();
                        } else {
                            // Assume it's a sequence that can be converted to
                            // Array
                            return result.cast<Array>();
                        }
                    },
                    serie);
            } else {
                // Handle vector Serie -> could return scalar or vector
                return df::map(
                    [callback](const Array &value, uint32_t index) {
                        pybind11::object result = callback(value, index);
                        if (pybind11::isinstance<pybind11::float_>(result) ||
                            pybind11::isinstance<pybind11::int_>(result)) {
                            return result.cast<double>();
                        } else {
                            return result.cast<Array>();
                        }
                    },
                    serie);
            }
        },
        "Map a function over a Serie");

    // Multi-series map
    m.def(
        "map",
        [](const pybind11::function &callback,
           const std::vector<df::Serie> &series) {
            if (series.empty()) {
                throw std::invalid_argument("At least one Serie is required");
            }

            size_t count = series[0].count();
            for (const auto &serie : series) {
                if (serie.count() != count) {
                    throw std::invalid_argument(
                        "All series must have the same count");
                }
            }

            // Get first result to determine output type
            std::vector<pybind11::object> first_args;
            for (const auto &serie : series) {
                if (serie.itemSize() == 1) {
                    first_args.push_back(pybind11::cast(serie.get<double>(0)));
                } else {
                    first_args.push_back(pybind11::cast(serie.get<Array>(0)));
                }
            }
            first_args.push_back(pybind11::cast(0u)); // index

            pybind11::object first_result = callback.call(first_args);
            bool returns_scalar =
                pybind11::isinstance<pybind11::float_>(first_result) ||
                pybind11::isinstance<pybind11::int_>(first_result);

            if (returns_scalar) {
                // Callback returns scalar
                Array results;
                results.reserve(count);
                results.push_back(first_result.cast<double>());

                for (uint32_t i = 1; i < count; ++i) {
                    std::vector<pybind11::object> args;
                    for (const auto &serie : series) {
                        if (serie.itemSize() == 1) {
                            args.push_back(
                                pybind11::cast(serie.get<double>(i)));
                        } else {
                            args.push_back(pybind11::cast(serie.get<Array>(i)));
                        }
                    }
                    args.push_back(pybind11::cast(i));
                    results.push_back(callback(args).cast<double>());
                }
                return df::Serie(1, results);
            } else {
                // Callback returns Array
                Array first_array = first_result.cast<Array>();
                const size_t itemSize = first_array.size();

                Array flattened;
                flattened.reserve(itemSize * count);
                flattened.insert(flattened.end(), first_array.begin(),
                                 first_array.end());

                for (uint32_t i = 1; i < count; ++i) {
                    std::vector<pybind11::object> args;
                    for (const auto &serie : series) {
                        if (serie.itemSize() == 1) {
                            args.push_back(
                                pybind11::cast(serie.get<double>(i)));
                        } else {
                            args.push_back(pybind11::cast(serie.get<Array>(i)));
                        }
                    }
                    args.push_back(pybind11::cast(i));

                    Array result_i = callback(args).cast<Array>();
                    if (result_i.size() != itemSize) {
                        throw std::runtime_error(
                            "Inconsistent output vector sizes in map");
                    }
                    flattened.insert(flattened.end(), result_i.begin(),
                                     result_i.end());
                }
                return df::Serie(itemSize, flattened);
            }
        },
        "Map a function over multiple Series");
}