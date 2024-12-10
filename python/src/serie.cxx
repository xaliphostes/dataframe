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
             });
    // OTHER IMPLEMENTATION OF FUNCTOR
    // -------------------------------
    // .def("forEach",
    //      [](df::Serie &self,
    //         std::function<void(const Array &, uint32_t)> callback) {
    //          self.forEach(callback);
    //      });
}