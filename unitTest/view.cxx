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
#include <dataframe/Serie.h>
#include <dataframe/Dataframe.h>
#include <dataframe/utils/nameOfSerie.h>
#include <cmath>
#include <tuple>
#include <ranges>
#include "assertions.h"

int main()
{
    Array m(27);
    std::iota(std::begin(m), std::end(m), 0);
    df::Serie M(9, m);

    Array v(9);
    std::iota(std::begin(v), std::end(v), 0);
    df::Serie V(3, v);

    // --------------------------------
    // When C++23 will be available ...
    // See https://stackoverflow.com/posts/74163206/revisions
    // --------------------------------
    // for (auto elem : std::views::zip(M, V))
    // {
    //     std::cout << std::get<0>(elem) << ' '
    //               << std::get<1>(elem) << '\n';
    // }
    // --------------------------------


    // -----------------------------------------------
    // TODO... (like in TypeScript)
    // -----------------------------------------------
    // const reduced = map([M,V], ([m,v]) => {
    //     const A = squaredMatrix(m)
    //     const x = vector(v)
    //     return A.multVec(x).array
    // })
    //
    // expect(reduced.count).toEqual(3)
    // expect(reduced.itemSize).toEqual(3)
    //
    // const sol = [
    //     [  5,  14,  23],
    //     [122, 158, 194],
    //     [401, 464, 527]
    // ]
    //
    // reduced.forEach( (v,i) => {
    //     expect(v).toEqual(sol[i])
    // })
    // -----------------------------------------------

    return 0;
}
