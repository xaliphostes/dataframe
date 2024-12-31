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
#include "Decomposer.h"

namespace df {
namespace attributes {

/**
 * @brief User defined decomposer using a lambda function
 * @ingroup Attributes
 */
template <typename F> class UserDefinedDecomposer : public Decomposer {
    using UDD = UserDefinedDecomposer<F>;

  public:
    UserDefinedDecomposer(uint, const String &, F);

    std::unique_ptr<Decomposer> clone() const override {
        return std::make_unique<UDD>(itemSize_, name_, cb_);
    }

    Strings names(const Dataframe &, uint32_t, const Serie &,
                  const String &) const override;

    Serie serie(const Dataframe &, uint32_t, const String &) const override;

  private:
    uint itemSize_{0};
    String name_;
    F cb_;
};

// ------------------------------------------------------------

template <typename F>
inline UserDefinedDecomposer<F>::UserDefinedDecomposer(uint itemSize,
                                                       const String &name, F cb)
    : itemSize_(itemSize), name_(name), cb_(std::move(cb)) {}

template <typename F>
inline Strings
UserDefinedDecomposer<F>::names(const Dataframe &dataframe, uint32_t itemSize,
                                const Serie &serie, const String &name) const {
    if (itemSize != itemSize_) {
        return Strings();
    }

    Serie s = cb_(dataframe);
    if (s.isValid() == false) {
        return Strings();
    }

    return Strings{name_};
}

template <typename F>
inline Serie UserDefinedDecomposer<F>::serie(const Dataframe &dataframe,
                                             uint32_t itemSize,
                                             const String &name) const {
    if (itemSize != itemSize_ || name_ != name) {
        return Serie();
    }

    return cb_(dataframe);
}

} // namespace attributes
} // namespace df
