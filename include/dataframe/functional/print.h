/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include <dataframe/Serie.h>
#include <dataframe/functional/macros.h>

namespace df {

// Helper pour imprimer un vecteur
template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i < vec.size() - 1)
            os << ", ";
    }
    os << "]";
    return os;
}

template <typename T>
void print(const GenSerie<T> &serie, uint32_t precision = 4) {
    if (!serie.isValid()) {
        std::cerr << "Invalid Serie" << std::endl;
        return;
    }

    // Save current precision and set new one
    std::ios::fmtflags oldFlags = std::cout.flags();
    std::streamsize oldPrecision = std::cout.precision();
    std::cout.precision(precision);
    std::cout << std::fixed;

    // Print header info
    std::cout << "GenSerie<" << type_name<T>() << "> {\n";
    std::cout << "  itemSize : " << serie.itemSize() << "\n";
    std::cout << "  count    : " << serie.count() << "\n";
    std::cout << "  dimension: " << serie.dimension() << "\n";

    if (serie.count() > 0) {
        std::cout << "  values   : [\n";

        if (serie.itemSize() == 1) {
            // Pour les séries scalaires
            for (uint32_t i = 0; i < serie.count(); ++i) {
                std::cout << "    " << serie.value(i);
                if (i < serie.count() - 1)
                    std::cout << ",";
                std::cout << "\n";
            }
        } else {
            // Pour les séries vectorielles
            for (uint32_t i = 0; i < serie.count(); ++i) {
                std::cout << "    [";
                auto array = serie.array(i);
                for (uint32_t j = 0; j < array.size(); ++j) {
                    std::cout << array[j];
                    if (j < array.size() - 1)
                        std::cout << ", ";
                }
                std::cout << "]";
                if (i < serie.count() - 1)
                    std::cout << ",";
                std::cout << "\n";
            }
        }
        std::cout << "  ]\n";
    } else {
        std::cout << "  values   : []\n";
    }
    std::cout << "}" << std::endl;

    // Restore old flags and precision
    std::cout.flags(oldFlags);
    std::cout.precision(oldPrecision);
}

// Version pour imprimer plusieurs séries
template <typename T>
void print(const std::vector<GenSerie<T>> &series, uint32_t precision = 4) {
    std::cout << series.size() << " series:" << std::endl;
    for (const auto &serie : series) {
        print(serie, precision);
    }
}

MAKE_OP(print);

} // namespace df