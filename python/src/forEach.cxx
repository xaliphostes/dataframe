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

void init_forEach(pybind11::module &m) {
    // Bind the standalone forEach function
    m.def(
        "forEach",
        [](const pybind11::function &callback, const df::Serie &serie) {
            // Handle scalar Serie
            if (serie.itemSize() == 1) {
                df::forEach(
                    [callback](double value, uint32_t index) {
                        callback(value, index);
                    },
                    serie);
            }
            // Handle non-scalar Serie
            else {
                df::forEach(
                    [callback](const Array &value, uint32_t index) {
                        callback(value, index);
                    },
                    serie);
            }
        },
        "Execute callback for each element in the Serie");

    // Multi-series forEach using variadic template
    m.def("forEach", [](const pybind11::function &callback,
                        const std::vector<df::Serie> &series) {
        if (series.empty()) {
            throw std::invalid_argument("At least one Serie is required");
        }

        // Check all series have same count
        size_t count = series[0].count();
        for (const auto &serie : series) {
            if (serie.count() != count) {
                throw std::invalid_argument(
                    "All series must have the same count");
            }
        }

        // Process each item
        for (uint32_t i = 0; i < count; ++i) {
            // Build arguments list
            pybind11::list args;

            // Add all series values
            for (const auto &serie : series) {
                if (serie.itemSize() == 1) {
                    args.append(serie.get<double>(i));
                } else {
                    args.append(serie.get<Array>(i));
                }
            }

            // Add index
            args.append(i);

            // Call the function
            callback(*pybind11::reinterpret_steal<pybind11::tuple>(args));
        }
    });
}