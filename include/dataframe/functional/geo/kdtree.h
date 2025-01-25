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
#include <algorithm>
#include <dataframe/Serie.h>
#include <memory>
#include <queue>

namespace df {
namespace geo {

template <typename T> class KDTree {
  public:
    using Indices = df::GenSerie<uint32_t>;

    /**
     * @brief Construct KD-tree from points
     * @param points Serie containing point coordinates
     */
    KDTree(const GenSerie<T> &points);

    /**
     * @brief Find k nearest neighbors to target point
     * @param target Target point coordinates
     * @param k Number of neighbors to find
     * @return Vector of indices of nearest neighbors
     */
    Indices find_nearest(const std::vector<T> &target, uint32_t k) const;

    /**
     * @brief Find points within radius of target point
     * @param target Target point coordinates
     * @param radius Search radius
     * @return Vector of indices of points within radius
     */
    Indices find_radius(const std::vector<T> &target, T radius) const;

  private:
    struct Node {
        uint32_t point_idx; // Index of the point in original data
        uint32_t axis;      // Split axis
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        Node(uint32_t idx, uint32_t ax)
            : point_idx(idx), axis(ax), left(nullptr), right(nullptr) {}
    };

    struct SearchNode {
        Node *node;
        T distance;

        SearchNode(Node *n, T d) : node(n), distance(d) {}

        bool operator>(const SearchNode &other) const {
            return distance > other.distance;
        }
    };

    const GenSerie<T> &points_;
    std::unique_ptr<Node> root_;
    uint32_t dimension_;

    using PQ = std::priority_queue<SearchNode, std::vector<SearchNode>,
                                   std::greater<SearchNode>>;

    // Helper function to compute squared distance between points
    T distance_sq(uint32_t idx1, const std::vector<T> &point2) const;
    // Helper function to compute squared distance between two indices
    T distance_sq(uint32_t idx1, uint32_t idx2) const;
    // Build tree recursively
    std::unique_ptr<Node> build_tree(std::vector<uint32_t> &, uint32_t,
                                     uint32_t, uint32_t);
    // Find k nearest neighbors recursively

    void find_k_nearest(Node *, const std::vector<T> &, uint32_t, PQ &,
                        T &) const;
};

/**
 * @brief Build KD-tree from points
 * @param points Serie containing point coordinates
 * @return KDTree object for point queries
 */
template <typename T> KDTree<T> build_kdtree(const GenSerie<T> &points);

} // namespace geo
} // namespace df

#include "inline/kdtree.hxx"