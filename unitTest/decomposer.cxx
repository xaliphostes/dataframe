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

#include <iostream>
#include "../src/Serie.h"
#include "../src/Dataframe.h"
#include "../src/attributes/Manager.h"
#include "../src/attributes/ComponentsDecomposer.h"
#include "../src/types.h"
#include "assertions.h"

namespace df {

    class EigenValuesDecomposer: public df::Decomposer {
        Strings names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const override {
            if (name == "positions" || name == "indices") {
                return Strings();
            }
            if (serie.dimension() == 2 && (serie.itemSize() != 3 || itemSize != 1)) {
                return Strings();
            }
            if (serie.dimension() == 3 && (serie.itemSize() != 6 || itemSize != 1)) {
                return Strings();
            }

            if (serie.dimension() == 2) {
                return std::vector<String>{name + "1", name + "2"};
            }
            return std::vector<String>{name + "1", name + "2", name + "3"}; // dimension == 3
        }

        Serie serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const override {
            // To be done...
            return Serie();
        }
    };

    class EigenVectorsDecomposer: public df::Decomposer {
        Strings names(const df::Dataframe &dataframe, uint32_t itemSize, const df::Serie &serie, const String &name) const override {
            if (name == "positions" || name == "indices") {
                return Strings();
            }
            if (serie.dimension() == 2) {
                if (serie.itemSize() != 3 || itemSize != 2) {
                    return Strings();
                }
                return std::vector<String>{name + "1", name + "2"};
            } else {
                if (serie.itemSize() != 6 || itemSize != 3) {
                    return Strings();
                }
                return std::vector<String>{name + "1", name + "2", name + "3"};
            }
        }

        df::Serie serie(const df::Dataframe &dataframe, uint32_t itemSize, const String &name) const override {
            // To be done...
            return df::Serie();
        }
    };
}


void namesExist() {
    df::Dataframe dataframe;
    dataframe.add("a", df::Serie(1, {1, 2})); // scalar
    dataframe.add("U", df::Serie(3, {2, 4, 6, 3, 6, 9})); // vector 3
    dataframe.add("S", df::Serie(6, {2, 4, 6, 3, 6, 9, 1, 2, 3, 4, 5, 6})); // sym matrix 3x3
    dataframe.add("E", df::Serie(9, {2, 4, 6, 3, 6, 9, 1, 2, 3,  4, 5, 6, 7, 8, 9, 1, 2, 3})); // full matrix 3x3

    df::Manager mng(dataframe, {
        new df::ComponentsDecomposer(),
        new df::EigenValuesDecomposer()
    }, 3);

    // Gather scalar attributes
    // ------------------------
    Strings names = mng.names(1);

    std::cerr << names.size() << std::endl ;
    std::cerr << names << std::endl ;

    assertCondition(names.size() == 22, "names.size() != 22");
    assertCondition(mng.contains(1, "a"));
    assertCondition(mng.contains(1, "Exx"));
    assertCondition(mng.contains(1, "Exy"));
    assertCondition(mng.contains(1, "Exz"));
    assertCondition(mng.contains(1, "Eyx"));
    assertCondition(mng.contains(1, "Eyy"));
    assertCondition(mng.contains(1, "Eyz"));
    assertCondition(mng.contains(1, "Ezx"));
    assertCondition(mng.contains(1, "Ezy"));
    assertCondition(mng.contains(1, "Ezz"));
    assertCondition(mng.contains(1, "S1"));
    assertCondition(mng.contains(1, "S2"));
    assertCondition(mng.contains(1, "S3"));
    assertCondition(mng.contains(1, "Sxx"));
    assertCondition(mng.contains(1, "Sxy"));
    assertCondition(mng.contains(1, "Sxz"));
    assertCondition(mng.contains(1, "Syy"));
    assertCondition(mng.contains(1, "Syz"));
    assertCondition(mng.contains(1, "Szz"));
    assertCondition(mng.contains(1, "Ux"));
    assertCondition(mng.contains(1, "Uy"));
    assertCondition(mng.contains(1, "Uz"));

    mng.add(new df::EigenVectorsDecomposer());

    // Gather vector3 attributes
    // -------------------------
    names = mng.names(3);
    
    std::cerr << names.size() << std::endl ;
    std::cerr << names << std::endl ;

    assertCondition(names.size() == 4, "names.size() != 4");
    assertCondition(mng.contains(3, "U"));
    assertCondition(mng.contains(3, "S1"));
    assertCondition(mng.contains(3, "S2"));
    assertCondition(mng.contains(3, "S3"));
}

int main()
{
    namesExist();
    
    return 0;
}
