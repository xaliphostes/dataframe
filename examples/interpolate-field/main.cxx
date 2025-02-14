#include <dataframe/Serie.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/geo/interpolate.h>
#include <dataframe/math/random.h>
#include <dataframe/types.h>
#include <fstream>

using namespace df;

int main() {
    // Create some scattered data points (2D)
    Serie<Vector2> points =
        random_uniform<Vector2>(50, Vector2{-1.0, -1.0}, Vector2{1.0, 1.0});

    // Create scalar field values at these points
    auto scalars = points.map([](const Vector2 &p, size_t) {
        return std::sin(p[0]*2) * std::cos(p[1]*2);
    });

    // Create regular grid for interpolation
    auto grid =
        grid::cartesian::from_dims<2>({50, 50}, {0.0, 0.0}, {2.0, 2.0});

    // Interpolate scalar field
    auto interpolated_scalar = interpolate_field<double, 2>(
        grid, points, scalars, {
            .method = InterpolationType::NearestNeighbor,
            .num_neighbors = 4,
            .power = 2.0
        });

    // Export results to CSV
    std::ofstream out_grid("grid_points.csv");
    out_grid << "x,y,distance\n";
    for (size_t i = 0; i < grid.size(); ++i) {
        out_grid << grid[i][0] << "," << grid[i][1] << ","
                 << interpolated_scalar[i] << "\n";
    }
    out_grid.close();

    // Export reference points
    std::ofstream out_ref("reference_points.csv");
    out_ref << "x,y\n";
    for (const auto &p : points.data()) {
        out_ref << p[0] << "," << p[1] << "\n";
    }
    out_ref.close();

    // // Create vector field values
    // auto vector_values = scattered_points.map([](const Vector2 &p, size_t) {
    //     return Vector2{std::sin(p[0]), std::cos(p[1])}; // Example vector field
    // });

    // // Interpolate vector field
    // InterpolationParams<Vector2> vector_params;
    // vector_params.method = InterpolationType::RBF;
    // vector_params.num_neighbors = 6;

    // auto interpolated_vector = interpolate_field<Vector2, 2>(
    //     grid_points, scattered_points, vector_values, vector_params);

    // // Custom weight function example
    // InterpolationParams<double> custom_params;
    // custom_params.weight_function = [](double d) {
    //     return 1.0 / (1.0 + d * d); // Custom weight function
    // };

    // auto interpolated_custom = interpolate_field<double, 2>(
    //     grid_points, scattered_points, scalar_values, custom_params);

    return 0;
}