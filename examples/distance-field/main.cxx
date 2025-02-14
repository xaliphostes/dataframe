#include <dataframe/Serie.h>
#include <dataframe/geo/distance_field.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/math/random.h>
#include <dataframe/types.h>
#include <fstream>

using namespace df;

/**
 * Read the [README.md](README.md) for instructions on how to plot the result.
 */
int main() {
    // Generate random reference points (nb_pts, min, max)
    Serie<Vector2> reference_points =
        random_uniform(10, Vector2{-1.0, -1.0}, Vector2{1.0, 1.0});

    // Create a regular grid for sampling (nb_pts, center, dimensions)
    auto grid_points =
        grid::cartesian::from_dims<2>({100, 100}, {0.0, 0.0}, {2.0, 2.0});

    // Compute distance field (at, ref)
    auto distances = distance_field<2>(grid_points, reference_points);

    // Export results to CSV
    std::ofstream out_grid("grid_points.csv");
    out_grid << "x,y,distance\n";
    for (size_t i = 0; i < grid_points.size(); ++i) {
        out_grid << grid_points[i][0] << "," << grid_points[i][1] << ","
                 << distances[i] << "\n";
    }
    out_grid.close();

    // Export reference points
    std::ofstream out_ref("reference_points.csv");
    out_ref << "x,y\n";
    for (const auto &p : reference_points.data()) {
        out_ref << p[0] << "," << p[1] << "\n";
    }
    out_ref.close();

    return 0;
}