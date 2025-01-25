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

#include "TEST.h"
#include <dataframe/Dataframe.h>

struct MyType {
    MyType() : a(0), b(0.0) {}
    MyType(int a, double b) : a(a), b(b) {}
    int a;
    double b;
};
std::ostream &operator<<(std::ostream &o, const MyType &s) {
    o << "{" << s.a << ", " << s.b << "}";
    return o;
}

TEST(dataframe, test1) {

    df::Dataframe df;

    // Ajout de séries de différents types
    df.add("a", df::GenSerie<double>(1, {1.0, 2.0, 3.0}));
    df.add("b", df::GenSerie<int>(3, {1, 2, 3, 4, 5, 6}));
    df.add("c", df::GenSerie<float>(1, {1.0f, 2.0f, 3.0f}));
    df.add("d", df::GenSerie<MyType>(1, {MyType(1, 2.0), MyType(3, 4.0)}));

    // Accès aux séries avec le bon type
    {
        auto &a = df.get<double>("a");

        auto expected = ParsedSerie<double>{.type = "double",
                                            .itemSize = 1,
                                            .count = 3,
                                            .dimension = 3,
                                            .values{1.0, 2.0, 3.0}};

        EXPECT_SERIE_EQ(a, expected);
    }

    {
        auto &b = df.get<int>("b");
        auto expected = ParsedSerie<int>{.type = "int",
                                         .itemSize = 3,
                                         .count = 2,
                                         .dimension = 3,
                                         .values{1, 2, 3, 4, 5, 6}};
        EXPECT_SERIE_EQ(b, expected);
    }

    {
        auto &c = df.get<float>("c");
        auto expected = ParsedSerie<float>{.type = "float",
                                           .itemSize = 1,
                                           .count = 3,
                                           .dimension = 3,
                                           .values{1, 2, 3}};
        EXPECT_SERIE_EQ(c, expected);
    }

    {
        auto &d = df.get<MyType>("d");
        auto expected =
            ParsedSerie<MyType>{.type = "int",
                                .itemSize = 1,
                                .count = 2,
                                .dimension = 3,
                                .values = {MyType(1, 2.0), MyType(3, 4.0)}};
        // EXPECT_SERIE_EQ(d, expected);
    }
}

TEST(Dataframe, CopyConstructor) {
    MSG("Testing Dataframe copy constructor");

    df::Dataframe original;
    original.add("doubles",
                 df::GenSerie<double>(3, {1.0, 2.0, 3.0, 4.0, 5.0, 6.0}));
    original.add("ints", df::GenSerie<int>(2, {1, 2, 3, 4}));

    // Test copy construction
    df::Dataframe copy(original);

    // Verify structure
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_EQ(copy.names(), original.names());

    // Verify data
    auto &orig_doubles = original.get<double>("doubles");
    auto &copy_doubles = copy.get<double>("doubles");
    EXPECT_ARRAY_EQ(orig_doubles.asArray(), copy_doubles.asArray());

    auto &orig_ints = original.get<int>("ints");
    auto &copy_ints = copy.get<int>("ints");
    EXPECT_ARRAY_EQ(orig_ints.asArray(), copy_ints.asArray());

    // Verify independence
    copy.get<double>("doubles").setValue(0, 99.0);
    EXPECT_NOT_EQ(copy.get<double>("doubles").value(0),
              original.get<double>("doubles").value(0));
}

TEST(Dataframe, AssignmentOperator) {
    MSG("Testing Dataframe assignment operator");

    df::Dataframe original;
    original.add("values", df::GenSerie<float>(2, {1.0f, 2.0f, 3.0f, 4.0f}));

    df::Dataframe assigned;
    assigned.add("other", df::GenSerie<int>(1, {9, 8, 7}));

    // Test assignment
    assigned = original;

    // Verify structure
    EXPECT_EQ(assigned.size(), original.size());
    EXPECT_EQ(assigned.names(), original.names());
    EXPECT_FALSE(assigned.has("other"));

    // Verify data
    auto &orig_values = original.get<float>("values");
    auto &assigned_values = assigned.get<float>("values");
    EXPECT_ARRAY_EQ(orig_values.asArray(), assigned_values.asArray());

    // Verify independence
    assigned.get<float>("values").setValue(0, 99.0f);
    EXPECT_NOT_EQ(assigned.get<float>("values").value(0),
              original.get<float>("values").value(0));

    // Test self-assignment
    original = original;
    EXPECT_EQ(original.size(), 1);
    EXPECT_TRUE(original.has("values"));
}

RUN_TESTS()
