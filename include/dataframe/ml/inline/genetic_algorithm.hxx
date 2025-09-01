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
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <dataframe/math/random.h>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <unordered_set>

namespace ml {

/**
 * @brief Selection method utilities
 */
struct SelectionMethodUtils {
    static SelectionMethod fromString(const std::string &method);
    static std::string toString(SelectionMethod method);
};

/**
 * @brief Crossover method utilities
 */
struct CrossoverMethodUtils {
    static CrossoverMethod fromString(const std::string &method);
    static std::string toString(CrossoverMethod method);
};

/**
 * @brief Mutation method utilities
 */
struct MutationMethodUtils {
    static MutationMethod fromString(const std::string &method);
    static std::string toString(MutationMethod method);
};

SelectionMethod SelectionMethodUtils::fromString(const std::string &method) {
    if (method == "tournament")
        return SelectionMethod::TOURNAMENT;
    if (method == "roulette")
        return SelectionMethod::ROULETTE;
    if (method == "rank")
        return SelectionMethod::RANK;
    throw std::invalid_argument("Unknown selection method: " + method);
}

std::string SelectionMethodUtils::toString(SelectionMethod method) {
    switch (method) {
    case SelectionMethod::TOURNAMENT:
        return "tournament";
    case SelectionMethod::ROULETTE:
        return "roulette";
    case SelectionMethod::RANK:
        return "rank";
    default:
        return "unknown";
    }
}

CrossoverMethod CrossoverMethodUtils::fromString(const std::string &method) {
    if (method == "single_point")
        return CrossoverMethod::SINGLE_POINT;
    if (method == "two_point")
        return CrossoverMethod::TWO_POINT;
    if (method == "uniform")
        return CrossoverMethod::UNIFORM;
    if (method == "arithmetic")
        return CrossoverMethod::ARITHMETIC;
    if (method == "order_based")
        return CrossoverMethod::ORDER_BASED;
    throw std::invalid_argument("Unknown crossover method: " + method);
}

std::string CrossoverMethodUtils::toString(CrossoverMethod method) {
    switch (method) {
    case CrossoverMethod::SINGLE_POINT:
        return "single_point";
    case CrossoverMethod::TWO_POINT:
        return "two_point";
    case CrossoverMethod::UNIFORM:
        return "uniform";
    case CrossoverMethod::ARITHMETIC:
        return "arithmetic";
    case CrossoverMethod::ORDER_BASED:
        return "order_based";
    default:
        return "unknown";
    }
}

MutationMethod MutationMethodUtils::fromString(const std::string &method) {
    if (method == "gaussian")
        return MutationMethod::GAUSSIAN;
    if (method == "uniform")
        return MutationMethod::UNIFORM;
    if (method == "swap")
        return MutationMethod::SWAP;
    if (method == "inversion")
        return MutationMethod::INVERSION;
    if (method == "scramble")
        return MutationMethod::SCRAMBLE;
    throw std::invalid_argument("Unknown mutation method: " + method);
}

std::string MutationMethodUtils::toString(MutationMethod method) {
    switch (method) {
    case MutationMethod::GAUSSIAN:
        return "gaussian";
    case MutationMethod::UNIFORM:
        return "uniform";
    case MutationMethod::SWAP:
        return "swap";
    case MutationMethod::INVERSION:
        return "inversion";
    case MutationMethod::SCRAMBLE:
        return "scramble";
    default:
        return "unknown";
    }
}

inline GeneticAlgorithm
create_genetic_algorithm(size_t population_size, double crossover_rate,
                         double mutation_rate, size_t elite_count,
                         size_t max_generations,
                         const std::string &selection_method) {
    GeneticAlgorithm ga(population_size, crossover_rate, mutation_rate,
                        elite_count, max_generations, selection_method,
                        "single_point", "gaussian");
    return ga;
}

inline GeneticAlgorithm create_genetic_algorithm_combinatorial(
    size_t population_size, double crossover_rate, double mutation_rate,
    size_t elite_count, size_t max_generations,
    const std::string &selection_method) {
    GeneticAlgorithm ga(population_size, crossover_rate, mutation_rate,
                        elite_count, max_generations, selection_method,
                        "order_based", "swap");
    return ga;
}

// -----------------------------------------------------

// Constructor implementation
inline GeneticAlgorithm::GeneticAlgorithm(
    size_t population_size, double crossover_rate, double mutation_rate,
    size_t elite_count, size_t max_generations,
    const std::string &selection_method, const std::string &crossover_method,
    const std::string &mutation_method, size_t tournament_size, bool verbose)
    : population_size_(population_size), crossover_rate_(crossover_rate),
      mutation_rate_(mutation_rate), elite_count_(elite_count),
      max_generations_(max_generations),
      selection_method_(SelectionMethodUtils::fromString(selection_method)),
      crossover_method_(CrossoverMethodUtils::fromString(crossover_method)),
      mutation_method_(MutationMethodUtils::fromString(mutation_method)),
      tournament_size_(tournament_size), verbose_(verbose),
      rng_(std::random_device{}()) {
    // Initialize evolution history dataframe
    evolution_history_ = df::Dataframe();
    evolution_history_.add("generation", df::Serie<size_t>{});
    evolution_history_.add("best_fitness", df::Serie<double>{});
    evolution_history_.add("avg_fitness", df::Serie<double>{});
    evolution_history_.add("worst_fitness", df::Serie<double>{});
    evolution_history_.add("diversity", df::Serie<double>{});

    // Validate parameters
    if (population_size_ < 2) {
        throw std::invalid_argument("Population size must be at least 2");
    }
    if (crossover_rate_ < 0.0 || crossover_rate_ > 1.0) {
        throw std::invalid_argument(
            "Crossover rate must be between 0.0 and 1.0");
    }
    if (mutation_rate_ < 0.0 || mutation_rate_ > 1.0) {
        throw std::invalid_argument(
            "Mutation rate must be between 0.0 and 1.0");
    }
    if (elite_count_ >= population_size_) {
        throw std::invalid_argument(
            "Elite count must be less than population size");
    }
    if (tournament_size_ < 2) {
        throw std::invalid_argument("Tournament size must be at least 2");
    }
}

// Setter methods implementation
inline GeneticAlgorithm &
GeneticAlgorithm::setSelectionMethod(const std::string &method) {
    selection_method_ = SelectionMethodUtils::fromString(method);
    return *this;
}

inline GeneticAlgorithm &
GeneticAlgorithm::setSelectionMethod(SelectionMethod method) {
    selection_method_ = method;
    return *this;
}

inline GeneticAlgorithm &
GeneticAlgorithm::setCrossoverMethod(const std::string &method) {
    crossover_method_ = CrossoverMethodUtils::fromString(method);
    return *this;
}

inline GeneticAlgorithm &
GeneticAlgorithm::setCrossoverMethod(CrossoverMethod method) {
    crossover_method_ = method;
    return *this;
}

inline GeneticAlgorithm &
GeneticAlgorithm::setMutationMethod(const std::string &method) {
    mutation_method_ = MutationMethodUtils::fromString(method);
    return *this;
}

inline GeneticAlgorithm &
GeneticAlgorithm::setMutationMethod(MutationMethod method) {
    mutation_method_ = method;
    return *this;
}

inline GeneticAlgorithm &GeneticAlgorithm::setTournamentSize(size_t size) {
    if (size < 2) {
        throw std::invalid_argument("Tournament size must be at least 2");
    }
    tournament_size_ = size;
    return *this;
}

inline GeneticAlgorithm &GeneticAlgorithm::setVerbose(bool verbose) {
    verbose_ = verbose;
    return *this;
}

// Get evolution history
inline df::Dataframe GeneticAlgorithm::get_evolution_history() const {
    return evolution_history_;
}

// Get population metrics
inline std::map<std::string, double>
GeneticAlgorithm::get_population_metrics() const {
    std::map<std::string, double> metrics;

    // Extract metrics from the last generation in the evolution history
    if (evolution_history_.size() != 0 &&
        evolution_history_.get<size_t>("generation").size() > 0) {
        size_t last_idx =
            evolution_history_.get<size_t>("generation").size() - 1;
        metrics["best_fitness"] =
            evolution_history_.get<double>("best_fitness")[last_idx];
        metrics["avg_fitness"] =
            evolution_history_.get<double>("avg_fitness")[last_idx];
        metrics["worst_fitness"] =
            evolution_history_.get<double>("worst_fitness")[last_idx];
        metrics["diversity"] =
            evolution_history_.get<double>("diversity")[last_idx];
        metrics["generations"] =
            evolution_history_.get<size_t>("generation")[last_idx] + 1;
    }

    metrics["time_seconds"] = optimization_time_seconds_;

    return metrics;
}

// ------------------------------------------------------

// Set generation callback
inline GeneticAlgorithm &GeneticAlgorithm::setGenerationCallback(
    std::function<void(size_t, double, double, double)> callback) {
    generation_callback_ = callback;
    return *this;
}

// Set custom crossover function
template <typename T>
GeneticAlgorithm &GeneticAlgorithm::setCustomCrossoverFunction(
    std::function<std::pair<df::Serie<T>, df::Serie<T>>(const df::Serie<T> &,
                                                        const df::Serie<T> &)>
        crossover_func) {
    // Store the custom function in a type-erased manner
    custom_crossover_function_ =
        new std::function<std::pair<df::Serie<T>, df::Serie<T>>(
            const df::Serie<T> &, const df::Serie<T> &)>(crossover_func);
    return *this;
}

// Set custom mutation function
template <typename T>
GeneticAlgorithm &GeneticAlgorithm::setCustomMutationFunction(
    std::function<df::Serie<T>(const df::Serie<T> &)> mutation_func) {
    // Store the custom function in a type-erased manner
    custom_mutation_function_ =
        new std::function<df::Serie<T>(const df::Serie<T> &)>(mutation_func);
    return *this;
}

// Helper method to generate random individual for numerical optimization
template <typename T>
df::Serie<T>
GeneticAlgorithm::generateRandomIndividual(const df::Serie<T> &lower_bounds,
                                           const df::Serie<T> &upper_bounds) {
    std::vector<T> values;
    values.reserve(lower_bounds.size());

    for (size_t i = 0; i < lower_bounds.size(); ++i) {
        if constexpr (std::is_floating_point_v<T>) {
            // For floating-point types, generate a random number in the range
            // [lower, upper)
            std::uniform_real_distribution<T> dist(lower_bounds[i],
                                                   upper_bounds[i]);
            values.push_back(dist(rng_));
        } else if constexpr (std::is_integral_v<T>) {
            // For integral types, generate a random number in the range [lower,
            // upper]
            std::uniform_int_distribution<T> dist(lower_bounds[i],
                                                  upper_bounds[i]);
            values.push_back(dist(rng_));
        } else {
            // For other types, use a ratio approach
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            double ratio = dist(rng_);
            // Interpolate between lower and upper bound based on ratio
            // This is a simplified approach and may not work for all types
            values.push_back(
                lower_bounds[i] +
                static_cast<T>(ratio * (upper_bounds[i] - lower_bounds[i])));
        }
    }

    return df::Serie<T>(values);
}

// Helper method to generate random individual for combinatorial optimization
template <typename T>
df::Serie<T>
GeneticAlgorithm::generateRandomCombinatorial(const df::Serie<T> &candidate_set,
                                              size_t solution_length,
                                              bool allow_repetition) {
    std::vector<T> values;
    values.reserve(solution_length);

    if (allow_repetition) {
        // With repetition: simply select a random element from the candidate
        // set for each position
        std::uniform_int_distribution<size_t> dist(0, candidate_set.size() - 1);
        for (size_t i = 0; i < solution_length; ++i) {
            values.push_back(candidate_set[dist(rng_)]);
        }
    } else {
        // Without repetition: perform sampling without replacement
        if (solution_length > candidate_set.size()) {
            throw std::invalid_argument(
                "Solution length cannot exceed candidate set size when "
                "repetition is not allowed");
        }

        // Create an index vector and shuffle it
        std::vector<size_t> indices(candidate_set.size());
        std::iota(indices.begin(), indices.end(), 0);
        std::shuffle(indices.begin(), indices.end(), rng_);

        // Take the first solution_length elements
        for (size_t i = 0; i < solution_length; ++i) {
            values.push_back(candidate_set[indices[i]]);
        }
    }

    return df::Serie<T>(values);
}

// Calculate population diversity
template <typename T>
double GeneticAlgorithm::calculateDiversity(
    const std::vector<df::Serie<T>> &population) {
    if (population.empty() || population[0].size() == 0) {
        return 0.0;
    }

    // For numerical types, calculate standard deviation of each parameter
    if constexpr (std::is_arithmetic_v<T>) {
        double avg_std_dev = 0.0;

        for (size_t j = 0; j < population[0].size(); ++j) {
            // Calculate mean for this parameter
            double mean = 0.0;
            for (size_t i = 0; i < population.size(); ++i) {
                mean +=
                    population[i][j] / static_cast<double>(population.size());
            }

            // Calculate variance for this parameter
            double variance = 0.0;
            for (size_t i = 0; i < population.size(); ++i) {
                double diff = population[i][j] - mean;
                variance +=
                    (diff * diff) / static_cast<double>(population.size());
            }

            // Add std dev to average
            avg_std_dev +=
                std::sqrt(variance) / static_cast<double>(population[0].size());
        }

        return avg_std_dev;
    } else {
        // For non-numerical types, calculate a similarity index
        double similarity_sum = 0.0;
        double count = 0.0;

        for (size_t i = 0; i < population.size(); ++i) {
            for (size_t j = i + 1; j < population.size(); ++j) {
                // Count number of matching elements
                size_t matches = 0;
                for (size_t k = 0; k < population[i].size(); ++k) {
                    if (population[i][k] == population[j][k]) {
                        matches++;
                    }
                }

                // Add similarity (as percentage of matches) to sum
                similarity_sum += static_cast<double>(matches) /
                                  static_cast<double>(population[i].size());
                count += 1.0;
            }
        }

        // Convert similarity to diversity (1 - avg_similarity)
        return count > 0.0 ? 1.0 - (similarity_sum / count) : 0.0;
    }
}

// --------------------------------------------------------------------

// Tournament selection implementation
template <typename T>
size_t GeneticAlgorithm::tournamentSelection(
    const std::vector<df::Serie<T>> &population,
    const std::vector<double> &fitness_values, bool minimize) {
    std::uniform_int_distribution<size_t> dist(0, population.size() - 1);

    // Select tournament_size_ individuals randomly
    std::vector<size_t> tournament;
    tournament.reserve(tournament_size_);
    for (size_t i = 0; i < tournament_size_; ++i) {
        tournament.push_back(dist(rng_));
    }

    // Find the best individual in the tournament
    size_t best_idx = tournament[0];
    for (size_t i = 1; i < tournament.size(); ++i) {
        if (minimize) {
            if (fitness_values[tournament[i]] < fitness_values[best_idx]) {
                best_idx = tournament[i];
            }
        } else {
            if (fitness_values[tournament[i]] > fitness_values[best_idx]) {
                best_idx = tournament[i];
            }
        }
    }

    return best_idx;
}

// Roulette wheel selection implementation
template <typename T>
size_t
GeneticAlgorithm::rouletteSelection(const std::vector<df::Serie<T>> &population,
                                    const std::vector<double> &fitness_values,
                                    bool minimize) {
    // For minimization, we need to invert fitness values
    std::vector<double> selection_values;
    selection_values.reserve(fitness_values.size());

    if (minimize) {
        // Find the maximum value to subtract from
        double max_fitness =
            *std::max_element(fitness_values.begin(), fitness_values.end());

        // Transform to minimize by subtracting from max + epsilon
        for (double fitness : fitness_values) {
            selection_values.push_back(max_fitness + 1.0 - fitness);
        }
    } else {
        // For maximization, use fitness values directly
        // Add a small constant to ensure no zero probability (if all have same
        // fitness)
        double min_fitness =
            *std::min_element(fitness_values.begin(), fitness_values.end());
        double offset = min_fitness < 0 ? -min_fitness + 1.0 : 0.0;

        for (double fitness : fitness_values) {
            selection_values.push_back(fitness + offset);
        }
    }

    // Calculate sum of selection values
    double sum =
        std::accumulate(selection_values.begin(), selection_values.end(), 0.0);
    if (sum <= 0.0) {
        // If sum is zero or negative, use uniform selection instead
        std::uniform_int_distribution<size_t> dist(0, population.size() - 1);
        return dist(rng_);
    }

    // Generate a random value between 0 and sum
    std::uniform_real_distribution<double> dist(0.0, sum);
    double r = dist(rng_);

    // Select individual based on wheel position
    double cumulative = 0.0;
    for (size_t i = 0; i < selection_values.size(); ++i) {
        cumulative += selection_values[i];
        if (r <= cumulative) {
            return i;
        }
    }

    // Fallback (should never happen unless floating-point error)
    return population.size() - 1;
}

// Rank-based selection implementation
template <typename T>
size_t
GeneticAlgorithm::rankSelection(const std::vector<df::Serie<T>> &population,
                                const std::vector<double> &fitness_values,
                                bool minimize) {
    // Create index vector
    std::vector<size_t> indices(population.size());
    std::iota(indices.begin(), indices.end(), 0);

    // Sort indices by fitness
    if (minimize) {
        std::sort(indices.begin(), indices.end(),
                  [&fitness_values](size_t a, size_t b) {
                      return fitness_values[a] < fitness_values[b];
                  });
    } else {
        std::sort(indices.begin(), indices.end(),
                  [&fitness_values](size_t a, size_t b) {
                      return fitness_values[a] > fitness_values[b];
                  });
    }

    // Assign ranks (best individual gets highest rank)
    std::vector<double> ranks(population.size());
    for (size_t i = 0; i < indices.size(); ++i) {
        ranks[indices[i]] = population.size() - i;
    }

    // Select using roulette wheel on ranks
    double rank_sum = (population.size() * (population.size() + 1)) / 2.0;
    std::uniform_real_distribution<double> dist(0.0, rank_sum);
    double r = dist(rng_);

    double cumulative = 0.0;
    for (size_t i = 0; i < ranks.size(); ++i) {
        cumulative += ranks[i];
        if (r <= cumulative) {
            return i;
        }
    }

    // Fallback
    return population.size() - 1;
}

// ----------------------------------------------------------------

// Single-point crossover implementation
template <typename T>
std::pair<df::Serie<T>, df::Serie<T>>
GeneticAlgorithm::singlePointCrossover(const df::Serie<T> &parent1,
                                       const df::Serie<T> &parent2) {
    if (parent1.size() != parent2.size() || parent1.size() < 2) {
        throw std::invalid_argument(
            "Parents must have the same size and at least 2 elements");
    }

    // Select random crossover point
    std::uniform_int_distribution<size_t> dist(1, parent1.size() - 1);
    size_t crossover_point = dist(rng_);

    // Create child vectors
    std::vector<T> child1_values;
    std::vector<T> child2_values;
    child1_values.reserve(parent1.size());
    child2_values.reserve(parent1.size());

    // Perform crossover
    for (size_t i = 0; i < parent1.size(); ++i) {
        if (i < crossover_point) {
            child1_values.push_back(parent1[i]);
            child2_values.push_back(parent2[i]);
        } else {
            child1_values.push_back(parent2[i]);
            child2_values.push_back(parent1[i]);
        }
    }

    return std::make_pair(df::Serie<T>(child1_values),
                          df::Serie<T>(child2_values));
}

// Two-point crossover implementation
template <typename T>
std::pair<df::Serie<T>, df::Serie<T>>
GeneticAlgorithm::twoPointCrossover(const df::Serie<T> &parent1,
                                    const df::Serie<T> &parent2) {
    if (parent1.size() != parent2.size() || parent1.size() < 3) {
        throw std::invalid_argument(
            "Parents must have the same size and at least 3 elements");
    }

    // Select two random crossover points
    std::uniform_int_distribution<size_t> dist(1, parent1.size() - 2);
    size_t point1 = dist(rng_);
    size_t point2 = dist(rng_);

    // Ensure point1 < point2
    if (point1 > point2) {
        std::swap(point1, point2);
    }

    // Create child vectors
    std::vector<T> child1_values;
    std::vector<T> child2_values;
    child1_values.reserve(parent1.size());
    child2_values.reserve(parent1.size());

    // Perform crossover
    for (size_t i = 0; i < parent1.size(); ++i) {
        if (i < point1 || i >= point2) {
            child1_values.push_back(parent1[i]);
            child2_values.push_back(parent2[i]);
        } else {
            child1_values.push_back(parent2[i]);
            child2_values.push_back(parent1[i]);
        }
    }

    return std::make_pair(df::Serie<T>(child1_values),
                          df::Serie<T>(child2_values));
}

// Uniform crossover implementation
template <typename T>
std::pair<df::Serie<T>, df::Serie<T>>
GeneticAlgorithm::uniformCrossover(const df::Serie<T> &parent1,
                                   const df::Serie<T> &parent2) {
    if (parent1.size() != parent2.size()) {
        throw std::invalid_argument("Parents must have the same size");
    }

    // Create child vectors
    std::vector<T> child1_values;
    std::vector<T> child2_values;
    child1_values.reserve(parent1.size());
    child2_values.reserve(parent1.size());

    // Random distribution for swap decision
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    // Perform crossover
    for (size_t i = 0; i < parent1.size(); ++i) {
        if (dist(rng_) < 0.5) {
            child1_values.push_back(parent1[i]);
            child2_values.push_back(parent2[i]);
        } else {
            child1_values.push_back(parent2[i]);
            child2_values.push_back(parent1[i]);
        }
    }

    return std::make_pair(df::Serie<T>(child1_values),
                          df::Serie<T>(child2_values));
}

// Arithmetic crossover implementation
template <typename T>
std::pair<df::Serie<T>, df::Serie<T>>
GeneticAlgorithm::arithmeticCrossover(const df::Serie<T> &parent1,
                                      const df::Serie<T> &parent2) {
    if (parent1.size() != parent2.size()) {
        throw std::invalid_argument("Parents must have the same size");
    }

    // Create child vectors
    std::vector<T> child1_values;
    std::vector<T> child2_values;
    child1_values.reserve(parent1.size());
    child2_values.reserve(parent1.size());

    // Generate random alpha value for arithmetic crossover
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double alpha = dist(rng_);

    // Perform arithmetic crossover
    for (size_t i = 0; i < parent1.size(); ++i) {
        // For numeric types, perform linear interpolation between parents
        if constexpr (std::is_arithmetic_v<T>) {
            child1_values.push_back(static_cast<T>(alpha * parent1[i] +
                                                   (1.0 - alpha) * parent2[i]));
            child2_values.push_back(static_cast<T>((1.0 - alpha) * parent1[i] +
                                                   alpha * parent2[i]));
        } else {
            // For non-numeric types, resort to uniform crossover
            if (dist(rng_) < 0.5) {
                child1_values.push_back(parent1[i]);
                child2_values.push_back(parent2[i]);
            } else {
                child1_values.push_back(parent2[i]);
                child2_values.push_back(parent1[i]);
            }
        }
    }

    return std::make_pair(df::Serie<T>(child1_values),
                          df::Serie<T>(child2_values));
}

// --------------------------------------------------------------------------

// Order-based crossover implementation (for permutation problems)
template <typename T>
std::pair<df::Serie<T>, df::Serie<T>>
GeneticAlgorithm::orderBasedCrossover(const df::Serie<T> &parent1,
                                      const df::Serie<T> &parent2) {
    if (parent1.size() != parent2.size() || parent1.size() < 2) {
        throw std::invalid_argument(
            "Parents must have the same size and at least 2 elements");
    }

    // Check if the parents are permutations (contain the same elements)
    std::unordered_multiset<T> p1_elements(parent1.data().begin(),
                                           parent1.data().end());
    std::unordered_multiset<T> p2_elements(parent2.data().begin(),
                                           parent2.data().end());

    if (p1_elements != p2_elements) {
        // Fall back to another crossover method if not permutations
        return uniformCrossover(parent1, parent2);
    }

    // Select a random subset of positions
    std::vector<bool> position_mask(parent1.size(), false);
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    for (size_t i = 0; i < parent1.size(); ++i) {
        if (dist(rng_) < 0.5) {
            position_mask[i] = true;
        }
    }

    // Create child vectors
    std::vector<T> child1_values(parent1.size());
    std::vector<T> child2_values(parent1.size());

    // Copy the selected positions from the parents
    std::vector<bool> used1(parent1.size(), false);
    std::vector<bool> used2(parent1.size(), false);

    for (size_t i = 0; i < parent1.size(); ++i) {
        if (position_mask[i]) {
            child1_values[i] = parent1[i];
            child2_values[i] = parent2[i];

            // Mark these values as used
            for (size_t j = 0; j < parent1.size(); ++j) {
                if (parent2[j] == parent1[i]) {
                    used1[j] = true;
                }
                if (parent1[j] == parent2[i]) {
                    used2[j] = true;
                }
            }
        }
    }

    // Fill the remaining positions
    size_t idx1 = 0;
    size_t idx2 = 0;

    for (size_t i = 0; i < parent1.size(); ++i) {
        if (!position_mask[i]) {
            // Find the next unused element in the opposite parent
            while (idx1 < parent1.size() && used1[idx1]) {
                idx1++;
            }
            while (idx2 < parent1.size() && used2[idx2]) {
                idx2++;
            }

            // Fill in child1 from parent2
            if (idx1 < parent1.size()) {
                child1_values[i] = parent2[idx1];
                used1[idx1] = true;
            }

            // Fill in child2 from parent1
            if (idx2 < parent1.size()) {
                child2_values[i] = parent1[idx2];
                used2[idx2] = true;
            }
        }
    }

    return std::make_pair(df::Serie<T>(child1_values),
                          df::Serie<T>(child2_values));
}

// Swap mutation implementation (for permutation problems)
template <typename T>
df::Serie<T> GeneticAlgorithm::swapMutation(const df::Serie<T> &individual) {
    if (individual.size() < 2) {
        return individual;
    }

    std::vector<T> mutated_values(individual.data());

    // Check if mutation should be applied
    std::uniform_real_distribution<double> should_mutate(0.0, 1.0);
    if (should_mutate(rng_) < mutation_rate_) {
        // Select two random positions to swap
        std::uniform_int_distribution<size_t> dist(0, individual.size() - 1);
        size_t pos1 = dist(rng_);
        size_t pos2 = dist(rng_);

        // Ensure the positions are different
        while (pos1 == pos2) {
            pos2 = dist(rng_);
        }

        // Swap elements
        std::swap(mutated_values[pos1], mutated_values[pos2]);
    }

    return df::Serie<T>(mutated_values);
}

// Inversion mutation implementation (for permutation problems)
template <typename T>
df::Serie<T>
GeneticAlgorithm::inversionMutation(const df::Serie<T> &individual) {
    if (individual.size() < 2) {
        return individual;
    }

    std::vector<T> mutated_values(individual.data());

    // Check if mutation should be applied
    std::uniform_real_distribution<double> should_mutate(0.0, 1.0);
    if (should_mutate(rng_) < mutation_rate_) {
        // Select two random positions
        std::uniform_int_distribution<size_t> dist(0, individual.size() - 1);
        size_t pos1 = dist(rng_);
        size_t pos2 = dist(rng_);

        // Ensure the positions are different
        while (pos1 == pos2) {
            pos2 = dist(rng_);
        }

        // Make sure pos1 < pos2
        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        // Reverse the segment between pos1 and pos2
        std::reverse(mutated_values.begin() + pos1,
                     mutated_values.begin() + pos2 + 1);
    }

    return df::Serie<T>(mutated_values);
}

// Scramble mutation implementation (for permutation problems)
template <typename T>
df::Serie<T>
GeneticAlgorithm::scrambleMutation(const df::Serie<T> &individual) {
    if (individual.size() < 2) {
        return individual;
    }

    std::vector<T> mutated_values(individual.data());

    // Check if mutation should be applied
    std::uniform_real_distribution<double> should_mutate(0.0, 1.0);
    if (should_mutate(rng_) < mutation_rate_) {
        // Select two random positions
        std::uniform_int_distribution<size_t> dist(0, individual.size() - 1);
        size_t pos1 = dist(rng_);
        size_t pos2 = dist(rng_);

        // Ensure the positions are different
        while (pos1 == pos2) {
            pos2 = dist(rng_);
        }

        // Make sure pos1 < pos2
        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }

        // Scramble the segment between pos1 and pos2
        std::shuffle(mutated_values.begin() + pos1,
                     mutated_values.begin() + pos2 + 1, rng_);
    }

    return df::Serie<T>(mutated_values);
}

// ------------------------------------------------------------

// Gaussian mutation implementation
template <typename T>
df::Serie<T>
GeneticAlgorithm::gaussianMutation(const df::Serie<T> &individual,
                                   const df::Serie<T> &lower_bounds,
                                   const df::Serie<T> &upper_bounds) {
    std::vector<T> mutated_values;
    mutated_values.reserve(individual.size());

    // Standard deviation is 10% of the parameter range by default
    std::vector<double> std_devs;
    std_devs.reserve(individual.size());

    for (size_t i = 0; i < individual.size(); ++i) {
        double range = upper_bounds[i] - lower_bounds[i];
        std_devs.push_back(0.1 * range);
    }

    // Apply Gaussian mutation
    for (size_t i = 0; i < individual.size(); ++i) {
        // Check if this gene should be mutated based on mutation rate
        std::uniform_real_distribution<double> should_mutate(0.0, 1.0);
        if (should_mutate(rng_) < mutation_rate_) {
            if constexpr (std::is_floating_point_v<T>) {
                // For floating-point types, add Gaussian noise
                std::normal_distribution<double> gauss(0.0, std_devs[i]);
                double mutated = individual[i] + gauss(rng_);

                // Clamp to bounds
                mutated =
                    std::clamp(mutated, static_cast<double>(lower_bounds[i]),
                               static_cast<double>(upper_bounds[i]));
                mutated_values.push_back(static_cast<T>(mutated));
            } else if constexpr (std::is_integral_v<T>) {
                // For integral types, discretize the Gaussian noise
                std::normal_distribution<double> gauss(0.0, std_devs[i]);
                double noise = gauss(rng_);
                T mutated = individual[i] + static_cast<T>(std::round(noise));

                // Clamp to bounds
                mutated = std::clamp(mutated, lower_bounds[i], upper_bounds[i]);
                mutated_values.push_back(mutated);
            } else {
                // For other types, don't mutate
                mutated_values.push_back(individual[i]);
            }
        } else {
            // No mutation
            mutated_values.push_back(individual[i]);
        }
    }

    return df::Serie<T>(mutated_values);
}

// Uniform mutation implementation
template <typename T>
df::Serie<T>
GeneticAlgorithm::uniformMutation(const df::Serie<T> &individual,
                                  const df::Serie<T> &lower_bounds,
                                  const df::Serie<T> &upper_bounds) {
    std::vector<T> mutated_values;
    mutated_values.reserve(individual.size());

    for (size_t i = 0; i < individual.size(); ++i) {
        // Check if this gene should be mutated based on mutation rate
        std::uniform_real_distribution<double> should_mutate(0.0, 1.0);
        if (should_mutate(rng_) < mutation_rate_) {
            if constexpr (std::is_floating_point_v<T>) {
                // For floating-point types, generate a new random value
                std::uniform_real_distribution<T> dist(lower_bounds[i],
                                                       upper_bounds[i]);
                mutated_values.push_back(dist(rng_));
            } else if constexpr (std::is_integral_v<T>) {
                // For integral types, generate a new random value
                std::uniform_int_distribution<T> dist(lower_bounds[i],
                                                      upper_bounds[i]);
                mutated_values.push_back(dist(rng_));
            } else {
                // For other types, don't mutate
                mutated_values.push_back(individual[i]);
            }
        } else {
            // No mutation
            mutated_values.push_back(individual[i]);
        }
    }

    return df::Serie<T>(mutated_values);
}

// -----------------------------------------------------------

// Main optimization method for numerical problems
template <typename T>
std::pair<df::Serie<T>, double> GeneticAlgorithm::optimize(
    std::function<double(const df::Serie<T> &)> fitness_function,
    const df::Serie<T> &lower_bounds, const df::Serie<T> &upper_bounds,
    bool minimize) {
    if (lower_bounds.size() != upper_bounds.size()) {
        throw std::invalid_argument(
            "Lower bounds and upper bounds must have the same size");
    }

    // Start a timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Initialize evolution history
    evolution_history_ = df::Dataframe();
    evolution_history_.add("generation", df::Serie<size_t>{});
    evolution_history_.add("best_fitness", df::Serie<double>{});
    evolution_history_.add("avg_fitness", df::Serie<double>{});
    evolution_history_.add("worst_fitness", df::Serie<double>{});
    evolution_history_.add("diversity", df::Serie<double>{});

    // Initialize population
    std::vector<df::Serie<T>> population;
    population.reserve(population_size_);

    for (size_t i = 0; i < population_size_; ++i) {
        population.push_back(
            generateRandomIndividual(lower_bounds, upper_bounds));
    }

    // Evaluate initial population
    std::vector<double> fitness_values;
    fitness_values.reserve(population_size_);

    for (const auto &individual : population) {
        fitness_values.push_back(fitness_function(individual));
    }

    // Find best individual
    size_t best_idx = 0;
    double best_fitness = fitness_values[0];
    double worst_fitness = fitness_values[0];
    double sum_fitness = 0.0;

    for (size_t i = 0; i < population_size_; ++i) {
        sum_fitness += fitness_values[i];

        if ((minimize && fitness_values[i] < best_fitness) ||
            (!minimize && fitness_values[i] > best_fitness)) {
            best_fitness = fitness_values[i];
            best_idx = i;
        }

        if ((minimize && fitness_values[i] > worst_fitness) ||
            (!minimize && fitness_values[i] < worst_fitness)) {
            worst_fitness = fitness_values[i];
        }
    }

    // Add initial population stats to history
    double avg_fitness = sum_fitness / static_cast<double>(population_size_);
    double diversity = calculateDiversity(population);

    evolution_history_.get<size_t>("generation").add(0);
    evolution_history_.get<double>("best_fitness").add(best_fitness);
    evolution_history_.get<double>("avg_fitness").add(avg_fitness);
    evolution_history_.get<double>("worst_fitness").add(worst_fitness);
    evolution_history_.get<double>("diversity").add(diversity);

    // Call generation callback if provided
    if (generation_callback_) {
        generation_callback_(0, best_fitness, avg_fitness, diversity);
    }

    // Verbose output
    if (verbose_) {
        std::cout << "Generation 0: Best fitness = " << best_fitness
                  << ", Avg fitness = " << avg_fitness
                  << ", Diversity = " << diversity << std::endl;
    }

    // Main evolution loop
    for (size_t generation = 0; generation < max_generations_; ++generation) {
        // Create new population
        std::vector<df::Serie<T>> new_population;
        new_population.reserve(population_size_);

        // Elitism: keep the best individuals
        std::vector<size_t> sorted_indices(population_size_);
        std::iota(sorted_indices.begin(), sorted_indices.end(), 0);

        if (minimize) {
            std::sort(sorted_indices.begin(), sorted_indices.end(),
                      [&fitness_values](size_t a, size_t b) {
                          return fitness_values[a] < fitness_values[b];
                      });
        } else {
            std::sort(sorted_indices.begin(), sorted_indices.end(),
                      [&fitness_values](size_t a, size_t b) {
                          return fitness_values[a] > fitness_values[b];
                      });
        }

        // Add elite individuals to new population
        for (size_t i = 0; i < elite_count_; ++i) {
            new_population.push_back(population[sorted_indices[i]]);
        }

        // Fill the rest of the population with offspring
        while (new_population.size() < population_size_) {
            // Select parents
            size_t parent1_idx, parent2_idx;

            switch (selection_method_) {
            case SelectionMethod::TOURNAMENT:
                parent1_idx =
                    tournamentSelection(population, fitness_values, minimize);
                parent2_idx =
                    tournamentSelection(population, fitness_values, minimize);
                break;
            case SelectionMethod::ROULETTE:
                parent1_idx =
                    rouletteSelection(population, fitness_values, minimize);
                parent2_idx =
                    rouletteSelection(population, fitness_values, minimize);
                break;
            case SelectionMethod::RANK:
                parent1_idx =
                    rankSelection(population, fitness_values, minimize);
                parent2_idx =
                    rankSelection(population, fitness_values, minimize);
                break;
            }

            // Crossover
            std::pair<df::Serie<T>, df::Serie<T>> children;
            std::uniform_real_distribution<double> crossover_dist(0.0, 1.0);

            if (crossover_dist(rng_) < crossover_rate_) {
                // Apply custom crossover if provided
                if (custom_crossover_function_ != nullptr) {
                    auto crossover_func = static_cast<
                        std::function<std::pair<df::Serie<T>, df::Serie<T>>(
                            const df::Serie<T> &, const df::Serie<T> &)> *>(
                        custom_crossover_function_);
                    children = (*crossover_func)(population[parent1_idx],
                                                 population[parent2_idx]);
                } else {
                    // Apply selected crossover method
                    switch (crossover_method_) {
                    case CrossoverMethod::SINGLE_POINT:
                        children = singlePointCrossover(
                            population[parent1_idx], population[parent2_idx]);
                        break;
                    case CrossoverMethod::TWO_POINT:
                        children = twoPointCrossover(population[parent1_idx],
                                                     population[parent2_idx]);
                        break;
                    case CrossoverMethod::UNIFORM:
                        children = uniformCrossover(population[parent1_idx],
                                                    population[parent2_idx]);
                        break;
                    case CrossoverMethod::ARITHMETIC:
                        children = arithmeticCrossover(population[parent1_idx],
                                                       population[parent2_idx]);
                        break;
                    case CrossoverMethod::ORDER_BASED:
                        children = orderBasedCrossover(population[parent1_idx],
                                                       population[parent2_idx]);
                        break;
                    }
                }
            } else {
                // No crossover, just copy parents
                children = std::make_pair(population[parent1_idx],
                                          population[parent2_idx]);
            }

            // Mutation
            df::Serie<T> child1, child2;

            // Apply custom mutation if provided
            if (custom_mutation_function_ != nullptr) {
                auto mutation_func = static_cast<
                    std::function<df::Serie<T>(const df::Serie<T> &)> *>(
                    custom_mutation_function_);
                child1 = (*mutation_func)(children.first);
                child2 = (*mutation_func)(children.second);
            } else {
                // Apply selected mutation method
                switch (mutation_method_) {
                case MutationMethod::GAUSSIAN:
                    child1 = gaussianMutation(children.first, lower_bounds,
                                              upper_bounds);
                    child2 = gaussianMutation(children.second, lower_bounds,
                                              upper_bounds);
                    break;
                case MutationMethod::UNIFORM:
                    child1 = uniformMutation(children.first, lower_bounds,
                                             upper_bounds);
                    child2 = uniformMutation(children.second, lower_bounds,
                                             upper_bounds);
                    break;
                case MutationMethod::SWAP:
                    child1 = swapMutation(children.first);
                    child2 = swapMutation(children.second);
                    break;
                case MutationMethod::INVERSION:
                    child1 = inversionMutation(children.first);
                    child2 = inversionMutation(children.second);
                    break;
                case MutationMethod::SCRAMBLE:
                    child1 = scrambleMutation(children.first);
                    child2 = scrambleMutation(children.second);
                    break;
                }
            }

            // Add children to new population
            new_population.push_back(child1);
            if (new_population.size() < population_size_) {
                new_population.push_back(child2);
            }
        }

        // Replace old population
        population = std::move(new_population);

        // Evaluate new population
        fitness_values.clear();
        fitness_values.reserve(population_size_);

        for (const auto &individual : population) {
            fitness_values.push_back(fitness_function(individual));
        }

        // Find best individual
        best_idx = 0;
        best_fitness = fitness_values[0];
        worst_fitness = fitness_values[0];
        sum_fitness = 0.0;

        for (size_t i = 0; i < population_size_; ++i) {
            sum_fitness += fitness_values[i];

            if ((minimize && fitness_values[i] < best_fitness) ||
                (!minimize && fitness_values[i] > best_fitness)) {
                best_fitness = fitness_values[i];
                best_idx = i;
            }

            if ((minimize && fitness_values[i] > worst_fitness) ||
                (!minimize && fitness_values[i] < worst_fitness)) {
                worst_fitness = fitness_values[i];
            }
        }

        // Add generation stats to history
        avg_fitness = sum_fitness / static_cast<double>(population_size_);
        diversity = calculateDiversity(population);

        evolution_history_.get<size_t>("generation").add(generation + 1);
        evolution_history_.get<double>("best_fitness").add(best_fitness);
        evolution_history_.get<double>("avg_fitness").add(avg_fitness);
        evolution_history_.get<double>("worst_fitness").add(worst_fitness);
        evolution_history_.get<double>("diversity").add(diversity);

        // Call generation callback if provided
        if (generation_callback_) {
            generation_callback_(generation + 1, best_fitness, avg_fitness,
                                 diversity);
        }

        // Verbose output
        if (verbose_) {
            std::cout << "Generation " << (generation + 1)
                      << ": Best fitness = " << best_fitness
                      << ", Avg fitness = " << avg_fitness
                      << ", Diversity = " << diversity << std::endl;
        }

        // Check for early convergence if diversity is too low
        if (diversity < 1e-6) {
            if (verbose_) {
                std::cout << "Early stopping due to low diversity."
                          << std::endl;
            }
            break;
        }
    }

    // Stop the timer
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    optimization_time_seconds_ = elapsed.count();

    if (verbose_) {
        std::cout << "Optimization completed in " << optimization_time_seconds_
                  << " seconds." << std::endl;
    }

    // Return the best individual and its fitness
    return std::make_pair(population[best_idx], best_fitness);
}

// ------------------------------------------------------

// Main optimization method for combinatorial problems
template <typename T>
std::pair<df::Serie<T>, double> GeneticAlgorithm::optimize_combinatorial(
    std::function<double(const df::Serie<T> &)> fitness_function,
    const df::Serie<T> &candidate_set, size_t solution_length,
    bool allow_repetition, bool minimize) {
    if (candidate_set.empty()) {
        throw std::invalid_argument("Candidate set cannot be empty");
    }

    if (!allow_repetition && solution_length > candidate_set.size()) {
        throw std::invalid_argument("Solution length cannot exceed candidate "
                                    "set size when repetition is not allowed");
    }

    // Start a timer
    auto start_time = std::chrono::high_resolution_clock::now();

    // Initialize evolution history
    evolution_history_ = df::Dataframe();
    evolution_history_.add("generation", df::Serie<size_t>{});
    evolution_history_.add("best_fitness", df::Serie<double>{});
    evolution_history_.add("avg_fitness", df::Serie<double>{});
    evolution_history_.add("worst_fitness", df::Serie<double>{});
    evolution_history_.add("diversity", df::Serie<double>{});

    // Set appropriate mutation method for combinatorial problems if not
    // explicitly set
    if (mutation_method_ == MutationMethod::GAUSSIAN ||
        mutation_method_ == MutationMethod::UNIFORM) {
        mutation_method_ = MutationMethod::SWAP;
        if (verbose_) {
            std::cout << "Automatically switched to swap mutation for "
                         "combinatorial problem"
                      << std::endl;
        }
    }

    // Set appropriate crossover method for combinatorial problems if not
    // explicitly set
    if (crossover_method_ == CrossoverMethod::ARITHMETIC) {
        crossover_method_ = CrossoverMethod::ORDER_BASED;
        if (verbose_) {
            std::cout << "Automatically switched to order-based crossover for "
                         "combinatorial problem"
                      << std::endl;
        }
    }

    // Initialize population
    std::vector<df::Serie<T>> population;
    population.reserve(population_size_);

    for (size_t i = 0; i < population_size_; ++i) {
        population.push_back(generateRandomCombinatorial(
            candidate_set, solution_length, allow_repetition));
    }

    // Evaluate initial population
    std::vector<double> fitness_values;
    fitness_values.reserve(population_size_);

    for (const auto &individual : population) {
        fitness_values.push_back(fitness_function(individual));
    }

    // Find best individual
    size_t best_idx = 0;
    double best_fitness = fitness_values[0];
    double worst_fitness = fitness_values[0];
    double sum_fitness = 0.0;

    for (size_t i = 0; i < population_size_; ++i) {
        sum_fitness += fitness_values[i];

        if ((minimize && fitness_values[i] < best_fitness) ||
            (!minimize && fitness_values[i] > best_fitness)) {
            best_fitness = fitness_values[i];
            best_idx = i;
        }

        if ((minimize && fitness_values[i] > worst_fitness) ||
            (!minimize && fitness_values[i] < worst_fitness)) {
            worst_fitness = fitness_values[i];
        }
    }

    // Add initial population stats to history
    double avg_fitness = sum_fitness / static_cast<double>(population_size_);
    double diversity = calculateDiversity(population);

    evolution_history_.get<size_t>("generation").add(0);
    evolution_history_.get<double>("best_fitness").add(best_fitness);
    evolution_history_.get<double>("avg_fitness").add(avg_fitness);
    evolution_history_.get<double>("worst_fitness").add(worst_fitness);
    evolution_history_.get<double>("diversity").add(diversity);

    // Call generation callback if provided
    if (generation_callback_) {
        generation_callback_(0, best_fitness, avg_fitness, diversity);
    }

    // Verbose output
    if (verbose_) {
        std::cout << "Generation 0: Best fitness = " << best_fitness
                  << ", Avg fitness = " << avg_fitness
                  << ", Diversity = " << diversity << std::endl;
    }

    // Create dummy bounds for mutation functions that require them
    // (not used by combinatorial mutation methods but required by function
    // signature)
    df::Serie<T> dummy_lower_bounds(solution_length);
    df::Serie<T> dummy_upper_bounds(solution_length);

    // Main evolution loop
    for (size_t generation = 0; generation < max_generations_; ++generation) {
        // Create new population
        std::vector<df::Serie<T>> new_population;
        new_population.reserve(population_size_);

        // Elitism: keep the best individuals
        std::vector<size_t> sorted_indices(population_size_);
        std::iota(sorted_indices.begin(), sorted_indices.end(), 0);

        if (minimize) {
            std::sort(sorted_indices.begin(), sorted_indices.end(),
                      [&fitness_values](size_t a, size_t b) {
                          return fitness_values[a] < fitness_values[b];
                      });
        } else {
            std::sort(sorted_indices.begin(), sorted_indices.end(),
                      [&fitness_values](size_t a, size_t b) {
                          return fitness_values[a] > fitness_values[b];
                      });
        }

        // Add elite individuals to new population
        for (size_t i = 0; i < elite_count_; ++i) {
            new_population.push_back(population[sorted_indices[i]]);
        }

        // Fill the rest of the population with offspring
        while (new_population.size() < population_size_) {
            // Select parents
            size_t parent1_idx, parent2_idx;

            switch (selection_method_) {
            case SelectionMethod::TOURNAMENT:
                parent1_idx =
                    tournamentSelection(population, fitness_values, minimize);
                parent2_idx =
                    tournamentSelection(population, fitness_values, minimize);
                break;
            case SelectionMethod::ROULETTE:
                parent1_idx =
                    rouletteSelection(population, fitness_values, minimize);
                parent2_idx =
                    rouletteSelection(population, fitness_values, minimize);
                break;
            case SelectionMethod::RANK:
                parent1_idx =
                    rankSelection(population, fitness_values, minimize);
                parent2_idx =
                    rankSelection(population, fitness_values, minimize);
                break;
            }

            // Crossover
            std::pair<df::Serie<T>, df::Serie<T>> children;
            std::uniform_real_distribution<double> crossover_dist(0.0, 1.0);

            if (crossover_dist(rng_) < crossover_rate_) {
                // Apply custom crossover if provided
                if (custom_crossover_function_ != nullptr) {
                    auto crossover_func = static_cast<
                        std::function<std::pair<df::Serie<T>, df::Serie<T>>(
                            const df::Serie<T> &, const df::Serie<T> &)> *>(
                        custom_crossover_function_);
                    children = (*crossover_func)(population[parent1_idx],
                                                 population[parent2_idx]);
                } else {
                    // Apply selected crossover method
                    switch (crossover_method_) {
                    case CrossoverMethod::SINGLE_POINT:
                        children = singlePointCrossover(
                            population[parent1_idx], population[parent2_idx]);
                        break;
                    case CrossoverMethod::TWO_POINT:
                        children = twoPointCrossover(population[parent1_idx],
                                                     population[parent2_idx]);
                        break;
                    case CrossoverMethod::UNIFORM:
                        children = uniformCrossover(population[parent1_idx],
                                                    population[parent2_idx]);
                        break;
                    case CrossoverMethod::ARITHMETIC:
                        children = arithmeticCrossover(population[parent1_idx],
                                                       population[parent2_idx]);
                        break;
                    case CrossoverMethod::ORDER_BASED:
                        children = orderBasedCrossover(population[parent1_idx],
                                                       population[parent2_idx]);
                        break;
                    }
                }
            } else {
                // No crossover, just copy parents
                children = std::make_pair(population[parent1_idx],
                                          population[parent2_idx]);
            }

            // Mutation
            df::Serie<T> child1, child2;

            // Apply custom mutation if provided
            if (custom_mutation_function_ != nullptr) {
                auto mutation_func = static_cast<
                    std::function<df::Serie<T>(const df::Serie<T> &)> *>(
                    custom_mutation_function_);
                child1 = (*mutation_func)(children.first);
                child2 = (*mutation_func)(children.second);
            } else {
                // Apply selected mutation method
                switch (mutation_method_) {
                case MutationMethod::GAUSSIAN:
                    child1 = gaussianMutation(
                        children.first, dummy_lower_bounds, dummy_upper_bounds);
                    child2 =
                        gaussianMutation(children.second, dummy_lower_bounds,
                                         dummy_upper_bounds);
                    break;
                case MutationMethod::UNIFORM:
                    child1 = uniformMutation(children.first, dummy_lower_bounds,
                                             dummy_upper_bounds);
                    child2 =
                        uniformMutation(children.second, dummy_lower_bounds,
                                        dummy_upper_bounds);
                    break;
                case MutationMethod::SWAP:
                    child1 = swapMutation(children.first);
                    child2 = swapMutation(children.second);
                    break;
                case MutationMethod::INVERSION:
                    child1 = inversionMutation(children.first);
                    child2 = inversionMutation(children.second);
                    break;
                case MutationMethod::SCRAMBLE:
                    child1 = scrambleMutation(children.first);
                    child2 = scrambleMutation(children.second);
                    break;
                }
            }

            // Ensure no duplicates if repetition is not allowed
            if (!allow_repetition) {
                // Helper function to check and fix duplicates
                auto fix_duplicates = [&candidate_set](
                                          df::Serie<T> &individual) {
                    std::vector<T> values(individual.data());
                    std::unordered_set<T> seen;

                    for (size_t i = 0; i < values.size(); ++i) {
                        if (seen.count(values[i]) > 0) {
                            // Found a duplicate, replace with an unused element
                            for (size_t j = 0; j < candidate_set.size(); ++j) {
                                if (seen.count(candidate_set[j]) == 0) {
                                    values[i] = candidate_set[j];
                                    seen.insert(candidate_set[j]);
                                    break;
                                }
                            }
                        } else {
                            seen.insert(values[i]);
                        }
                    }

                    return df::Serie<T>(values);
                };

                child1 = fix_duplicates(child1);
                child2 = fix_duplicates(child2);
            }

            // Add children to new population
            new_population.push_back(child1);
            if (new_population.size() < population_size_) {
                new_population.push_back(child2);
            }
        }

        // Replace old population
        population = std::move(new_population);

        // Evaluate new population
        fitness_values.clear();
        fitness_values.reserve(population_size_);

        for (const auto &individual : population) {
            fitness_values.push_back(fitness_function(individual));
        }

        // Find best individual
        best_idx = 0;
        best_fitness = fitness_values[0];
        worst_fitness = fitness_values[0];
        sum_fitness = 0.0;

        for (size_t i = 0; i < population_size_; ++i) {
            sum_fitness += fitness_values[i];

            if ((minimize && fitness_values[i] < best_fitness) ||
                (!minimize && fitness_values[i] > best_fitness)) {
                best_fitness = fitness_values[i];
                best_idx = i;
            }

            if ((minimize && fitness_values[i] > worst_fitness) ||
                (!minimize && fitness_values[i] < worst_fitness)) {
                worst_fitness = fitness_values[i];
            }
        }

        // Add generation stats to history
        avg_fitness = sum_fitness / static_cast<double>(population_size_);
        diversity = calculateDiversity(population);

        evolution_history_.get<size_t>("generation").add(generation + 1);
        evolution_history_.get<double>("best_fitness").add(best_fitness);
        evolution_history_.get<double>("avg_fitness").add(avg_fitness);
        evolution_history_.get<double>("worst_fitness").add(worst_fitness);
        evolution_history_.get<double>("diversity").add(diversity);

        // Call generation callback if provided
        if (generation_callback_) {
            generation_callback_(generation + 1, best_fitness, avg_fitness,
                                 diversity);
        }

        // Verbose output
        if (verbose_) {
            std::cout << "Generation " << (generation + 1)
                      << ": Best fitness = " << best_fitness
                      << ", Avg fitness = " << avg_fitness
                      << ", Diversity = " << diversity << std::endl;
        }

        // Check for early convergence if diversity is too low
        if (diversity < 1e-6) {
            if (verbose_) {
                std::cout << "Early stopping due to low diversity."
                          << std::endl;
            }
            break;
        }
    }

    // Stop the timer
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end_time - start_time;
    optimization_time_seconds_ = elapsed.count();

    if (verbose_) {
        std::cout << "Optimization completed in " << optimization_time_seconds_
                  << " seconds." << std::endl;
    }

    // Return the best individual and its fitness
    return std::make_pair(population[best_idx], best_fitness);
}

} // namespace ml
