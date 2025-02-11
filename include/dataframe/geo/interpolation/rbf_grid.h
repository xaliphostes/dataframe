

/**
 * @brief RBF interpolation optimized for regular 2D grids
 * @param grid Grid information
 * @param values Serie of values at grid points (must match grid dimensions)
 * @param targets Serie of points to interpolate to
 * @param kernel RBF kernel type
 * @param epsilon Shape parameter for the kernel
 * @param support_radius Number of grid cells to consider for local
 * interpolation
 * @return Serie of interpolated values
 */

#include "rbf_kernels.h"
#include <dataframe/Serie.h>

namespace df {

class Grid2D;
class Grid3D;

/**
 * @code
 * // Define a 2D grid
 * df::Grid2D grid {
 *     {0.0, 0.0},     // origin
 *     {0.1, 0.1},     // spacing
 *     {100, 100}      // dimensions
 * };
 *
 * // Create values on the grid
 * df::Serie<double> grid_values(grid.total_points());
 * // ... fill grid_values ...
 *
 * // Points to interpolate to
 * df::Serie<df::Vector2> targets = {
 *     {1.5, 2.3}, {4.2, 3.1}
 * };
 *
 * // Interpolate using grid-optimized RBF
 * auto interpolated = df::rbf_grid_2d(
 *     grid,
 *     grid_values,
 *     targets,
 *     df::RBFKernel::Multiquadric,
 *     1.0,    // epsilon
 *     2       // support_radius in grid cells
 * );
 * @endcode
 */
template <typename T>
Serie<T> rbf_grid_2d(const Grid2D &grid, const Serie<T> &values,
                     const Serie<Vector2> &targets,
                     RBFKernel kernel = RBFKernel::Multiquadric,
                     double epsilon = 1.0, uint support_radius = 2);

/**
 * @brief RBF interpolation optimized for regular 3D grids
 * Similar to 2D version but for 3D grids
 */
template <typename T>
Serie<T> rbf_grid_3d(const Grid3D &grid, const Serie<T> &values,
                     const Serie<Vector3> &targets,
                     RBFKernel kernel = RBFKernel::Multiquadric,
                     double epsilon = 1.0, uint support_radius = 2);

} // namespace df

#include "inline/rbf_grid.hxx"