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

#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

void init_dataframe(pybind11::module &m) {
    pybind11::class_<df::Dataframe>(m, "Dataframe")
        // Constructor
        .def(pybind11::init<uint32_t>(), pybind11::arg("count") = 0)

        // Methods
        .def("setCount", &df::Dataframe::setCount)
        .def("create", &df::Dataframe::create, pybind11::arg("name"),
             pybind11::arg("itemSize"), pybind11::arg("count") = 0)
        .def("add", &df::Dataframe::add)
        .def("set", &df::Dataframe::set)
        .def("del", &df::Dataframe::del)
        .def("clear", &df::Dataframe::clear)
        .def("contains", pybind11::overload_cast<const String &>(
                             &df::Dataframe::contains, pybind11::const_))
        .def("contains", pybind11::overload_cast<const df::Serie &>(
                             &df::Dataframe::contains, pybind11::const_))

        // Operators and getters
        .def(
            "__getitem__",
            [](df::Dataframe &df, const std::string &name) -> df::Serie & {
                return df[name];
            },
            pybind11::return_value_policy::reference)
        .def("get", pybind11::overload_cast<const std::string &>(&df::Dataframe::get))
        .def("series", &df::Dataframe::series)
        .def("dump", &df::Dataframe::dump);
}