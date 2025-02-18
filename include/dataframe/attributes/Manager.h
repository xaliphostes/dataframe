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
#include <string>
#include <vector>

namespace df {
namespace attributes {

class Decomposer;
using Decomposers = std::vector<std::unique_ptr<Decomposer>>;

class Manager {
  public:
    Manager(const Dataframe &df);
    Manager(const Manager &other);

    void addDecomposer(std::unique_ptr<Decomposer> decomposer);

    std::vector<std::string> getNames(uint32_t itemSize) const;

    template <typename T> Serie<T> getSerie(const std::string &name) const;

    bool hasAttribute(uint32_t itemSize, const std::string &name) const;

    void clear();

    size_t decomposerCount() const;

  private:
    const Dataframe &dataframe_;
    Decomposers decomposers_;
};

// Helper function to create a Manager from series and names
inline Manager createManager(const std::vector<SerieBase> &series,
                             const std::vector<std::string> &names,
                             Decomposers decomposers = {});

} // namespace attributes
} // namespace df

#include "inline/Manager.hxx"