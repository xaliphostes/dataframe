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

class Decomposer;
using Decomposers = std::vector<Decomposer *>;

/**
 * @brief Manager of decomposers
 * @ingroup Attributes
 */
class Manager {
  public:
    Manager(const Manager &);
    Manager(const Dataframe &, const Decomposers & = {}, uint = 3);
    Manager(const Dataframe &, const Strings & = {}, uint = 3);
    ~Manager();

    void add(Decomposer *);
    void add(const String &);

    void clear();

    uint nbDecomposers() const { return ds_.size(); }
    Serie serie(uint32_t itemSize, const String &name) const;
    Strings names(uint32_t itemSize) const;
    bool contains(uint32_t itemSize, const String &name) const;

  private:
    const Dataframe &df_;
    std::vector<std::unique_ptr<Decomposer>> ds_;
    uint8_t dim_{3};
    uint dimension_{3};
};

/**
 * @brief Useful function to create a manager given a list of series and
 * decomposers
 * @code
 * df::Serie s1(3, {1,2,3, 4,5,6});
 * df::Serie s2(1, {1, 3});
 *
 * Manager mng = df::attributes::createManager({s1, s2}, {"U", "a"}, {
 *      new df::attributes::Coordinates(), // x,y,z if applicable
 *      new df::attributes::Components()   // for vectors and matrices
 * }, 3);
 * @endcode
 *
 * The same code using directly a dataframe:
 * @code
 * df::Dataframe dataframe;
 * dataframe.add("U", df::Serie(3, {1,2,3, 4,5,6}));
 * dataframe.add("a", df::Serie(1, {1, 3}));
 *
 * Manager mng(dataframe, {
 *      new df::attributes::Coordinates(), // x,y,z if applicable
 *      new df::attributes::Components()   // for vectors and matrices
 * }, 3);
 * @endcode
 * @ingroup Attributes
 */
Manager createManager(const Series &, const Strings &,
                             const Decomposers & = {}, uint = 3);

} // namespace attributes
} // namespace df
