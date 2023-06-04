/*
 * Copyright (c) 2023 fmaerten@gmail.com
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

#include "../Dataframe.h"
#include "../Serie.h"
#include "../types.h"
#include "Decomposer.h"

namespace df
{

    /**
     * @brief Manager of decomposers
     */
    class Manager
    {
    public:
        /**
         * By default, no decomposer...
         */
        Manager(const Dataframe &dataframe, const Decomposers &decomposers = {});

        void add(const Decomposer &decomposer);
        void clear();

        Serie serie(uint32_t itemSize, const String &name) const;
        Strings names(uint32_t itemSize) const;
        bool contains(uint32_t itemSize, const String &name) const;

    private:
        const Dataframe &df_;
        std::vector<Decomposer> ds_;
        uint8_t dim_{3};
    };

}