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

template <size_t N>
template <typename T>
HarmonicDiffusion<N>::HarmonicDiffusion(const Mesh<N> &mesh,
                                        const T &init_value)
    : mesh_(mesh), max_iter_(618), eps_(0.382e-5), epsilon_(0.5) {
    initializeValues(init_value);
}

template <size_t N>
template <typename T>
HarmonicDiffusion<N>::HarmonicDiffusion(const Serie<Vector3> &vertices,
                                        const Serie<iVector3> &triangles,
                                        const T &init_value)
    : max_iter_(618), eps_(0.382e-5), epsilon_(0.5) {
    mesh_ = Mesh<N>(vertices, triangles);
    initializeValues(init_value);
}

template <size_t N> void HarmonicDiffusion<N>::setMaxIter(size_t n) {
    max_iter_ = n;
}

template <size_t N> void HarmonicDiffusion<N>::setEps(double e) { eps_ = e; }

template <size_t N> void HarmonicDiffusion<N>::setEpsilon(double e) {
    epsilon_ = e;
}

template <size_t N>
template <typename T>
void HarmonicDiffusion<N>::addConstraint(const Vector<N> &pos, const T &value) {
    const auto &vertices = mesh_.vertices();
    size_t node_idx = findClosestNode(pos);

    if (node_idx != size_t(-1) && !isConstrained(node_idx)) {
        if constexpr (std::is_arithmetic_v<T>) {
            values_[node_idx] = {value};
        } else {
            values_[node_idx] = std::vector<double>(value.begin(), value.end());
        }
        constrained_nodes_.push_back(node_idx);
    }
}

template <size_t N>
template <typename T>
void HarmonicDiffusion<N>::constrainBorders(const T &value) {
    const auto &border_nodes = mesh_.borderNodes();
    for (size_t idx : border_nodes) {
        if (!isConstrained(idx)) {
            if constexpr (std::is_arithmetic_v<T>) {
                values_[idx] = {value};
            } else {
                values_[idx] = std::vector<double>(value.begin(), value.end()); //value;
            }
            constrained_nodes_.push_back(idx);
        }
    }
}

template <size_t N>
Dataframe HarmonicDiffusion<N>::solve(const std::string &name, bool record,
                                      size_t step_interval) {
    double conv = 1.0;
    size_t iter = 0;
    size_t step_count = 1;

    // Store initial state if recording
    Dataframe df;
    df.add("positions", mesh_.vertices());
    df.add("triangles", mesh_.triangles());

    auto initial_values = values_;
    if (record && step_interval == 0) {
        df.add(name + "_init", createSerie(initial_values));
    }

    // Main iteration loop
    while (conv > eps_ && iter < max_iter_) {
        conv = 0.0;

        // Update non-constrained nodes
        for (size_t i = 0; i < mesh_.vertexCount(); ++i) {
            if (!isConstrained(i)) {
                const auto &neighbors = mesh_.neighbors(i);
                std::vector<double> new_value(values_[i].size(), 0.0);

                // Average neighbor values
                for (size_t n : neighbors) {
                    for (size_t j = 0; j < new_value.size(); ++j) {
                        new_value[j] += values_[n][j];
                    }
                }
                for (double &v : new_value) {
                    v /= neighbors.size();
                }

                // Relaxation
                auto old_value = values_[i];
                for (size_t j = 0; j < new_value.size(); ++j) {
                    new_value[j] = epsilon_ * new_value[j] +
                                   (1.0 - epsilon_) * old_value[j];
                }

                // Compute convergence
                double diff = 0.0;
                for (size_t j = 0; j < new_value.size(); ++j) {
                    diff += std::pow(new_value[j] - old_value[j], 2);
                }
                conv += diff;

                values_[i] = new_value;
            }
        }

        conv = std::sqrt(conv);

        // Record intermediate state if requested
        if (record && step_interval > 0 && iter % step_interval == 0) {
            df.add(name + std::to_string(step_count++), createSerie(values_));
        }

        ++iter;
    }

    // Add final result
    df.add(name, createSerie(values_));

    return df;
}

template <size_t N>
template <typename T>
void HarmonicDiffusion<N>::initializeValues(const T &init_value) {
    values_.resize(mesh_.vertexCount());
    if constexpr (std::is_arithmetic_v<T>) {
        for (auto &v : values_) {
            v = {init_value};
        }
    } else {
        for (auto &v : values_) {
            v = std::vector<double>(init_value.begin(), init_value.end());
        }
    }
}

template <size_t N>
size_t HarmonicDiffusion<N>::findClosestNode(const Vector<N> &pos) const {
    const auto &vertices = mesh_.vertices();
    size_t closest = -1;
    double min_dist = std::numeric_limits<double>::max();

    for (size_t i = 0; i < vertices.size(); ++i) {
        const auto &p = vertices[i];
        double dist = 0;
        for (size_t j = 0; j < N; ++j) {
            dist += std::pow(p[j] - pos[j], 2);
        }
        if (dist < min_dist) {
            min_dist = dist;
            closest = i;
        }
    }

    return closest;
}

template <size_t N>
bool HarmonicDiffusion<N>::isConstrained(size_t node_idx) const {
    return std::find(constrained_nodes_.begin(), constrained_nodes_.end(),
                     node_idx) != constrained_nodes_.end();
}

template <size_t N>
Serie<double> HarmonicDiffusion<N>::createSerie(
    const std::vector<std::vector<double>> &data) const {
    size_t item_size = data[0].size();
    std::vector<double> flat_data;
    flat_data.reserve(data.size() * item_size);

    for (const auto &v : data) {
        flat_data.insert(flat_data.end(), v.begin(), v.end());
    }

    return Serie<double>(flat_data);
}

} // namespace df