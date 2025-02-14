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

#include <algorithm>
#include <memory>
#include <numeric>

namespace df {

struct Node {
    size_t index;
    size_t axis;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    Node(size_t idx, size_t ax)
        : index(idx), axis(ax), left(nullptr), right(nullptr) {}
};

template <typename T, size_t DIM>
inline double KDTree<T, DIM>::getComponent(size_t idx, size_t axis) const {
    return positions_[idx][axis];
}

template <typename T, size_t DIM>
inline std::unique_ptr<Node>
KDTree<T, DIM>::buildTree(std::vector<size_t> &indices, size_t depth) {
    if (indices.empty())
        return nullptr;

    const size_t axis = depth % DIM;

    // Sort indices based on the current axis
    std::sort(indices.begin(), indices.end(), [this, axis](size_t a, size_t b) {
        return getComponent(a, axis) < getComponent(b, axis);
    });

    // Get median index
    const size_t median = indices.size() / 2;
    auto node = std::make_unique<Node>(indices[median], axis);

    // Split indices into left and right
    std::vector<size_t> leftIndices(indices.begin(), indices.begin() + median);
    std::vector<size_t> rightIndices(indices.begin() + median + 1,
                                     indices.end());

    // Recursively build subtrees
    node->left = buildTree(leftIndices, depth + 1);
    node->right = buildTree(rightIndices, depth + 1);

    return node;
}

template <typename T, size_t DIM>
inline double KDTree<T, DIM>::squaredDistance(size_t idx,
                                              const point_t &point) const {
    double dist = 0.0;
    for (size_t i = 0; i < DIM; ++i) {
        double diff = positions_[idx][i] - point[i];
        dist += diff * diff;
    }
    return dist;
}

template <typename T, size_t DIM>
inline void KDTree<T, DIM>::searchNN(const Node *node, const point_t &point,
                                     size_t &bestIndex,
                                     double &bestDist) const {
    if (!node)
        return;

    const size_t axis = node->axis;
    const double nodeDist = squaredDistance(node->index, point);

    // Update best distance if current node is closer
    if (nodeDist < bestDist) {
        bestDist = nodeDist;
        bestIndex = node->index;
    }

    // Determine which subtree to search first
    const double axisDiff = point[axis] - getComponent(node->index, axis);
    auto firstSide = axisDiff <= 0 ? node->left.get() : node->right.get();
    auto secondSide = axisDiff <= 0 ? node->right.get() : node->left.get();

    // Search the closer subtree
    searchNN(firstSide, point, bestIndex, bestDist);

    // Check if we need to search the other subtree
    if (axisDiff * axisDiff < bestDist) {
        searchNN(secondSide, point, bestIndex, bestDist);
    }
}

template <typename T, size_t DIM>
inline KDTree<T, DIM>::KDTree(const Serie<T> &data,
                              const Serie<point_t> &positions)
    : data_(data), positions_(positions) {
    if (data.size() != positions.size()) {
        throw std::runtime_error(
            "Data and positions series must have the same size");
    }

    std::vector<size_t> indices(data.size());
    std::iota(indices.begin(), indices.end(), 0);
    root_ = buildTree(indices, 0);
}

template <typename T, size_t DIM>
inline KDTree<T, DIM>::Neighbor
KDTree<T, DIM>::findNearest(const point_t &point) const {
    size_t bestIndex = 0;
    double bestDist = std::numeric_limits<double>::max();

    searchNN(root_.get(), point, bestIndex, bestDist);
    return {bestIndex, data_[bestIndex]};
}

template <typename T, size_t DIM>
inline std::vector<typename KDTree<T, DIM>::Neighbor>
KDTree<T, DIM>::findNearest(const Serie<point_t> &points, size_t k) const {

    struct NeighborInfo {
        size_t index;
        double distance;
        NeighborInfo(size_t idx, double dist) : index(idx), distance(dist) {}
        bool operator<(const NeighborInfo &other) const {
            return distance < other.distance;
        }
    };

    std::vector<std::pair<size_t, const T &>> result;
    result.reserve(k * points.size());

    // Process each query point
    points.forEach([&](const point_t &point, size_t pointIdx) {
        std::vector<NeighborInfo> neighbors;
        neighbors.reserve(k);

        // Helper function to update neighbors for current point
        auto updateNeighbors = [&](size_t idx, double dist) {
            if (neighbors.size() < k) {
                neighbors.emplace_back(idx, dist);
                std::push_heap(neighbors.begin(), neighbors.end());
            } else if (dist < neighbors.front().distance) {
                std::pop_heap(neighbors.begin(), neighbors.end());
                neighbors.back() = NeighborInfo(idx, dist);
                std::push_heap(neighbors.begin(), neighbors.end());
            }
        };

        // Search function for current point
        std::function<void(const Node *)> searchKNN = [&](const Node *node) {
            if (!node)
                return;

            const size_t axis = node->axis;
            const double nodeDist = squaredDistance(node->index, point);

            updateNeighbors(node->index, nodeDist);

            const double axisDiff =
                point[axis] - getComponent(node->index, axis);
            auto firstSide =
                axisDiff <= 0 ? node->left.get() : node->right.get();
            auto secondSide =
                axisDiff <= 0 ? node->right.get() : node->left.get();

            searchKNN(firstSide);

            const double maxDist = neighbors.size() < k
                                       ? std::numeric_limits<double>::max()
                                       : neighbors.front().distance;
            if (axisDiff * axisDiff < maxDist) {
                searchKNN(secondSide);
            }
        };

        // Perform search for current point
        searchKNN(root_.get());

        // Sort neighbors by distance and add to result
        std::sort_heap(neighbors.begin(), neighbors.end());
        for (const auto &neighbor : neighbors) {
            result.emplace_back(neighbor.index, data_[neighbor.index]);
        }
    });

    return result;
}

} // namespace df
