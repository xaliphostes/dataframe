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

#include <dataframe/attributes/EigenVectors.h>
#include <dataframe/utils/utils.h>
#include <dataframe/types.h>
#include <dataframe/math/eigen.h>
#include <algorithm>

namespace df
{

    Strings EigenVectors::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const
    {
        if (name == "positions" || name == "indices") {
            return Strings();
        }
        if (serie.dimension() == 2 && (serie.itemSize() != 3 || itemSize != 2)) {
            return Strings();
        }
        if (serie.dimension() == 3 && (serie.itemSize() != 6 || itemSize != 3)) {
            return Strings();
        }

        if (serie.dimension() == 2) {
            return Strings{name + "1", name + "2"};
        }
        return Strings{name + "1", name + "2", name + "3"};
    }

    Serie EigenVectors::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const
    {
        if (itemSize != 3) {
            return Serie();
        }
        String newName = name.substr(0, name.size() - 1);
        const Serie& serie = dataframe[newName];
        uint id = name[name.size() - 1];

        if (serie.isValid() == false) {
            return Serie();
        }

        if (serie.dimension() == 2) {
            if (serie.itemSize() != 3) {
                return Serie();
            }
            if (id < 1 || id > 2) {
                return Serie();
            }
        } else {
            if (serie.itemSize() != 6) {
                return Serie();
            }
            if (id < 1 || id > 3) {
                return Serie();
            }
        }

        return eigenVectors(serie).map( [id](const Array& item, uint32_t) {
            return Array{item[id-1]};
        });
    }

}
