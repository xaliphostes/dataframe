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
#include <dataframe/functional/unzip.h>
#include <dataframe/functional/reject.h>
#include <dataframe/functional/reduce.h>
#include <dataframe/functional/zip.h>
#include <map>

using namespace df;

int main() {
    // Computing average stress state around faults in a 3D geological model:

    // Input data
    Serie stress(6, {
                        // Stress tensors (xx,xy,xz,yy,yz,zz)
                        1,  0,  0, 1,  0, 1,  // Point 1
                        2,  1,  0, 2,  0, 2,  // Point 2
                        -1, 0,  0, -1, 0, -1, // Point 3
                        -2, -1, 0, -2, 0, -2  // Point 4
                    });

    Serie distances(1, {
                           // Distance to nearest fault
                           5.0, // Point 1
                           2.0, // Point 2
                           1.0, // Point 3
                           0.5  // Point 4
                       });

    Serie rockTypes(1, {
                           // Rock type markers
                           1, // Point 1: granite
                           1, // Point 2: granite
                           2, // Point 3: basalt
                           2  // Point 4: basalt
                       });

    // Pipeline to:
    // 1. Reject points too far from faults (> 2.0)
    // 2. Filter compressive states only (trace < 0)
    // 3. Compute average stress state by rock type

    auto [filtered_stress, filtered_rocks] = pipe(
        zip(stress, distances, rockTypes),

        // Reject points far from faults
        reject([](const Array &data, uint32_t) {
            double distance = data[6]; // Index after stress components
            return distance > 2.0;
        }),

        // Keep only compressive states
        filter([](const Array &data, uint32_t) {
            double trace = data[0] + data[3] + data[5]; // xx + yy + zz
            return trace < 0;
        }),

        // Unzip to get separate series
        [](const Serie &combined) {
            return unzip(combined, {6, 1}); // Split into stress and rock type
        });

    // Compute average stress by rock type
    auto avg_by_type = reduce(
        [](std::map<int, Array> &acc, const Array &data, uint32_t) {
            Array stress(data.begin(), data.begin() + 6);
            int rockType = static_cast<int>(data[6]);

            if (acc.find(rockType) == acc.end()) {
                acc[rockType] = {stress, 1}; // First entry
            } else {
                // Update running average
                auto &existing = acc[rockType];
                for (size_t i = 0; i < 6; ++i) {
                    existing[i] = (existing[i] * existing[6] + stress[i]) /
                                  (existing[6] + 1);
                }
                existing[6]++; // Increment count
            }
            return acc;
        },
        zip(filtered_stress, filtered_rocks),
        std::map<int, Array>{});
}
