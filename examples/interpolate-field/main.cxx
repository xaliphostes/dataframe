#include <dataframe/Serie.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/geo/interpolate.h>
#include <dataframe/math/random.h>
#include <dataframe/types.h>
#include <fstream>

using namespace df;

int main() {
    // Create some scattered data points
    Serie<Vector2> scattered_points =
        random_uniform<Vector2>(50, Vector2{-1.0, -1.0}, Vector2{1.0, 1.0});

    // Create scalar field values at the scattered points
    auto values = scattered_points.map([](const Vector2 &p, size_t) {
        return std::sin(p[0] * 2) * std::cos(p[1] * 2);
    });

    // Create a regular grid where we want to interpolate the values
    auto grid_points =
        grid::cartesian::from_dims<2>({50, 50}, {0, 0}, {2.0, 2.0});

    // Interpolate the field
    auto interpolated_values =
        interpolate_field<double, 2>(grid_points, scattered_points, values,
                                     {.num_neighbors = 4,
                                      .power = 2.0,
                                      .smoothing = SmoothingMethod::Gaussian,
                                      .smoothing_radius = 0.1,
                                      .smoothing_iterations = 10});

    // Export results to CSV
    std::ofstream out_grid("grid_points.csv");
    out_grid << "x,y,distance\n";
    for (size_t i = 0; i < grid_points.size(); ++i) {
        out_grid << grid_points[i][0] << "," << grid_points[i][1] << ","
                 << interpolated_values[i] << "\n";
    }
    out_grid.close();

    // Export reference points
    std::ofstream out_ref("reference_points.csv");
    out_ref << "x,y\n";
    for (const auto &p : scattered_points.data()) {
        out_ref << p[0] << "," << p[1] << "\n";
    }
    out_ref.close();

    return 0;
}