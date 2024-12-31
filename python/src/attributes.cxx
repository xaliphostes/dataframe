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

#include <dataframe/attributes/Area.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/EigenValues.h>
#include <dataframe/attributes/EigenVectors.h>
#include <dataframe/attributes/Normals.h>
#include <dataframe/attributes/UserDefinedDecomposer.h>
#include <dataframe/attributes/Valence.h>
#include <dataframe/attributes/Decomposer.h>
#include <dataframe/attributes/Manager.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

void init_attributes(pybind11::module &m) {

    auto attr = m.def_submodule("attributes");

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Decomposer>(attr, "Decomposer")
        .def("names", &df::attributes::Decomposer::names)
        .def("serie", &df::attributes::Decomposer::serie);

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Manager>(attr, "Manager")
        .def(pybind11::init<df::Dataframe,
                            std::vector<df::attributes::Decomposer *>, uint>())
        .def("add", &df::attributes::Manager::add)
        .def("serie", &df::attributes::Manager::serie)
        .def("names", &df::attributes::Manager::names)
        .def("contains", &df::attributes::Manager::contains);

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Area>(attr, "Area");
    pybind11::class_<df::attributes::Components>(attr, "Components");
    pybind11::class_<df::attributes::Coordinates>(attr, "Coordinates")
        .def(pybind11::init<>());
    pybind11::class_<df::attributes::EigenValues>(attr, "EigenValues");
    pybind11::class_<df::attributes::EigenVectors>(attr, "EigenVectors");
    pybind11::class_<df::attributes::Normals>(attr, "Normals");
    pybind11::class_<df::attributes::Valence>(attr, "Valence");

    // pybind11::class_<df::attributes::UserDefinedDecomposer>(attr, "UserDefinedDecomposer")
    //     .def(pybind11::init<uint, String, const pybind11::function &func>())
    //     .def("names", &df::attributes::UserDefinedDecomposer::names)
    //     .def("serie", &df::attributes::UserDefinedDecomposer::serie);

    // ---------------------------------------------------
}