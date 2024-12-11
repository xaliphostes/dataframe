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
#include <dataframe/functional/utils/concat.h>
#include <dataframe/functional/utils/parallel_execute.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

void init_utils(pybind11::module &m) {
    auto utils = m.def_submodule("utils");

    // Bind concat to handle n Series
    utils.def(
        "concat",
        [](pybind11::list series_list) {
            std::vector<df::Serie> series;

            // Convert pybind11::list to vector of Series
            for (auto item : series_list) {
                series.push_back(item.cast<df::Serie>());
            }

            return df::utils::concat(series);
        },
        "Concatenate n Series");

    utils.def(
        "parallel_execute",
        [](const pybind11::function &callback, const df::Serie &serie,
           uint32_t nbCores) {
            // Create a wrapper functor that converts Python callback to C++
            // functor
            auto wrapper = [callback](const df::Serie &s) -> df::Serie {
                pybind11::gil_scoped_acquire
                    gil; // Acquire GIL for Python callback
                try {
                    pybind11::object result = callback(s);
                    return result.cast<df::Serie>();
                } catch (const pybind11::error_already_set &e) {
                    throw std::runtime_error(
                        std::string("Python error in callback: ") + e.what());
                }
            };

            // Call the C++ parallel_execute with our wrapper
            return df::utils::parallel_execute(wrapper, serie, nbCores);
        },
        "Execute a function in parallel across multiple cores",
        pybind11::arg("callback"), pybind11::arg("serie"),
        pybind11::arg("nb_cores"));
}