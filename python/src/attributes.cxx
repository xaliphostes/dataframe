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
#include <dataframe/attributes/Decomposer.h>
#include <dataframe/attributes/EigenValues.h>
#include <dataframe/attributes/EigenVectors.h>
#include <dataframe/attributes/Manager.h>
#include <dataframe/attributes/Normals.h>
#include <dataframe/attributes/UserDefinedDecomposer.h>
#include <dataframe/attributes/Valence.h>

#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

void init_attributes(pybind11::module &m) {

    pybind11::bind_vector<std::vector<std::string>>(m, "StringVector");

    auto attr = m.def_submodule("attributes");

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Decomposer>(attr, "Decomposer")
        .def("names", &df::attributes::Decomposer::names)
        .def("serie", &df::attributes::Decomposer::serie);

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Manager>(attr, "Manager")
        .def(pybind11::init<const df::Dataframe &,
                            const std::vector<std::string> &, uint>(),
             pybind11::keep_alive<1, 2>())

        .def("add",
             pybind11::overload_cast<df::attributes::Decomposer *>(
                 &df::attributes::Manager::add),
             "Add a Decomposer directly")
        .def("add",
             pybind11::overload_cast<const String &>(
                 &df::attributes::Manager::add),
             "Add a Decomposer given by its name")

        .def("serie", &df::attributes::Manager::serie)
        .def(
            "names",
            [](df::attributes::Manager &self, uint32_t itemSize) {
                try {
                    std::cout
                        << "Starting names() call with itemSize: " << itemSize
                        << std::endl;

                    auto names = self.names(itemSize);
                    std::cout << "Got names vector with size: " << names.size()
                              << std::endl;

                    // Check each string in the vector
                    for (size_t i = 0; i < names.size(); i++) {
                        try {
                            std::cout << "Checking name at index " << i << ": ";
                            if (names[i].empty()) {
                                std::cout << "(empty string)" << std::endl;
                            } else {
                                std::cout << "'" << names[i] << "'"
                                          << std::endl;
                            }
                        } catch (const std::exception &e) {
                            std::cout << "Error processing string at index "
                                      << i << ": " << e.what() << std::endl;
                        }
                    }

                    // Create a new vector with validated strings
                    std::vector<std::string> validated_names;
                    for (const auto &name : names) {
                        if (!name.empty()) {
                            validated_names.push_back(name);
                        }
                    }

                    return validated_names;
                } catch (const std::exception &e) {
                    std::cout << "Top level error in names(): " << e.what()
                              << std::endl;
                    throw;
                }
            },
            pybind11::return_value_policy::copy)
        .def("contains", &df::attributes::Manager::contains);

    attr.def("createManager", &df::attributes::createManager,
             "Create a Manager of attributes given a list of Series");

    // ---------------------------------------------------

    pybind11::class_<df::attributes::Area>(attr, "Area");
    pybind11::class_<df::attributes::Components>(attr, "Components");
    pybind11::class_<df::attributes::Coordinates>(attr, "Coordinates");
    pybind11::class_<df::attributes::EigenValues>(attr, "EigenValues");
    pybind11::class_<df::attributes::EigenVectors>(attr, "EigenVectors");
    pybind11::class_<df::attributes::Normals>(attr, "Normals");
    pybind11::class_<df::attributes::Valence>(attr, "Valence");

    // Bind UserDefinedDecomposer with a std::function that matches the callback
    // signature The callback takes a Dataframe and returns a Serie
    using Callback = std::function<df::Serie(const df::Dataframe &)>;
    using UDD = df::attributes::UserDefinedDecomposer<Callback>;

    pybind11::class_<UDD, df::attributes::Decomposer>(attr,
                                                      "UserDefinedDecomposer")
        .def(pybind11::init<uint, const String &, Callback>())
        .def("names", &UDD::names)
        .def("serie", &UDD::serie);

    // ---------------------------------------------------
}