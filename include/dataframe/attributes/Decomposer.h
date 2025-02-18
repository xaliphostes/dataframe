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

#pragma once
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <dataframe/types.h>
#include <dataframe/meta.h>
#include <memory>
#include <string>
#include <vector>

namespace df {
namespace attributes {

/**
 * @brief Base class for attribute decomposers
 */
class Decomposer {
public:
    virtual ~Decomposer() = default;
    
    // Create a clone of this decomposer
    virtual std::unique_ptr<Decomposer> clone() const = 0;
    
    // Get the names of all decomposed attributes for a given serie
    virtual std::vector<std::string> names(
        const Dataframe& dataframe,
        uint32_t itemSize,
        const SerieBase& serie,
        const std::string& name) const = 0;
    
    // Get a specific decomposed serie
    virtual Serie<double> serie(
        const Dataframe& dataframe,
        uint32_t itemSize,
        const std::string& name) const = 0;

protected:
    // Helper to extract component at index from array-like types
    template<typename T>
    Serie<double> extractComponent(const Serie<T>& serie, size_t index) const {
        return serie.map([index](const T& value, size_t) -> double {
            if constexpr (std::is_array_v<T> || details::is_std_array_v<T>) {
                return static_cast<double>(value[index]);
            } else {
                return static_cast<double>(value);
            }
        });
    }
};

} // namespace attributes
} // namespace df