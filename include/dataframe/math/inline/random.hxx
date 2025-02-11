/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

namespace df {

template <> struct DistParams<DistType::Uniform> {
    double min;
    double max;
    DistParams(double min_ = 0.0, double max_ = 1.0) : min(min_), max(max_) {}
};

template <> struct DistParams<DistType::Normal> {
    double mean;
    double stddev;
    DistParams(double mean_ = 0.0, double stddev_ = 1.0)
        : mean(mean_), stddev(stddev_) {}
};

template <> struct DistParams<DistType::Bernoulli> {
    double probability;
    DistParams(double p = 0.5) : probability(p) {}
};

template <> struct DistParams<DistType::Poisson> {
    double mean;
    DistParams(double mean_ = 1.0) : mean(mean_) {}
};

template <> struct DistParams<DistType::Sampling> {
    const std::vector<double> &population;
    bool replacement;
    DistParams(const std::vector<double> &pop, bool replace = true)
        : population(pop), replacement(replace) {}
};

namespace detail {

template <typename T, DistType D> class RandomGenerator {
  public:
    RandomGenerator()
        : engine_(std::chrono::system_clock::now().time_since_epoch().count()) {
    }

    std::vector<T> generate(size_t n, const DistParams<D> &params) {
        std::vector<T> result;
        result.reserve(n);

        if constexpr (D == DistType::Uniform) {
            if constexpr (std::is_integral_v<T>) {
                std::uniform_int_distribution<T> dist(
                    static_cast<T>(params.min), static_cast<T>(params.max));
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(dist(engine_));
                }
            } else {
                std::uniform_real_distribution<T> dist(params.min, params.max);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(dist(engine_));
                }
            }
        } else if constexpr (D == DistType::Normal) {
            std::normal_distribution<T> dist(params.mean, params.stddev);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(dist(engine_));
            }
        } else if constexpr (D == DistType::Bernoulli) {
            std::bernoulli_distribution dist(params.probability);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(static_cast<T>(dist(engine_)));
            }
        } else if constexpr (D == DistType::Poisson) {
            std::poisson_distribution<int> dist(params.mean);
            for (size_t i = 0; i < n; ++i) {
                result.push_back(static_cast<T>(dist(engine_)));
            }
        } else if constexpr (D == DistType::Sampling) {
            if (params.replacement) {
                std::uniform_int_distribution<size_t> dist(
                    0, params.population.size() - 1);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(
                        static_cast<T>(params.population[dist(engine_)]));
                }
            } else {
                // Sampling without replacement using Fisher-Yates shuffle
                std::vector<size_t> indices(params.population.size());
                std::iota(indices.begin(), indices.end(), 0);

                for (size_t i = 0; i < n && i < params.population.size(); ++i) {
                    std::uniform_int_distribution<size_t> dist(
                        i, indices.size() - 1);
                    size_t j = dist(engine_);
                    std::swap(indices[i], indices[j]);
                    result.push_back(
                        static_cast<T>(params.population[indices[i]]));
                }
            }
        }

        return result;
    }

  private:
    std::mt19937 engine_;
};

} // namespace detail

template <typename T> inline Serie<T> random_uniform(size_t n, T min, T max) {
    detail::RandomGenerator<T, DistType::Uniform> generator;
    return Serie<T>(
        generator.generate(n, DistParams<DistType::Uniform>(min, max)));
}

template <typename T>
inline Serie<T> random_normal(size_t n, T mean, T stddev) {
    detail::RandomGenerator<T, DistType::Normal> generator;
    return Serie<T>(
        generator.generate(n, DistParams<DistType::Normal>(mean, stddev)));
}

template <typename T>
inline Serie<T> random_bernoulli(size_t n, double probability) {
    detail::RandomGenerator<T, DistType::Bernoulli> generator;
    return Serie<T>(
        generator.generate(n, DistParams<DistType::Bernoulli>(probability)));
}

template <typename T> inline Serie<T> random_poisson(size_t n, double mean) {
    detail::RandomGenerator<T, DistType::Poisson> generator;
    return Serie<T>(generator.generate(n, DistParams<DistType::Poisson>(mean)));
}

template <typename T>
inline Serie<T> random_sample(size_t n, const std::vector<T> &population,
                              bool replacement) {
    detail::RandomGenerator<T, DistType::Sampling> generator;
    return Serie<T>(generator.generate(
        n, DistParams<DistType::Sampling>(population, replacement)));
}

// Generic random function that can work with any distribution type
template <typename T, DistType D>
inline Serie<T> random(size_t n, const DistParams<D> &params) {
    detail::RandomGenerator<T, D> generator;
    return Serie<T>(generator.generate(n, params));
}

} // namespace df
