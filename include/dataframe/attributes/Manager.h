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

namespace df {
namespace attributes {

// -----------------------------------------------------------

// Mathematical decomposition dimension
enum class DecompDimension {
    Scalar = 1, // Individual components (x, y, z, etc.)
    Vector,     // N-dimensional vectors
    Matrix      // N-dimensional matrices/tensors
};

// -----------------------------------------------------------

/**
 * @brief Base class for attribute decomposers
 */
class Decomposer {
  public:
    virtual ~Decomposer() = default;

    // Create a clone of this decomposer
    virtual std::unique_ptr<Decomposer> clone() const = 0;

    // Get the names of all decomposed attributes for a given serie
    virtual Strings names(const Dataframe &dataframe, DecompDimension targetDim,
                          const SerieBase &serie, const String &name) const = 0;

    // Get a specific decomposed serie
    virtual Serie<double> serie(const Dataframe &dataframe,
                                DecompDimension targetDim,
                                const std::string &name) const = 0;

  protected:
    template <typename T> static size_t getComponentCount();

    template <typename T>
    static Serie<double> extractComponent(const Serie<T> &serie, size_t index);
};

// -----------------------------------------------------------

template <typename C> class GenDecomposer : public Decomposer {
  public:
    std::unique_ptr<Decomposer> clone() const override;

    Strings names(const Dataframe &dataframe, DecompDimension targetDim,
                  const SerieBase &serie, const String &name) const override;

    Serie<double> serie(const Dataframe &dataframe, DecompDimension targetDim,
                        const std::string &name) const override;
};

// -----------------------------------------------------------

class Manager {
  public:
    explicit Manager(const Dataframe &df);

    Manager(const Manager &other);

    void addDecomposer(const Decomposer &decomposer);

    std::vector<std::string> getNames(DecompDimension targetDim) const;

    template <typename T> Serie<T> getSerie(const std::string &name) const;

    bool hasAttribute(DecompDimension, const std::string &) const;

    void clear();

    size_t decomposerCount() const;

  private:
    const Dataframe &dataframe_;
    std::vector<std::unique_ptr<Decomposer>> decomposers_;
};

// -----------------------------------------------------------

// Helper function to create a Manager
template <typename... Ts>
Manager createManager(const std::vector<std::string> &names,
                      const Serie<Ts> &...series);

// -----------------------------------------------------------

} // namespace attributes
} // namespace df

#include "inline/Manager.hxx"