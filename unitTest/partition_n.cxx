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

#include "assertions.h"
#include <dataframe/Serie.h>
#include <dataframe/functional/utils/partition_n.h>
#include <dataframe/functional/utils/print.h>
#include <tuple>

START_TEST(partition_n) {
    df::Serie s1(1, {1, 2, 3, 4});                            // scalars
    df::Serie s2(3, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12}); // 4 3D vectors
    df::Serie s3(1, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10});         // scalars

    // Partition single Serie into 2 parts
    message("Partition single Serie into 2 parts (1)");
    {
        auto parts = df::utils::partition_n(2, s1);
        assertEqual<size_t>(parts.size(), 2);
        assertEqual<size_t>(parts[0].size(), 2);
        assertArrayEqual(parts[0].asArray(), {1, 2});
        assertArrayEqual(parts[1].asArray(), {3, 4});
    }

    message("Partition single Serie into 2 parts (2)");
    {
        auto parts = df::utils::partition_n(2, s3);
        assertEqual<size_t>(parts.size(), 2);
        assertEqual<size_t>(parts[0].size(), 5);
        assertEqual<size_t>(parts[1].size(), 5);
        assertArrayEqual(parts[0].asArray(), {1, 2, 3, 4, 5});
        assertArrayEqual(parts[1].asArray(), {6, 7, 8, 9, 10});
    }

    // Partition multiple Series
    message("Partition multiple Serie into 2 parts");
    {
        auto parts = df::utils::partition_n(2, s1, s2);
        assertEqual<size_t>(parts.size(), 2);
        assertEqual<size_t>(parts[0].size(), 2);
        assertEqual<size_t>(parts[1].size(), 2);
        assertArrayEqual(parts[0][0].asArray(), {1, 2});
        assertArrayEqual(parts[0][1].asArray(), {3, 4});
        assertArrayEqual(parts[1][0].asArray(), {1, 2, 3, 4, 5, 6});
        assertArrayEqual(parts[1][1].asArray(), {7, 8, 9, 10, 11, 12});
    }

    // with remainder
    message("Partition single Serie into 3 parts with remainder");
    {
        auto parts = df::utils::partition_n(3, s3);
        assertEqual<size_t>(parts.size(), 3);
        assertEqual<size_t>(parts[0].size(), 4);
        assertEqual<size_t>(parts[1].size(), 3);
        assertEqual<size_t>(parts[2].size(), 3);
        assertArrayEqual(parts[0].asArray(), {1, 2, 3, 4});
        assertArrayEqual(parts[1].asArray(), {5, 6, 7});
        assertArrayEqual(parts[2].asArray(), {8, 9, 10});
    }
}
END_TEST()

int main() {

    partition_n();

    return 0;
}
