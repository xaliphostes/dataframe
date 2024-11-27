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

#include <dataframe/Serie.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/math/equals.h>
#include "assertions.h"

int main()
{
    df::Serie s(1, {1, 2, 3, 4, 5});
    df::Serie solution(1, {6, 8, 10});

    // Using pipe directly
    auto result1 = pipe(s,
        [](const df::Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); }
    );
    assertCondition(df::equals(solution, result1));
    
    // Creating and using a reusable pipeline
    auto doubleAndFilter = df::make_pipe(
        [](const df::Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); }
    );

    auto result2 = doubleAndFilter(s);
    assertCondition(df::equals(solution, result2));

}
