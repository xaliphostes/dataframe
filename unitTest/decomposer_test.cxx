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

#include <iostream>
#include <dataframe/Serie.h>
#include <dataframe/Dataframe.h>
#include <dataframe/attributes/Manager.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/Normals.h>
#include <dataframe/attributes/Areas.h>
#include <dataframe/attributes/Valence.h>
#include <dataframe/attributes/UserDefinedDecomposer.h>
#include "TEST.h"

namespace df {

    class EigenValues: public df::Decomposer {
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

    class EigenVectors: public df::Decomposer {
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

TEST(decomposer, test) {
    df::Dataframe dataframe;
    dataframe.add("positions", df::Serie(3, {2,4,6, 3,6,9, 1,2,3})); // vector 3
    dataframe.add("indices", df::Serie(3, {0,1,2})); // vector 3
    dataframe.add("S", df::Serie(6, {2,4,6,3,6,9, 1,2,3,4,5,6, 9,8,7,6,5,4})); // sym matrix 3x3

    df::Manager mng(dataframe, {
        new df::Coordinates(), // x,y,z if applicable
        new df::Components(),  // vectors and matrices are decomposed
        new df::EigenValues(), // eigen values if applicable
        new df::EigenVectors(),// eigen vectors if applicable
        new df::Normals(),     // normal vectors if applicable
        new df::Area()         // polygon areas if applicable
    }, 3); // 3 stands for 3-dimensional

    std::cerr << "Available series of scalars :\n" << mng.names(1) << std::endl;
    std::cerr << "Available series of vector3 :\n" << mng.names(3) << std::endl;
    std::cerr << "Available series of matrix33:\n" << mng.names(6) << std::endl;
}

TEST(decomposer, basic) {
    df::Dataframe dataframe;
    dataframe.add("a", df::Serie(1, {1, 2})); // scalar
    dataframe.add("positions", df::Serie(3, {2,4,6, 3,6,9, 1,2,3})); // vector 3
    dataframe.add("indices", df::Serie(3, {0,1,2})); // vector 3
    dataframe.add("S", df::Serie(6, {2,4,6,3,6,9, 1,2,3,4,5,6, 9,8,7,6,5,4})); // sym matrix 3x3

    df::Manager mng(dataframe, {
        new df::Components(),
        new df::EigenValues(),
        new df::EigenVectors(),
        new df::Normals(),
        new df::Coordinates(),
        new df::Area(),
        new df::Valence()
    }, 3);

    {
        Strings names = mng.names(1);
        std::cerr << "Scalars:\n" << names ;
    }
    {
        Strings names = mng.names(3);
        std::cerr << "Vector3:\n" << names ;
    }
    {
        Strings names = mng.names(6);
        std::cerr << "SMatrix33:\n" << names ;
    }
}

TEST(decomposer, name_exist) {
    df::Dataframe dataframe;
    dataframe.add("a", df::Serie(1, {1, 2})); // scalar
    dataframe.add("U", df::Serie(3, {2, 4, 6, 3, 6, 9})); // vector 3
    dataframe.add("S", df::Serie(6, {2, 4, 6, 3, 6, 9, 1, 2, 3, 4, 5, 6})); // sym matrix 3x3
    dataframe.add("E", df::Serie(9, {2, 4, 6, 3, 6, 9, 1, 2, 3,  4, 5, 6, 7, 8, 9, 1, 2, 3})); // full matrix 3x3

    df::Manager mng(dataframe, {
        new df::Components(),
        new df::EigenValues()
    }, 3);

    // Gather scalar attributes
    // ------------------------
    Strings names = mng.names(1);

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

    mng.add(new df::EigenVectors());

    // Gather vector3 attributes
    // -------------------------
    names = mng.names(3);

    assertCondition(names.size() == 4, "names.size() != 4");
    assertCondition(mng.contains(3, "U"));
    assertCondition(mng.contains(3, "S1"));
    assertCondition(mng.contains(3, "S2"));
    assertCondition(mng.contains(3, "S3"));
}

TEST(decomposer, coordinates) {
    df::Dataframe dataframe;
    dataframe.add("positions", df::Serie(3, {0,0,0, 1,0,0, 1,1,0}));
    dataframe.add("indices", df::Serie(3, {0,1,2}));

    df::Manager mng(dataframe, {
        new df::Coordinates(),
        new df::Normals()
    }, 3);
    
    assertCondition(mng.nbDecomposers() == 2, "nb decomposers != 2");

    Strings names = mng.names(1);
    assertCondition(names.size() == 3, "names(1).size() != 3");

    df::Serie x = mng.serie(1, "x");
    assertCondition(x.isValid() == true, "x not valid");
    assertArrayEqual(x.asArray(), Array{0, 1, 1});
    
    df::Serie y = mng.serie(1, "y");
    assertCondition(y.isValid() == true, "y not valid");
    assertArrayEqual(y.asArray(), Array{0, 0, 1});

    df::Serie z = mng.serie(1, "z");
    assertCondition(z.isValid() == true, "z not valid");
    assertArrayEqual(z.asArray(), Array{0, 0, 0});
}

TEST(decomposer, normals) {
    df::Dataframe dataframe;
    dataframe.add("positions", df::Serie(3, {0,0,0, 1,0,0, 1,1,0}));
    dataframe.add("indices", df::Serie(3, {0,1,2}));

    df::Manager mng(dataframe, {
        new df::Normals()
    }, 3);
    
    assertCondition(mng.nbDecomposers() == 1, "nb decomposers != 1");

    Strings names = mng.names(3);
    assertCondition(names.size() == 1, "names(3).size() != 1");

    df::Serie normals = mng.serie(3, "normals");
    assertCondition(normals.isValid() == true, "normals not valid");
    assertArrayEqual(normals.asArray(), Array{0, 0, 1});
}

TEST(decomposer, area) {
    df::Dataframe dataframe;
    dataframe.add("positions", df::Serie(3, {0,0,0, 1,0,0, 1,1,0}));
    dataframe.add("indices", df::Serie(3, {0,1,2}));

    df::Manager mng(dataframe, {
        new df::Area()
    }, 1);
    
    assertCondition(mng.nbDecomposers() == 1, "nb decomposers != 1");

    Strings names = mng.names(1);
    assertCondition(names.size() == 1, "names(1).size() != 1");

    df::Serie normals = mng.serie(3, "area");
    assertCondition(normals.isValid() == true, "area not valid");
    assertEqual(normals.asArray()[0], 0.5);
}

TEST(decomposer, userDefined) {
    df::Dataframe dataframe;
    dataframe.add("S", df::Serie(6, {2,4,6,3,6,9, 1,2,3,4,5,6, 9,8,7,6,5,4})); // sym matrix 3x3

    df::Manager mng(dataframe, {
        new df::UserDefinedDecomposer(1, "xx", [](const df::Dataframe& dataframe) {
            return df::Components().serie(dataframe, 1, "Sxx");
            // return dataframe["S"];
        })
    }, 3);

    auto s = mng.serie(1, "xx");

    // std::cerr << s << std::endl;
    // std::cerr << "Available series of scalars :\n" << mng.names(1) << std::endl;
    // std::cerr << "Available series of vector3 :\n" << mng.names(3) << std::endl;
    // std::cerr << "Available series of matrix33:\n" << mng.names(6) << std::endl;

    assertArrayEqual(s.asArray(), Array{2, 1, 9});
    
}

RUN_TESTS()
