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
#include <memory>

namespace df {
namespace attributes {

/**
 * @brief Interface for a decomposer
 * @ingroup Attributes
 */
class Decomposer {
  public:
    virtual ~Decomposer();

    virtual std::unique_ptr<Decomposer> clone() const = 0;

    virtual Strings names(const Dataframe &dataframe, uint32_t itemSize,
                          const Serie &serie, const String &name) const = 0;

    virtual Serie serie(const Dataframe &dataframe, uint32_t itemSize,
                        const String &name) const = 0;
};

template <typename T>
class GenDecomposer : public Decomposer {
  public:
    std::unique_ptr<Decomposer> clone() const override{
      return std::make_unique<T>();
    }
};


} // namespace attributes
} // namespace df
