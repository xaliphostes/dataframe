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
#include "Serie.h"
#include <map>
#include <string>

/**
 * @brief Create a Dataframe containing several Series with same count.
 * @example
 * ```c++
 * Dataframe d ;
 * d.add("positions", Serie(3, 20)) ;
 * d.add("indices", Serie(2, 20)) ;
 * std::cout << d["positions"].array() << std::endl ;
 * d["positions"].dump() ;
 * ```
 */

namespace df
{

    class Dataframe
    {
    public:
        Dataframe(uint32_t count = 0);
        void setCount(uint32_t count);

        void create(const std::string &name, uint32_t itemSize, uint32_t count = 0); // convenience
        void add(const std::string &name, const Serie &serie);
        void set(const std::string &name, const Serie &serie);
        void del(const std::string &name);
        void clear() ;

        bool contains(const Serie &) const;
        bool contains(const String &) const;

        Serie &operator[](const std::string &name);
        const Serie &operator[](const std::string &name) const;
        
        const std::map<std::string, Serie> &series() const { return series_; }

        void dump() const;

    private:
        uint32_t count_{0};
        std::map<std::string, Serie> series_;
    };

}
