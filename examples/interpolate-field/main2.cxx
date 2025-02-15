#include <dataframe/Serie.h>
#include <dataframe/geo/grid/from_dims.h>
#include <dataframe/geo/interpolate2.h>
#include <dataframe/types.h>
#include <dataframe/utils/zip.h>
#include <dataframe/utils/concat.h>
#include <fstream>

using namespace df;

int main() {
    // Create points for two groups
    Serie<Vector2> red_points = {{0.0, -0.2},  {0.2, -0.15}, {0.4, -0.1},
                                 {0.6, -0.05}, {0.8, 0.0},   {1.0, 0.05}};

    Serie<Vector2> blue_points = {{0.0, 0.2},  {0.2, 0.15}, {0.4, 0.1},
                                  {0.6, 0.05}, {0.8, 0.0},  {1.0, -0.05}};

    // Combine points
    Serie<Vector2> scattered_points = concat(red_points, blue_points);
    // scattered_points.add(red_points.data());
    // scattered_points.add(blue_points.data());

    // Create group information
    Serie<GroupInfo> group_info;
    for (size_t i = 0; i < red_points.size(); ++i) {
        group_info.add({1, 1.0}); // Red group
    }
    for (size_t i = 0; i < blue_points.size(); ++i) {
        group_info.add({2, 2.0}); // Blue group
    }

    // Create field values
    auto values = scattered_points.map([](const Vector2 &p, size_t) {
        return std::sin(p[0] * 2) * std::cos(p[1] * 2);
    });

    // Define curved discontinuity
    CurvedDiscontinuity disc;
    disc.control_points = {{0.0, 0.0}, {0.3, 0.1}, {0.6, -0.1}, {1.0, 0.0}};
    disc.num_segments = 100;

    // Setup interpolation parameters
    InterpolationParams params;
    params.num_neighbors = 4;
    params.power = 2.0;
    params.smoothing = SmoothingMethod::Gaussian;
    params.smoothing_radius = 0.1;
    params.smoothing_iterations = 2;
    params.discontinuities = {disc};
    params.respect_groups = true;
    params.cross_group_penalty = 2.0;

    // Create regular grid
    auto grid_points =
        grid::cartesian::from_dims<2>({50, 50}, {0.0, -0.5}, {1.0, 1.0});

    // Interpolate the field
    auto interpolated = interpolate_field<double, 2>(
        grid_points, scattered_points, values, group_info, params);

    // Export the interpolated field to a CSV file
    std::ofstream out_grid("grid_points.csv");
    out_grid << "x,y,distance\n";
    for (size_t i = 0; i < grid_points.size(); ++i) {
        out_grid << grid_points[i][0] << "," << grid_points[i][1] << ","
                 << interpolated[i] << "\n";
    }
    out_grid.close();

    // Export reference points with group information
    std::ofstream out_ref("reference_points.csv");
    out_ref << "x,y,group,weight\n";
    for (size_t i = 0; i < scattered_points.size(); ++i) {
        out_ref << scattered_points[i][0] << "," << scattered_points[i][1]
                << "," << group_info[i].group_id << "," << group_info[i].weight
                << "\n";
    }
    out_ref.close();

    // Export discontinuity curve points for visualization
    std::ofstream out_disc("discontinuity.csv");
    out_disc << "x,y\n";
    for (size_t i = 0; i <= disc.num_segments; ++i) {
        double t = static_cast<double>(i) / disc.num_segments;
        Vector2 p = disc.evaluate(t);
        out_disc << p[0] << "," << p[1] << "\n";
    }
    out_disc.close();

    return 0;
}