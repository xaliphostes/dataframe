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

#include <dataframe/utils/meta.h>

namespace df {

// ------------------ Distribution Parameters ------------------

template <typename T> struct DistParams<T, DistType::Uniform> {
    T min;
    T max;
    DistParams(T min_ = T{}, T max_ = T{1}) : min(min_), max(max_) {}
};

template <typename T>
Serie<T> random_uniform(size_t n,
                        const DistParams<T, DistType::Uniform> &params) {
    return random<T, DistType::Uniform>(n, params);
}
template <typename T> Serie<T> random_uniform(size_t n, T min, T max) {
    return random_uniform<T>(n, DistParams<T, DistType::Uniform>{min, max});
}

// ------------------------------------------

template <typename T> struct DistParams<T, DistType::Normal> {
    T mean;
    T stddev;
    DistParams(T mean_ = T{}, T stddev_ = T{1})
        : mean(mean_), stddev(stddev_) {}
};

template <typename T>
Serie<T> random_normal(size_t n,
                       const DistParams<T, DistType::Normal> &params) {
    return random<T, DistType::Normal>(n, params);
}
template <typename T> Serie<T> random_normal(size_t n, T mean, T stddev) {
    return random_normal<T>(n, DistParams<T, DistType::Normal>{mean, stddev});
}

// ------------------------------------------

template <typename T> struct DistParams<T, DistType::Bernoulli> {
    double probability;
    DistParams(double p = 0.5) : probability(p) {}
};

template <typename T>
Serie<T> random_bernoulli(size_t n,
                          const DistParams<T, DistType::Bernoulli> &params) {
    return random<T, DistType::Bernoulli>(n, params);
}
template <typename T> Serie<T> random_bernoulli(size_t n, double probability) {
    return random_bernoulli<T>(n,
                               DistParams<T, DistType::Bernoulli>{probability});
}

// ------------------------------------------

template <typename T> struct DistParams<T, DistType::Poisson> {
    double mean;
    DistParams(double mean_ = 1.0) : mean(mean_) {}
};

template <typename T>
Serie<T> random_poisson(size_t n,
                        const DistParams<T, DistType::Poisson> &params) {
    return random<T, DistType::Poisson>(n, params);
}
template <typename T> Serie<T> random_poisson(size_t n, double mean) {
    return random_poisson<T>(n, DistParams<T, DistType::Poisson>{mean});
}

// ------------------------------------------

template <typename T> struct DistParams<T, DistType::Sampling> {
    const std::vector<T> &population;
    bool replacement;
    DistParams(const std::vector<T> &pop, bool replace = true)
        : population(pop), replacement(replace) {}
};

template <typename T>
Serie<T> random_sampling(size_t n,
                         const DistParams<T, DistType::Sampling> &params) {
    return random<T, DistType::Sampling>(n, params);
}
template <typename T>
Serie<T> random_sampling(size_t n, const std::vector<T> &population,
                         bool replace) {
    return random_sampling<T>(
        n, DistParams<T, DistType::Sampling>{population, replace});
}

// ------------------------------------------

// Specializations for vector types (using std::array)
template <typename T, size_t N>
struct DistParams<std::array<T, N>, DistType::Uniform> {
    std::array<T, N> min;
    std::array<T, N> max;
    DistParams(const std::array<T, N> &min_ = std::array<T, N>{},
               const std::array<T, N> &max_ = std::array<T, N>{})
        : min(min_), max(max_) {}
};

template <typename T, size_t N>
struct DistParams<std::array<T, N>, DistType::Normal> {
    std::array<T, N> mean;
    std::array<T, N> stddev;
    DistParams(const std::array<T, N> &mean_ = std::array<T, N>{},
               const std::array<T, N> &stddev_ = std::array<T, N>{})
        : mean(mean_), stddev(stddev_) {}
};

template <typename T, size_t N>
struct DistParams<std::array<T, N>, DistType::Bernoulli> {
    std::array<double, N> probability;
    DistParams(const std::array<double, N> &prob = std::array<double, N>{})
        : probability(prob) {}
};

template <typename T, size_t N>
struct DistParams<std::array<T, N>, DistType::Poisson> {
    std::array<double, N> mean;
    DistParams(const std::array<double, N> &mean_ = std::array<double, N>{})
        : mean(mean_) {}
};

template <typename T, size_t N>
struct DistParams<std::array<T, N>, DistType::Sampling> {
    const std::vector<std::array<T, N>> &population;
    bool replacement;
    DistParams(const std::vector<std::array<T, N>> &pop, bool replace = true)
        : population(pop), replacement(replace) {}
};

// ------------------ Random Generators ------------------

namespace detail {
template <typename T, DistType D> class RandomGenerator {
  public:
    RandomGenerator()
        : engine_(std::chrono::system_clock::now().time_since_epoch().count()) {
    }

    std::vector<T> generate(size_t n, const DistParams<T, D> &params) {
        std::vector<T> result;
        result.reserve(n);

        if constexpr (D == DistType::Uniform) {
            if constexpr (std::is_arithmetic_v<T>) {
                if constexpr (std::is_integral_v<T>) {
                    std::uniform_int_distribution<T> dist(params.min,
                                                          params.max);
                    for (size_t i = 0; i < n; ++i) {
                        result.push_back(dist(engine_));
                    }
                } else {
                    std::uniform_real_distribution<T> dist(params.min,
                                                           params.max);
                    for (size_t i = 0; i < n; ++i) {
                        result.push_back(dist(engine_));
                    }
                }
            } else if constexpr (details::is_std_array_v<T>) {
                constexpr size_t N = std::tuple_size_v<T>;
                std::array<
                    std::uniform_real_distribution<typename T::value_type>, N>
                    dists;
                for (size_t i = 0; i < N; ++i) {
                    dists[i] =
                        std::uniform_real_distribution<typename T::value_type>(
                            params.min[i], params.max[i]);
                }
                for (size_t i = 0; i < n; ++i) {
                    T vec;
                    for (size_t j = 0; j < N; ++j) {
                        vec[j] = dists[j](engine_);
                    }
                    result.push_back(vec);
                }
            }
        } else if constexpr (D == DistType::Normal) {
            if constexpr (std::is_arithmetic_v<T>) {
                std::normal_distribution<T> dist(params.mean, params.stddev);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(dist(engine_));
                }
            } else if constexpr (details::is_std_array_v<T>) {
                constexpr size_t N = std::tuple_size_v<T>;
                std::array<std::normal_distribution<typename T::value_type>, N>
                    dists;
                for (size_t i = 0; i < N; ++i) {
                    dists[i] = std::normal_distribution<typename T::value_type>(
                        params.mean[i], params.stddev[i]);
                }
                for (size_t i = 0; i < n; ++i) {
                    T vec;
                    for (size_t j = 0; j < N; ++j) {
                        vec[j] = dists[j](engine_);
                    }
                    result.push_back(vec);
                }
            }
        } else if constexpr (D == DistType::Sampling) {
            if (params.replacement) {
                std::uniform_int_distribution<size_t> dist(
                    0, params.population.size() - 1);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(params.population[dist(engine_)]);
                }
            } else {
                std::vector<size_t> indices(params.population.size());
                std::iota(indices.begin(), indices.end(), 0);
                for (size_t i = 0; i < n && i < params.population.size(); ++i) {
                    std::uniform_int_distribution<size_t> dist(
                        i, indices.size() - 1);
                    size_t j = dist(engine_);
                    std::swap(indices[i], indices[j]);
                    result.push_back(params.population[indices[i]]);
                }
            }
        } else if constexpr (D == DistType::Bernoulli) {
            if constexpr (std::is_arithmetic_v<T>) {
                std::bernoulli_distribution dist(params.probability);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(static_cast<T>(dist(engine_)));
                }
            } else if constexpr (details::is_std_array_v<T>) {
                constexpr size_t N = std::tuple_size_v<T>;
                std::array<std::bernoulli_distribution, N> dists;
                for (size_t i = 0; i < N; ++i) {
                    dists[i] =
                        std::bernoulli_distribution(params.probability[i]);
                }
                for (size_t i = 0; i < n; ++i) {
                    T vec;
                    for (size_t j = 0; j < N; ++j) {
                        vec[j] = static_cast<typename T::value_type>(
                            dists[j](engine_));
                    }
                    result.push_back(vec);
                }
            }
        } else if constexpr (D == DistType::Poisson) {
            if constexpr (std::is_arithmetic_v<T>) {
                std::poisson_distribution<int> dist(params.mean);
                for (size_t i = 0; i < n; ++i) {
                    result.push_back(static_cast<T>(dist(engine_)));
                }
            } else if constexpr (details::is_std_array_v<T>) {
                constexpr size_t N = std::tuple_size_v<T>;
                std::array<std::poisson_distribution<int>, N> dists;
                for (size_t i = 0; i < N; ++i) {
                    dists[i] = std::poisson_distribution<int>(params.mean[i]);
                }
                for (size_t i = 0; i < n; ++i) {
                    T vec;
                    for (size_t j = 0; j < N; ++j) {
                        vec[j] = static_cast<typename T::value_type>(
                            dists[j](engine_));
                    }
                    result.push_back(vec);
                }
            }
        } else if constexpr (D == DistType::Sampling) {
            if constexpr (details::is_std_array_v<T>) {
                if (params.replacement) {
                    std::uniform_int_distribution<size_t> dist(
                        0, params.population.size() - 1);
                    for (size_t i = 0; i < n; ++i) {
                        result.push_back(params.population[dist(engine_)]);
                    }
                } else {
                    std::vector<size_t> indices(params.population.size());
                    std::iota(indices.begin(), indices.end(), 0);
                    for (size_t i = 0; i < n && i < params.population.size();
                         ++i) {
                        std::uniform_int_distribution<size_t> dist(
                            i, indices.size() - 1);
                        size_t j = dist(engine_);
                        std::swap(indices[i], indices[j]);
                        result.push_back(params.population[indices[i]]);
                    }
                }
            }
        }
        return result;
    }

  private:
    std::mt19937 engine_;
};

} // namespace detail

template <typename T, DistType D>
inline Serie<T> random(size_t n, const DistParams<T, D> &params) {
    detail::RandomGenerator<T, D> generator;
    return Serie<T>(generator.generate(n, params));
}

// // Scalar random generator
// template <typename T, DistType D> class RandomGenerator {
//   public:
//     RandomGenerator()
//         :
//         engine_(std::chrono::system_clock::now().time_since_epoch().count())
//         {
//     }

//     std::vector<T> generate(size_t n, const DistParams<D> &params) {
//         std::vector<T> result;
//         result.reserve(n);

//         if constexpr (D == DistType::Uniform) {
//             if constexpr (std::is_integral_v<T>) {
//                 std::uniform_int_distribution<T> dist(
//                     static_cast<T>(params.min),
//                     static_cast<T>(params.max));
//                 for (size_t i = 0; i < n; ++i) {
//                     result.push_back(dist(engine_));
//                 }
//             } else {
//                 std::uniform_real_distribution<T> dist(params.min,
//                 params.max); for (size_t i = 0; i < n; ++i) {
//                     result.push_back(dist(engine_));
//                 }
//             }
//         } else if constexpr (D == DistType::Normal) {
//             std::normal_distribution<T> dist(params.mean, params.stddev);
//             for (size_t i = 0; i < n; ++i) {
//                 result.push_back(dist(engine_));
//             }
//         } else if constexpr (D == DistType::Bernoulli) {
//             std::bernoulli_distribution dist(params.probability);
//             for (size_t i = 0; i < n; ++i) {
//                 result.push_back(static_cast<T>(dist(engine_)));
//             }
//         } else if constexpr (D == DistType::Poisson) {
//             std::poisson_distribution<int> dist(params.mean);
//             for (size_t i = 0; i < n; ++i) {
//                 result.push_back(static_cast<T>(dist(engine_)));
//             }
//         } else if constexpr (D == DistType::Sampling) {
//             if (params.replacement) {
//                 std::uniform_int_distribution<size_t> dist(
//                     0, params.population.size() - 1);
//                 for (size_t i = 0; i < n; ++i) {
//                     result.push_back(
//                         static_cast<T>(params.population[dist(engine_)]));
//                 }
//             } else {
//                 std::vector<size_t> indices(params.population.size());
//                 std::iota(indices.begin(), indices.end(), 0);
//                 for (size_t i = 0; i < n && i < params.population.size();
//                 ++i) {
//                     std::uniform_int_distribution<size_t> dist(
//                         i, indices.size() - 1);
//                     size_t j = dist(engine_);
//                     std::swap(indices[i], indices[j]);
//                     result.push_back(
//                         static_cast<T>(params.population[indices[i]]));
//                 }
//             }
//         }
//         return result;
//     }

//   private:
//     std::mt19937 engine_;
// };

// // Vector random generator
// template <size_t N, DistType D> class RandomGeneratorVector {
//   public:
//     RandomGeneratorVector()
//         :
//         engine_(std::chrono::system_clock::now().time_since_epoch().count())
//         {
//     }

//     std::vector<std::array<double, N>>
//     generate(size_t n, const DistParamsVector<N> &params) {
//         std::vector<std::array<double, N>> result;
//         result.reserve(n);

//         // Create distributions for each component
//         std::array<std::uniform_real_distribution<double>, N> dists;
//         for (size_t i = 0; i < N; ++i) {
//             dists[i] =
//             std::uniform_real_distribution<double>(params.min[i],
//                                                               params.max[i]);
//         }

//         // Generate vectors
//         for (size_t i = 0; i < n; ++i) {
//             std::array<double, N> vec;
//             for (size_t j = 0; j < N; ++j) {
//                 vec[j] = dists[j](engine_);
//             }
//             result.push_back(vec);
//         }

//         return result;
//     }

//   private:
//     std::mt19937 engine_;
// };

// } // namespace detail

// ------------------ Public Interface ------------------

// // Scalar random functions
// template <typename T> inline Serie<T> random_uniform(size_t n, T min, T
// max)
// {
//     detail::RandomGenerator<T, DistType::Uniform> generator;
//     return Serie<T>(
//         generator.generate(n, DistParams<DistType::Uniform>(min, max)));
// }

// template <typename T>
// inline Serie<T> random_normal(size_t n, T mean, T stddev) {
//     detail::RandomGenerator<T, DistType::Normal> generator;
//     return Serie<T>(
//         generator.generate(n, DistParams<DistType::Normal>(mean,
//         stddev)));
// }

// template <typename T>
// inline Serie<T> random_bernoulli(size_t n, double probability) {
//     detail::RandomGenerator<T, DistType::Bernoulli> generator;
//     return Serie<T>(
//         generator.generate(n,
//         DistParams<DistType::Bernoulli>(probability)));
// }

// template <typename T> inline Serie<T> random_poisson(size_t n, double
// mean) {
//     detail::RandomGenerator<T, DistType::Poisson> generator;
//     return Serie<T>(generator.generate(n,
//     DistParams<DistType::Poisson>(mean)));
// }

// template <typename T>
// inline Serie<T> random_sample(size_t n, const std::vector<T> &population,
//                               bool replacement) {
//     detail::RandomGenerator<T, DistType::Sampling> generator;
//     return Serie<T>(generator.generate(
//         n, DistParams<DistType::Sampling>(population, replacement)));
// }

// // Vector random functions
// template <size_t N>
// inline Serie<std::array<double, N>>
// random_uniform(size_t n, const std::array<double, N> &min,
//                const std::array<double, N> &max) {
//     detail::RandomGeneratorVector<N, DistType::Uniform> generator;
//     return Serie<std::array<double, N>>(
//         generator.generate(n, DistParamsVector<N>(min, max)));
// }

} // namespace df
