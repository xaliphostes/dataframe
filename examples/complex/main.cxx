
#include <dataframe/Serie.h>
#include <dataframe/pipe.h>
#include <dataframe/utils/compose.h>
#include <dataframe/utils/unzip.h>
#include <dataframe/utils/zip.h>

/**
 * ## This example demonstrates several key features of the dataframe library:
 *
 * 1. ### Data Combination with zip:
 *
 * - Combines related time series data (timestamps, amplitudes, locations) into
 * a single series of tuples
 * - Makes it easier to process related measurements
 * together
 *
 * 2. ### Complex Processing with compose:
 *
 * - First transformation calculates time deltas and movement distances
 * - Second transformation computes velocities and normalizes amplitudes
 * - Operations are applied right-to-left, maintaining clear data flow
 *
 * 3. ### Data Separation with unzip:
 *
 * - Separates processed data back into individual series
 * - Makes it easy to analyze specific aspects independently
 *
 * 4. ### Analysis with pipe:
 *
 * - Processes velocity data to compute statistics
 * - Shows how pipe can be used for simple left-to-right transformations
 * 
 * 5. ### Display some results
 *
 * The example uses seismic sensor data to show a realistic use case where:
 *
 * - Data comes from multiple sources/sensors
 * - Processing requires considering multiple values together
 * - Analysis needs both combined and separate views of the data
 * - Transformations are applied in a clear, functional style
 */
int main() {
    // Raw sensor data: time, amplitude, and location (x,y)
    df::Serie<double> timestamps{0.0, 0.5, 1.0, 1.5, 2.0};
    df::Serie<double> amplitudes{0.2, 1.5, 0.8, 2.1, 1.3};
    df::Serie<Vector2> locations{
        {10.0, 20.0}, {10.2, 20.1}, {10.4, 20.3}, {10.5, 20.4}, {10.7, 20.6}};

    // Step 1: Combine related data using zip
    auto combined_data = df::zip(timestamps, amplitudes, locations);

    // Step 2: Process data using compose for a chain of transformations
    auto processed = df::compose(
        combined_data,
        // Compute velocity and normalize amplitude
        [](const auto &data) {
            return data.map([&data](const auto &tuple, size_t i) {
                auto [time, amp, loc] = tuple;

                // For first point, use 0 for velocity
                if (i == 0)
                    return std::make_tuple(0.0, amp / 2.5, loc);

                // Calculate velocity from previous point
                auto [prev_time, prev_amp, prev_loc] = data[i - 1];
                double dt = time - prev_time;
                double dx = loc[0] - prev_loc[0];
                double dy = loc[1] - prev_loc[1];
                double velocity =
                    dt > 0 ? std::sqrt(dx * dx + dy * dy) / dt : 0;
                double norm_amp =
                    amp / 2.5; // Normalize by max expected amplitude

                return std::make_tuple(velocity, norm_amp, loc);
            });
        });

    // Step 3: Unzip processed data for separate analysis
    auto [velocities, norm_amplitudes, final_locations] = df::unzip(processed);

    // Step 4: Analyze velocity data using pipe
    auto velocity_stats = velocities | [](const auto &v) {
        double sum = 0, max_vel = 0;
        v.forEach([&](double vel, size_t) {
            sum += vel;
            max_vel = std::max(max_vel, vel);
        });
        return std::make_tuple(sum / v.size(), max_vel);
    };

    auto [avg_velocity, max_velocity] = velocity_stats;

    // 5. Print results
    std::cout << "Seismic Analysis Results:\n"
              << "Average velocity: " << avg_velocity << " units/s\n"
              << "Maximum velocity: " << max_velocity << " units/s\n";

    return 0;
}
