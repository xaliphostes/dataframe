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

#pragma once
#include <array>
#include <dataframe/Serie.h>
#include <dataframe/types.h>
#include <vector>

namespace df {

class Node; // forward decl.

/**
 * @brief A k-dimensional tree implementation for spatial queries on Serie data
 * types
 *
 * The KDTree class provides an efficient spatial indexing structure for
 * performing nearest neighbor searches on Serie data types in k-dimensional
 * space. It supports both 2D and 3D spatial coordinates through template
 * specialization.
 *
 * The tree maintains a spatial hierarchy of points while preserving the
 * association with arbitrary data stored in the Serie. This allows for
 * efficient spatial queries while maintaining access to the associated data.
 *
 * Features:
 * - Template-based design supporting any data type T
 * - Specialized for 2D and 3D spaces using Vector2 and Vector3 types
 * - Efficient nearest neighbor search
 * - K-nearest neighbors search capability
 * - Memory-efficient implementation using smart pointers
 *
 * Example usage:
 * @code
 * // Create sample data for 2D space
 * Serie<std::string> labels{"A", "B", "C", "D"};
 * Serie<Vector2> positions{
 *     {0.0, 0.0},
 *     {1.0, 1.0},
 *     {2.0, 0.0},
 *     {0.0, 2.0}
 * };
 *
 * // Create KDTree
 * KDTree<std::string, 2> tree(labels, positions);
 *
 * // Find nearest neighbor
 * Vector2 queryPoint{1.5, 1.5};
 * auto [index, value] = tree.findNearest(queryPoint);
 *
 * // Find k nearest neighbors for multiple points
 * Serie<Vector2> queryPoints{{1.0, 1.0}, {2.0, 2.0}};
 * auto neighbors = tree.findNearest(queryPoints, 2);
 * @endcode
 *
 * @note The size of the position Serie must match the size of the data Serie.
 * @warning The tree structure is immutable after construction. For dynamic
 * point sets, a new tree must be constructed.
 *
 * @tparam T The type of data stored in the Serie
 * @tparam DIM The dimension of the space (2 or 3)
 */
template <typename T, size_t DIM> class KDTree {
  public:
    using point_t = detail::point_type<DIM>::type;
    using Neighbor = std::pair<size_t, const T &>;

    /**
     * @brief Constructs a KDTree from data and position Series
     *
     * Creates a balanced k-d tree structure from the provided data and position
     * Series. The tree is constructed using the median splitting approach,
     * which provides good average-case performance for nearest neighbor
     * queries.
     *
     * @param data The Serie containing the data values associated with each
     * point
     * @param positions The Serie containing spatial coordinates for each point
     * @throws std::runtime_error if data and positions Series have different
     * sizes
     *
     * Example:
     * @code
     * Serie<std::string> data{"A", "B", "C"};
     * Serie<Vector2> pos{{0,0}, {1,1}, {2,2}};
     * KDTree<std::string, 2> tree(data, pos);
     * @endcode
     */
    KDTree(const Serie<T> &data, const Serie<point_t> &positions);

    /**
     * @brief Finds the nearest neighbor to a given point
     *
     * Performs an efficient search through the k-d tree structure to find the
     * point closest to the query point according to Euclidean distance.
     *
     * @param point The query point in k-dimensional space
     * @return Neighbor A pair containing:
     *         - The index of the nearest neighbor in the original Serie
     *         - A const reference to the associated data value
     *
     * Example:
     * @code
     * auto [index, value] = tree.findNearest({1.0, 1.0});
     * std::cout << "Nearest point index: " << index << ", value: " << value <<
     * "\n";
     * @endcode
     */
    Neighbor findNearest(const point_t &point) const;

    /**
     * @brief Finds k nearest neighbors for multiple query points
     *
     * For each point in the input Serie, finds the k closest points according
     * to Euclidean distance. The results are returned in ascending order of
     * distance for each query point.
     *
     * @param points Serie of query points
     * @param k Number of nearest neighbors to find for each query point
     * @return std::vector<Neighbor> Vector containing k nearest neighbors for
     * each query point, where each neighbor is represented by its index and
     * associated data
     *
     * @note The total size of the return vector will be (number of query points
     * × k)
     *
     * Example:
     * @code
     * Serie<Vector2> queries{{0.5, 0.5}, {1.5, 1.5}};
     * auto neighbors = tree.findNearest(queries, 2);
     * for(const auto& [idx, value] : neighbors) {
     *     std::cout << "Index: " << idx << ", Value: " << value << "\n";
     * }
     * @endcode
     */
    std::vector<Neighbor> findNearest(const Serie<point_t> &, size_t) const;

    /**
     * @brief Find all points within a given radius of a query point
     *
     * @param target Query point to search around
     * @param radius Search radius
     * @param result Vector to store indices of points within radius
     */
    void findInRadius(const point_t &, double, std::vector<size_t> &) const;

    double squaredDistance(size_t idx, const point_t &point) const;

  private:
    std::unique_ptr<Node> root_;
    const Serie<T> &data_;
    const Serie<point_t> &positions_;

    double getComponent(size_t idx, size_t axis) const;
    std::unique_ptr<Node> buildTree(std::vector<size_t> &, size_t);
    void searchNN(const Node *, const point_t &, size_t &, double &) const;
    void radiusSearch(const Node *node, const point_t &target,
                      double squared_radius, std::vector<size_t> &result) const;

    /**
     * @brief Get the component value of a point at a given axis
     *
     * @param point The point to get component from
     * @param axis The axis index (0 for x, 1 for y, etc.)
     * @return Component value at the specified axis
     */
    double get_component(const point_t &point, size_t axis) const;

    /**
     * @brief Get the component value from index at a given axis
     *
     * @param idx Index in the positions series
     * @param axis The axis index (0 for x, 1 for y, etc.)
     * @return Component value at the specified axis
     */
    double get_component(size_t idx, size_t axis) const;
};

} // namespace df

#include "inline/kdtree.hxx"