/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge...
 * [MIT License text as in other files]
 */

namespace df {

template <size_t DIM>
inline Serie<double> distance_field(
    const Serie<typename detail::point_type<DIM>::type> &points,
    const Serie<typename detail::point_type<DIM>::type>
        &reference_points) {
    using point_t = typename detail::point_type<DIM>::type;

    if (points.empty() || reference_points.empty()) {
        throw std::runtime_error("Input series cannot be empty");
    }

    // Create temporary data for KDTree (it requires data Serie)
    Serie<size_t> indices(reference_points.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        indices[i] = i;
    }

    // Build KDTree with reference points
    KDTree<size_t, DIM> kdtree(indices, reference_points);

    // Compute distances using KDTree's efficient nearest neighbor search
    return points.map([&kdtree](const point_t &point, size_t) {
        auto [index, _] = kdtree.findNearest(point);
        return std::sqrt(kdtree.squaredDistance(index, point));
    });
}

template <size_t DIM>
inline auto
bind_distance_field(const Serie<typename detail::point_type<DIM>::type>
                        &reference_points) {
    return [&reference_points](
               const Serie<typename detail::point_type<DIM>::type>
                   &points) {
        return distance_field<DIM>(points, reference_points);
    };
}

} // namespace df