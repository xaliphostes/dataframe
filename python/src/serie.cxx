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

void init_serie(pybind11::module &m) {
    pybind11::class_<df::Serie>(m, "Serie")
        .def(pybind11::init<>())
        .def(pybind11::init<int, uint32_t>())
        .def(pybind11::init<int, Array>())
        .def("count", &df::Serie::count)
        .def("itemSize", &df::Serie::itemSize)
        .def("dimension", &df::Serie::dimension)
        .def("size", &df::Serie::size)
        .def("asArray",
             [](df::Serie &self) {
                 const Array &arr = self.asArray();
                 // Assuming Array contains a contiguous buffer of doubles
                 // Adjust shape and dtype according to your Array class
                 // implementation
                 return pybind11::array_t<double>({arr.size()},     // shape
                                                  {sizeof(double)}, // strides
                                                  arr.data() // data pointer
                 );
             })
        .def("forEach",
             [](df::Serie &self, const pybind11::function &func) {
                 self.forEach([func](const Array &arr, uint32_t idx) {
                     func(arr, idx);
                 });
             })
        .def("forEachScalar",
             [](df::Serie &self, const pybind11::function &func) {
                 self.forEach([func](double v, uint32_t idx) { func(v, idx); });
             })
        // Bind get method that automatically handles both scalar and vector
        // returns
        // Bind get method with explicit return type as pybind11::object
        .def(
            "get",
            [](const df::Serie &self, uint32_t index) -> pybind11::object {
                if (self.itemSize() == 1) {
                    return pybind11::cast(self.get<double>(index));
                } else {
                    return pybind11::cast(self.get<Array>(index));
                }
            },
            "Get value at index (returns scalar for itemSize=1, Array "
            "otherwise)")

        // Optional: Add explicit scalar and vector versions if needed
        .def(
            "getScalar",
            [](const df::Serie &self, uint32_t index) {
                if (self.itemSize() != 1) {
                    throw std::runtime_error(
                        "Cannot use getScalar on non-scalar Serie");
                }
                return self.get<double>(index);
            },
            "Get scalar value at index")

        .def(
            "getArray",
            [](const df::Serie &self, uint32_t index) {
                if (self.itemSize() == 1) {
                    throw std::runtime_error(
                        "Cannot use getArray on scalar Serie");
                }
                return self.get<Array>(index);
            },
            "Get Array value at index")

        // Bind set method that handles both scalar and vector inputs
        .def(
            "set",
            [](df::Serie &self, uint32_t index, const pybind11::object &value) {
                if (self.itemSize() == 1) {
                    // For scalar Serie, accept number types
                    if (pybind11::isinstance<pybind11::float_>(value) ||
                        pybind11::isinstance<pybind11::int_>(value)) {
                        self.set(index, value.cast<double>());
                    } else {
                        throw std::runtime_error(
                            "Scalar Serie requires numeric value");
                    }
                } else {
                    // For vector Serie, accept sequence types
                    if (pybind11::isinstance<pybind11::sequence>(value)) {
                        auto seq = value.cast<std::vector<double>>();
                        if (seq.size() != self.itemSize()) {
                            throw std::runtime_error(
                                "Input array size must match Serie itemSize");
                        }
                        self.set(index, seq);
                    } else {
                        throw std::runtime_error(
                            "Vector Serie requires sequence value");
                    }
                }
            },
            "Set value at index (accepts scalar for itemSize=1, sequence for "
            "itemSize>1)");

    // OTHER IMPLEMENTATION OF FUNCTOR
    // -------------------------------
    // .def("forEach",
    //      [](df::Serie &self,
    //         std::function<void(const Array &, uint32_t)> callback) {
    //          self.forEach(callback);
    //      });
}