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

#include <dataframe/attributes/Valence.h>
#include <dataframe/utils/utils.h>
#include <dataframe/types.h>
#include <algorithm>

namespace df
{

    Valence::Valence(const String &name): name_(name)
    {
    }

    Strings Valence::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const
    {
        if (itemSize != 1) {
            return Strings();
        }
        if (!dataframe.contains("positions") && !dataframe.contains("indices")) {
            return Strings();
        }

        return Strings{name_};
    }

    Serie Valence::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const
    {

        if (name != name_) {
            return Serie();
        }

        const Serie& positions = dataframe["positions"];
        const Serie& indices = dataframe["indices"];
        if (!positions.isValid() || !indices.isValid()) {
            return Serie();
        }

        Array ids = createArray(positions.count(), 0);

        indices.forEach( [&](const Array& t, uint32_t) {
            ids[t[0]]++;
            ids[t[1]]++;
            ids[t[2]]++;
        });

        return Serie(1, ids);
    }

}