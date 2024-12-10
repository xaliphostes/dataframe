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

#include <dataframe/attributes/Normals.h>
// #include <dataframe/utils/utils.h>
// #include <dataframe/types.h>
#include <dataframe/functional/geo/normals.h>
#include <algorithm>

namespace df
{

    Normals::Normals(const String &name): name_(name)
    {
    }

    Strings Normals::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const
    {
        if (itemSize != 3) {
            return Strings();
        }
        if (!dataframe.contains("positions") && !dataframe.contains("indices")) {
            return Strings();
        }

        return Strings{name_};
    }

    Serie Normals::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const
    {

        if (name != name_) {
            return Serie();
        }

        return geo::normals(dataframe["positions"], dataframe["indices"]);
    }

}