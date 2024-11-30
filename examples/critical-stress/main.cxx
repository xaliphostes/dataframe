#include <dataframe/Serie.h>
#include <dataframe/functional/algebra/eigen.h>
#include <dataframe/functional/filter.h>
#include <dataframe/functional/forEach.h>
#include <dataframe/functional/map.h>
#include <dataframe/functional/pipe.h>
#include <dataframe/functional/stats/mean.h>
#include <dataframe/functional/zip.h>
#include <cmath>

using namespace df;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int main() {
    // Input data structures

    // Positions of measures (x,y,z)
    Serie positions(3, {0, 0, 0,   1, 0, 0, 2, 0, 0});

    // Stress tensors (xx,xy,xz,yy,yz,zz)
    Serie stress(6, {-2, 4, 6, -3, 6, -9,   1, 2, 3, 4, 5, 6,   9, 8, 7, 6, 5, 4});

    // Geologic markers (0=sandstone, 1=granit...)
    Serie markers(1, {1, 2, 2, 1});

    double cohesion = 0;
    double friction_angle = 30 * M_PI / 180;

    // Compute the critical stress state
    auto computeCriticalityIndex = [=](const Serie &stress,
                                       const Serie &positions) {
        Serie result(1, stress.count());

        for (uint32_t i = 0; i < stress.count(); ++i) {
            const Array &values = stress.get<Array>(i);
            const Array &pos = positions.get<Array>(i);

            double sigma1 = values[0];
            double sigma3 = values[2];
            double deviatoric = sigma1 - sigma3;

            double critical_stress = 2 * cohesion * std::cos(friction_angle) /
                                     (1 - std::sin(friction_angle));

            double depth_factor = std::exp(-(-pos[2]) / 1000.0);
            result.set(i, (deviatoric / critical_stress) * depth_factor);
        }
        return result;
    };

    // Principal pipeline
    auto result = pipe(
        stress,

        // 1. Eigen values and vectors
        [](const Serie &s) {
            auto [val, vec] = eigenSystem(s);
            return val; // We only need eigenvalues for filtering
        },

        // 2. Filter stress and positions
        [&positions](const Serie &stress_values) {
            // First get filtered stress indices
            auto indices = std::vector<uint32_t>();
            for (uint32_t i = 0; i < stress_values.count(); ++i) {
                if (stress_values.get<Array>(i)[0] < 0) {
                    indices.push_back(i);
                }
            }

            std::cerr << stress_values << std::endl ;

            // Apply filter to both series
            Serie filtered_positions(positions.itemSize(), indices.size());
            Serie filtered_stress(stress_values.itemSize(), indices.size());

            for (uint32_t i = 0; i < indices.size(); ++i) {
                filtered_positions.set(i, positions.get<Array>(indices[i]));
                filtered_stress.set(i, stress_values.get<Array>(indices[i]));
            }

            return std::make_pair(filtered_stress, filtered_positions);
        },

        // 3. Compute criticality
        [computeCriticalityIndex](const auto &pair) {
            const auto &[filtered_stress, filtered_positions] = pair;
            return computeCriticalityIndex(filtered_stress, filtered_positions);
        });

    // Mean
    auto mean_criticality = mean(result);
    if (mean_criticality.isNumber) {
        std::cerr << mean_criticality.number << std::endl;
    } else {
        std::cerr << mean_criticality.array << std::endl;
    }
}
