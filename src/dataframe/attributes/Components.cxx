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

#include <dataframe/attributes/Components.h>
#include <dataframe/utils/nameOfSerie.h>
#include <dataframe/functional/apply.h>
#include <dataframe/utils/utils.h>
#include <algorithm>

namespace df
{

    static Strings vector2Names{"x", "y"};
    static Strings smatrix2Names{"xx", "xy", "yy"};
    static Strings matrix2Names{"xx", "xy", "yx", "yy"};

    static Strings vector3Names{"x", "y", "z"};
    static Strings smatrix3Names{"xx", "xy", "xz", "yy", "yz", "zz"};
    static Strings matrix3Names{"xx", "xy", "xz", "yx", "yy", "yz", "zx", "zy", "zz"};

    Strings Components::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const
    {
        if (name == "positions" || name == "indices")
        {
            return Strings();
        }

        // Passed name is, e.g., 'U' && itemSize=3 && dimension=3
        String sname = nameOfSerie(dataframe, serie);
        if (name == sname && serie.itemSize() == 1)
        {
            return Strings();
        }

        if (itemSize > 1)
        {
            return Strings();
        }

        if (serie.dimension() == 2)
        {
            switch (serie.itemSize())
            {
            case 2:
                return map(vector2Names, [name](const String& n) { return name + n; });
            case 3:
                return map(smatrix2Names, [name](const String& n) { return name + n; });
            case 4:
                return map(matrix2Names, [name](const String& n) { return name + n; });
            }
        }
        else
        {
            Strings result;

            switch (serie.itemSize())
            {
            case 3:
                return map(vector3Names, [name](const String& n) { return name + n; });
            case 6:
                return map(smatrix3Names, [name](const String& n) { return name + n; });
            case 9:
                return map(matrix3Names, [name](const String& n) { return name + n; });
            }
        }

        Strings names;
        for (uint32_t i = 0; i < itemSize; ++i)
        {
            names.push_back(name + std::to_string(i));
        }

        return names;
    }

    Serie Components::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const
    {
        if (itemSize > 1)
        {
            return Serie();
        }

        // vector2 / vector3
        auto newName = name.substr(0, name.size() - 1);
        auto serie = dataframe[newName];

        if (serie.isValid())
        {
            if (serie.dimension() == 2)
            {
                for (uint32_t i = 0; i < vector2Names.size(); ++i)
                {
                    if (name == newName + vector2Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
            }
            else
            {
                for (uint32_t i = 0; i < vector3Names.size(); ++i)
                {
                    if (name == newName + vector3Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
            }
        }

        // (smatrix2 and matrix2) / (smatrix3 and matrix3)
        newName = name.substr(0, name.size() - 2);
        serie = dataframe[newName];
        if (serie.isValid())
        {
            if (serie.dimension() == 2)
            {
                for (uint32_t i = 0; i < smatrix2Names.size(); ++i)
                {
                    if (name == newName + smatrix2Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
                for (uint32_t i = 0; i < matrix2Names.size(); ++i)
                {
                    if (name == newName + matrix2Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
            }
            else
            {
                for (uint32_t i = 0; i < smatrix3Names.size(); ++i)
                {
                    if (name == newName + smatrix3Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
                for (uint32_t i = 0; i < matrix3Names.size(); ++i)
                {
                    if (name == newName + matrix3Names[i])
                    {
                        return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                    }
                }
            }
        }

        // Others: use integer
        newName = name.substr(0, name.size() - 1);
        serie = dataframe[newName];
        if (serie.isValid())
        {
            for (uint32_t i = 0; i < itemSize; ++i)
            {
                if (name == newName + std::to_string(i))
                {
                    return serie.map( [i](const Array& item, uint32_t index) {return Array{item[i]};} );
                }
            }
        }

        return Serie();
    }

}