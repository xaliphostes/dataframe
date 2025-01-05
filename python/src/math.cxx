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

#include <dataframe/functional/math/add.h>
#include <dataframe/functional/math/div.h>
#include <dataframe/functional/math/equals.h>
#include <dataframe/functional/math/minMax.h>
#include <dataframe/functional/math/mult.h>
#include <dataframe/functional/math/negate.h>
#include <dataframe/functional/math/normalize.h>
#include <dataframe/functional/math/random.h>
#include <dataframe/functional/math/scale.h>
#include <dataframe/functional/math/sub.h>
#include <dataframe/functional/math/weightedSum.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

void bind_ops(py::module_ &m) {
    // Bind the variadic template version for multiple series
    m.def(
        "add",
        [](const py::args &args) {
            if (args.size() < 1) {
                throw std::invalid_argument("At least one Serie is required");
            }
            df::Serie result = args[0].cast<df::Serie>();
            for (size_t i = 1; i < args.size(); ++i) {
                result = df::math::add(result, args[i].cast<df::Serie>());
            }

            return result;
        },
        "Add multiple series together");

    // Bind the variadic template version for multiple series
    m.def(
        "sub",
        [](const py::args &args) {
            if (args.size() < 1) {
                throw std::invalid_argument("At least one Serie is required");
            }
            df::Serie result = args[0].cast<df::Serie>();
            for (size_t i = 1; i < args.size(); ++i) {
                result = df::math::sub(result, args[i].cast<df::Serie>());
            }

            return result;
        },
        "Subtract multiple series together");

    // Bind the variadic template version for multiple series
    m.def(
        "mult",
        [](const py::args &args) {
            if (args.size() != 2) {
                throw std::invalid_argument("Expected two Series");
            }
            return df::math::mult(args[0].cast<df::Serie>(), args[1].cast<df::Serie>());
        },
        "Multiply two series");

    // Bind the variadic template version for multiple series
    m.def(
        "div",
        [](const py::args &args) {
            if (args.size() != 2) {
                throw std::invalid_argument("Expected two Series");
            }
            return df::math::div(args[0].cast<df::Serie>(), args[1].cast<df::Serie>());
        },
        "Divide two series");
}

void init_math(pybind11::module &m) {
    auto mm = m.def_submodule("math");
    mm.doc() =
        "Mathematical operations for Series"; // optional module docstring

    bind_ops(mm);
}

/*
void init_math(pybind11::module &m) {
    auto mm = m.def_submodule("math");
    mm.def("add", &df::math::add);
    mm.def("div", &df::math::div);
    mm.def("equals", &df::math::equals);
    mm.def("minMax", &df::math::minMax);
    mm.xdef("mult", &df::math::mult);
    mm.def("negate", &df::math::negate);
    mm.def("normalize", &df::math::normalize);
    mm.def("random", &df::math::random);
    mm.def("scale", &df::math::scale);
    mm.def("sub", &df::math::sub);
    mm.def("weightedSum", &df::math::weightedSum);
}
*/
