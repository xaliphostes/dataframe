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
#include <dataframe/utils.h>

namespace df {

template <typename T>
void print(const Serie<T> &serie, size_t precision = 4) {
    if (serie.empty()) {
        std::cerr << "Invalid Serie" << std::endl;
        return;
    }

    // Save current precision and set new one
    std::ios::fmtflags oldFlags = std::cout.flags();
    std::streamsize oldPrecision = std::cout.precision();
    std::cout.precision(precision);
    std::cout << std::fixed;

    // std::cout << serie << std::endl;

    // Print header info
    std::cout << "Serie<" << type_name<T>() << "> {\n";
    std::cout << "  size: " << serie.size() << "\n";

    if (serie.size() > 0) {
        std::cout << "  values: [\n";
        for (size_t i = 0; i < serie.size(); ++i) {
            std::cout << "    " << serie[i];
            if (i < serie.size() - 1) {
                std::cout << ", \n";
            } else {
                std::cout << "\n  ]\n";
            }
        }
        std::cout << "}" << std::endl;

        // Restore old flags and precision
        std::cout.flags(oldFlags);
        std::cout.precision(oldPrecision);
    }
}

// Version pour imprimer plusieurs séries
template <typename T>
void print(const std::vector<Serie<T>> &series, size_t precision = 4) {
    std::cout << series.size() << " series:" << std::endl;
    for (const auto &serie : series) {
        print(serie, precision);
    }
}

MAKE_OP(print);

} // namespace df