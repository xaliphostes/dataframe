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

namespace df {

#ifdef USE_CGAL

/**
 * @brief Natural Neighbor interpolation for 2D points
 *
 * This implementation uses the Sibson's method which is based on the ratio of
 * stolen areas when inserting a new point into the Voronoi diagram.
 *
 * The steps are:
 * 1. Create Delaunay triangulation of input points
 * 2. For each target point:
 *    - Find the natural neighbors (points whose Voronoi cells will be affected)
 *    - Calculate the areas of overlap between the new and old Voronoi cells
 *    - Use these areas as weights for interpolation
 */
template <typename T> class NaturalNeighborInterpolator {
  private:
    using K = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Point_2 = K::Point_2;
    using DT = CGAL::Delaunay_triangulation_2<K>;
    using Vertex_handle = DT::Vertex_handle;

    DT triangulation;
    std::map<Vertex_handle, T> values_map;

    /**
     * @brief Calculate area of a polygon defined by points
     */
    double calculate_area(const std::vector<Point_2> &points) const {
        if (points.size() < 3)
            return 0.0;

        double area = 0.0;
        for (size_t i = 0; i < points.size(); ++i) {
            const Point_2 &p1 = points[i];
            const Point_2 &p2 = points[(i + 1) % points.size()];
            area += (p1.x() * p2.y() - p2.x() * p1.y());
        }
        return std::abs(area) / 2.0;
    }

    /**
     * @brief Get the vertices of the Voronoi cell for a point
     */
    std::vector<Point_2> get_voronoi_cell(const Point_2 &p,
                                          const Vertex_handle &v) const {
        std::vector<Point_2> cell;

        // Get circulator of vertices around the point
        auto start = triangulation.incident_vertices(v);
        auto curr = start;

        do {
            if (triangulation.is_infinite(curr))
                continue;

            // Get circumcenter (Voronoi vertex)
            auto face = triangulation.dual(curr);
            cell.push_back(face);

        } while (++curr != start);

        return cell;
    }

  public:
    /**
     * @brief Initialize interpolator with input points and values
     */
    void initialize(const Serie<Vector2> &points, const Serie<T> &values) {
        if (points.size() != values.size()) {
            throw std::runtime_error("Points and values must have same size");
        }

        // Insert points into triangulation and store values
        for (size_t i = 0; i < points.size(); ++i) {
            const auto &p = points[i];
            auto vh = triangulation.insert(Point_2(p[0], p[1]));
            values_map[vh] = values[i];
        }
    }

    /**
     * @brief Interpolate value at a target point
     */
    T interpolate(const Vector2 &target) const {
        Point_2 query(target[0], target[1]);

        // Find cell containing the target point
        auto nearest = triangulation.nearest_vertex(query);
        if (CGAL::squared_distance(nearest->point(), query) < 1e-10) {
            return values_map.at(nearest);
        }

        // Get natural neighbors
        std::vector<std::pair<Vertex_handle, double>> weights;

        // Insert point temporarily to find natural neighbors
        auto vh = triangulation.insert(query);

        // Get Voronoi cell of inserted point
        auto new_cell = get_voronoi_cell(query, vh);
        double total_area = calculate_area(new_cell);

        // For each natural neighbor
        auto start = triangulation.incident_vertices(vh);
        auto curr = start;

        do {
            if (triangulation.is_infinite(curr))
                continue;

            // Calculate stolen area
            auto old_cell = get_voronoi_cell(curr->point(), curr);
            std::vector<Point_2> intersection;
            CGAL::intersection(new_cell, old_cell,
                               std::back_inserter(intersection));

            double area = calculate_area(intersection);
            weights.emplace_back(curr, area / total_area);

        } while (++curr != start);

        // Remove temporary point
        triangulation.remove(vh);

        // Calculate interpolated value
        T result = T();
        for (const auto &[neighbor, weight] : weights) {
            result += values_map.at(neighbor) * weight;
        }

        return result;
    }

    /**
     * @brief Interpolate values at multiple target points
     */
    Serie<T> interpolate(const Serie<Vector2> &targets) const {
        Serie<T> result(targets.size());

#pragma omp parallel for if (targets.size() > 1000)
        for (size_t i = 0; i < targets.size(); ++i) {
            result[i] = interpolate(targets[i]);
        }

        return result;
    }
};

/**
 * @brief Convenience function for natural neighbor interpolation
 */
template <typename T>
inline Serie<T> natural_neighbor_2d(const Serie<Vector2> &points,
                                    const Serie<T> &values,
                                    const Serie<Vector2> &targets) {
    NaturalNeighborInterpolator<T> interpolator;
    interpolator.initialize(points, values);
    return interpolator.interpolate(targets);
}

#else

/**
 * @brief Natural neighbor interpolation. This function is empty as CGAL is not
 * used
 */
template <typename T>
inline Serie<T> natural_neighbor_2d(const Serie<Vector2> &, const Serie<T> &,
                                    const Serie<Vector2> &) {
    return Serie<T>();
}

#endif

} // namespace df