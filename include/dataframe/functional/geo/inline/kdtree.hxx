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
namespace geo {

template <typename T>
inline KDTree<T>::KDTree(const GenSerie<T> &points)
    : points_(points), dimension_(points.itemSize()) {
    if (points_.count() == 0)
        return;

    // Create initial indices array
    std::vector<uint32_t> indices(points_.count());
    std::iota(indices.begin(), indices.end(), 0);

    // Build tree
    root_ = build_tree(indices, 0, indices.size(), 0);
}

template <typename T>
inline KDTree<T>::Indices KDTree<T>::find_nearest(const std::vector<T> &target,
                                                  uint32_t k) const {
    if (k == 0 || !root_)
        return {};
    if (target.size() != dimension_) {
        throw std::invalid_argument(
            "Target point must have same dimension as KD-tree");
    }

    std::priority_queue<SearchNode, std::vector<SearchNode>,
                        std::greater<SearchNode>>
        pq;
    T max_dist = std::numeric_limits<T>::max();

    find_k_nearest(root_.get(), target, k, pq, max_dist);

    std::vector<uint32_t> result;
    while (!pq.empty()) {
        result.push_back(pq.top().node->point_idx);
        pq.pop();
    }
    std::reverse(result.begin(), result.end());
    return Indices(1, result);
}

template <typename T>
inline KDTree<T>::Indices KDTree<T>::find_radius(const std::vector<T> &target,
                                                 T radius) const {
    if (!root_)
        return {};
    if (target.size() != dimension_) {
        throw std::invalid_argument(
            "Target point must have same dimension as KD-tree");
    }

    std::vector<uint32_t> result;
    T radius_sq = radius * radius;

    std::function<void(Node *)> search = [&](Node *node) {
        if (!node)
            return;

        auto point = points_.array(node->point_idx);
        T dist_sq = 0;
        for (uint32_t i = 0; i < dimension_; ++i) {
            T diff = point[i] - target[i];
            dist_sq += diff * diff;
        }

        if (dist_sq <= radius_sq) {
            result.push_back(node->point_idx);
        }

        T axis_dist = target[node->axis] - point[node->axis];
        if (axis_dist <= 0) {
            search(node->left.get());
            if (axis_dist * axis_dist < radius_sq) {
                search(node->right.get());
            }
        } else {
            search(node->right.get());
            if (axis_dist * axis_dist < radius_sq) {
                search(node->left.get());
            }
        }
    };

    search(root_.get());
    return Indices(1, result);
}

template <typename T>
inline T KDTree<T>::distance_sq(uint32_t idx1,
                                const std::vector<T> &point2) const {
    auto point1 = points_.array(idx1);
    T dist = 0;
    for (uint32_t i = 0; i < dimension_; ++i) {
        T diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

template <typename T>
inline T KDTree<T>::distance_sq(uint32_t idx1, uint32_t idx2) const {
    auto point1 = points_.array(idx1);
    auto point2 = points_.array(idx2);
    T dist = 0;
    for (uint32_t i = 0; i < dimension_; ++i) {
        T diff = point1[i] - point2[i];
        dist += diff * diff;
    }
    return dist;
}

template <typename T>
inline std::unique_ptr<typename KDTree<T>::Node>
KDTree<T>::build_tree(std::vector<uint32_t> &indices, uint32_t start,
                      uint32_t end, uint32_t depth) {
    if (start >= end)
        return nullptr;

    // Choose axis based on depth
    uint32_t axis = depth % dimension_;

    // Sort points by the axis
    uint32_t mid = (start + end) / 2;
    std::nth_element(indices.begin() + start, indices.begin() + mid,
                     indices.begin() + end, [&](uint32_t a, uint32_t b) {
                         return points_.array(a)[axis] < points_.array(b)[axis];
                     });

    // Create node and build subtrees
    auto node = std::make_unique<Node>(indices[mid], axis);
    node->left = build_tree(indices, start, mid, depth + 1);
    node->right = build_tree(indices, mid + 1, end, depth + 1);
    return node;
}

template <typename T>
inline void KDTree<T>::find_k_nearest(KDTree<T>::Node *node,
                                      const std::vector<T> &target, uint32_t k,
                                      PQ &pq, T &max_dist) const {
    if (!node)
        return;

    auto point = points_.array(node->point_idx);
    T dist = 0;
    for (uint32_t i = 0; i < dimension_; ++i) {
        T diff = point[i] - target[i];
        dist += diff * diff;
    }

    if (dist < max_dist || pq.size() < k) {
        pq.push(SearchNode(node, dist));
        if (pq.size() > k)
            pq.pop();
        if (!pq.empty())
            max_dist = pq.top().distance;
    }

    T axis_dist = target[node->axis] - point[node->axis];
    Node *near_subtree = axis_dist <= 0 ? node->left.get() : node->right.get();
    Node *far_subtree = axis_dist <= 0 ? node->right.get() : node->left.get();

    find_k_nearest(near_subtree, target, k, pq, max_dist);

    if (axis_dist * axis_dist < max_dist || pq.size() < k) {
        find_k_nearest(far_subtree, target, k, pq, max_dist);
    }
}

template <typename T> inline KDTree<T> build_kdtree(const GenSerie<T> &points) {
    return KDTree<T>(points);
}

} // namespace geo
} // namespace df
