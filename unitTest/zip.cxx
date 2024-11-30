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

#include <dataframe/functional/zip.h>
#include <dataframe/Serie.h>

int main() {
    // Create some test Series
    df::Serie s1(1, {1, 2, 3});                // scalar serie
    df::Serie s2(2, {4,5, 6,7, 8,9});          // 2D serie
    df::Serie s3(1, {10, 11, 12});             // scalar serie
    
    // Using variadic zip
    auto result1 = df::zip(s1, s2, s3);
    // result1.dump();  // Will show the combined values
    std::cerr << result1 << std::endl ;
    
    // Using vector zip
    std::vector<df::Serie> series = {s1, s2, s3};
    auto result2 = df::zipVector(series);
    result2.dump();  // Will show the same combined values
    
    // The results can be used in further operations
    result1.forEach([](const Array& values, uint32_t i) {
        // Process the combined values
        // values will contain {1,4,5,10} for i=0, {2,6,7,11} for i=1, etc.
    });
}
