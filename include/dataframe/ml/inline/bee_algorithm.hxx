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

#include <dataframe/math/random.h>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <unordered_set>

namespace ml {

    // BeeAlgorithm constructor implementation
    inline BeeAlgorithm::BeeAlgorithm(size_t colony_size, size_t employed_bees,
        size_t onlooker_bees, size_t max_cycles, size_t limit, double neighborhood_size,
        bool verbose)
        : colony_size_(colony_size)
        , employed_bees_(employed_bees)
        , onlooker_bees_(onlooker_bees)
        , max_cycles_(max_cycles)
        , limit_(limit)
        , neighborhood_size_(neighborhood_size)
        , verbose_(verbose)
        , rng_(std::random_device {}())
    {
        // Validate parameters
        if (colony_size_ < 1) {
            throw std::invalid_argument("Colony size must be at least 1");
        }
        if (employed_bees_ < 1) {
            throw std::invalid_argument("Number of employed bees must be at least 1");
        }
        if (onlooker_bees_ < 1) {
            throw std::invalid_argument("Number of onlooker bees must be at least 1");
        }
        if (neighborhood_size_ <= 0.0) {
            throw std::invalid_argument("Neighborhood size must be positive");
        }

        // Initialize evolution history dataframe
        evolution_history_ = df::Dataframe();
        evolution_history_.add("cycle", df::Serie<size_t> {});
        evolution_history_.add("best_fitness", df::Serie<double> {});
        evolution_history_.add("avg_fitness", df::Serie<double> {});
        evolution_history_.add("diversity", df::Serie<double> {});
    }

    // Setter methods
    inline BeeAlgorithm& BeeAlgorithm::setNeighborhoodSize(double size)
    {
        if (size <= 0.0) {
            throw std::invalid_argument("Neighborhood size must be positive");
        }
        neighborhood_size_ = size;
        return *this;
    }

    inline BeeAlgorithm& BeeAlgorithm::setLimit(size_t limit)
    {
        limit_ = limit;
        return *this;
    }

    inline BeeAlgorithm& BeeAlgorithm::setVerbose(bool verbose)
    {
        verbose_ = verbose;
        return *this;
    }

    inline BeeAlgorithm& BeeAlgorithm::setCycleCallback(
        std::function<void(size_t, double, double, double)> callback)
    {
        cycle_callback_ = callback;
        return *this;
    }

    // Get evolution history
    inline df::Dataframe BeeAlgorithm::get_evolution_history() const { return evolution_history_; }

    // Get population metrics
    inline std::map<std::string, double> BeeAlgorithm::get_population_metrics() const
    {
        std::map<std::string, double> metrics;

        // Extract metrics from the last cycle in the evolution history
        if (!evolution_history_.size() == 0 && evolution_history_.get<size_t>("cycle").size() > 0) {
            size_t last_idx = evolution_history_.get<size_t>("cycle").size() - 1;
            metrics["best_fitness"] = evolution_history_.get<double>("best_fitness")[last_idx];
            metrics["avg_fitness"] = evolution_history_.get<double>("avg_fitness")[last_idx];
            metrics["diversity"] = evolution_history_.get<double>("diversity")[last_idx];
            metrics["cycles"] = evolution_history_.get<size_t>("cycle")[last_idx] + 1;
        }

        metrics["time_seconds"] = optimization_time_seconds_;

        return metrics;
    }

    // Helper method to generate random solution for continuous optimization
    template <typename T>
    df::Serie<T> BeeAlgorithm::generateRandomSolution(
        const df::Serie<T>& lower_bounds, const df::Serie<T>& upper_bounds)
    {
        std::vector<T> values;
        values.reserve(lower_bounds.size());

        for (size_t i = 0; i < lower_bounds.size(); ++i) {
            if constexpr (std::is_floating_point_v<T>) {
                // For floating-point types, generate a random number in the range
                // [lower, upper)
                std::uniform_real_distribution<T> dist(lower_bounds[i], upper_bounds[i]);
                values.push_back(dist(rng_));
            } else if constexpr (std::is_integral_v<T>) {
                // For integral types, generate a random number in the range [lower,
                // upper]
                std::uniform_int_distribution<T> dist(lower_bounds[i], upper_bounds[i]);
                values.push_back(dist(rng_));
            } else {
                // For other types, use a ratio approach
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                double ratio = dist(rng_);
                // Interpolate between lower and upper bound based on ratio
                values.push_back(
                    lower_bounds[i] + static_cast<T>(ratio * (upper_bounds[i] - lower_bounds[i])));
            }
        }

        return df::Serie<T>(values);
    }

    // Helper method to generate random solution for combinatorial optimization
    template <typename T>
    df::Serie<T> BeeAlgorithm::generateCombinatorial(
        const df::Serie<T>& candidate_set, size_t solution_length, bool allow_repetition)
    {
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
                throw std::invalid_argument("Solution length cannot exceed candidate set size when "
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

    // Helper method to modify a solution (employed and onlooker bee phase)
    template <typename T>
    df::Serie<T> BeeAlgorithm::modifySolution(const df::Serie<T>& solution,
        const df::Serie<T>& lower_bounds, const df::Serie<T>& upper_bounds, size_t param_idx)
    {
        // Choose a random parameter to modify
        std::uniform_int_distribution<size_t> param_dist(0, solution.size() - 1);
        if (param_idx >= solution.size()) {
            param_idx = param_dist(rng_);
        }

        // Choose a random different solution to use for modification
        std::uniform_int_distribution<size_t> solution_dist(0, solution.size() - 1);
        size_t other_param_idx = param_dist(rng_);
        while (other_param_idx == param_idx) {
            other_param_idx = param_dist(rng_);
        }

        // Generate a random factor within [-1, 1] for modification scale
        std::uniform_real_distribution<double> phi_dist(-1.0, 1.0);
        double phi = phi_dist(rng_);

        // Create the modified solution
        std::vector<T> modified_values(solution.data());

        if constexpr (std::is_floating_point_v<T>) {
            // For floating-point types, apply basic ABC formula: v_ij = x_ij + φ_ij
            // * (x_ij - x_kj)
            modified_values[param_idx] += phi * neighborhood_size_
                * (modified_values[param_idx] - modified_values[other_param_idx]);

            // Ensure the modified value is within bounds
            modified_values[param_idx] = std::clamp(
                modified_values[param_idx], lower_bounds[param_idx], upper_bounds[param_idx]);
        } else if constexpr (std::is_integral_v<T>) {
            // For integral types, apply a modified formula with rounding
            double modified = modified_values[param_idx]
                + phi * neighborhood_size_
                    * (modified_values[param_idx] - modified_values[other_param_idx]);

            // Round to nearest integer and clamp to bounds
            modified_values[param_idx] = std::clamp(static_cast<T>(std::round(modified)),
                lower_bounds[param_idx], upper_bounds[param_idx]);
        } else {
            // For other types, use simple swapping (less effective but safe)
            std::swap(modified_values[param_idx], modified_values[other_param_idx]);
        }

        return df::Serie<T>(modified_values);
    }

    // Helper method to modify a combinatorial solution
    template <typename T>
    df::Serie<T> BeeAlgorithm::modifyCombinatorial(
        const df::Serie<T>& solution, const df::Serie<T>& candidate_set, bool allow_repetition)
    {
        std::vector<T> modified_values(solution.data());

        // Choose a random modification strategy
        std::uniform_int_distribution<int> strategy_dist(0, 2);
        int strategy = strategy_dist(rng_);

        if (strategy == 0) {
            // Swap two elements
            if (solution.size() < 2)
                return solution;

            std::uniform_int_distribution<size_t> pos_dist(0, solution.size() - 1);
            size_t pos1 = pos_dist(rng_);
            size_t pos2 = pos_dist(rng_);

            // Make sure positions are different
            while (pos2 == pos1 && solution.size() > 1) {
                pos2 = pos_dist(rng_);
            }

            std::swap(modified_values[pos1], modified_values[pos2]);
        } else if (strategy == 1) {
            // Replace a random element with a random candidate
            if (!allow_repetition) {
                // For non-repetition, we need to swap with an unused candidate
                std::unordered_set<T> used_elements;
                for (const auto& val : solution.data()) {
                    used_elements.insert(val);
                }

                std::vector<T> unused_candidates;
                for (size_t i = 0; i < candidate_set.size(); ++i) {
                    if (used_elements.find(candidate_set[i]) == used_elements.end()) {
                        unused_candidates.push_back(candidate_set[i]);
                    }
                }

                if (!unused_candidates.empty()) {
                    // Choose a random position to replace
                    std::uniform_int_distribution<size_t> pos_dist(0, solution.size() - 1);
                    size_t pos = pos_dist(rng_);

                    // Choose a random unused candidate
                    std::uniform_int_distribution<size_t> cand_dist(
                        0, unused_candidates.size() - 1);
                    T new_element = unused_candidates[cand_dist(rng_)];

                    modified_values[pos] = new_element;
                }
            } else {
                // With repetition, just choose any candidate
                std::uniform_int_distribution<size_t> pos_dist(0, solution.size() - 1);
                std::uniform_int_distribution<size_t> cand_dist(0, candidate_set.size() - 1);

                size_t pos = pos_dist(rng_);
                T new_element = candidate_set[cand_dist(rng_)];

                modified_values[pos] = new_element;
            }
        } else if (strategy == 2) {
            // Reverse a segment
            if (solution.size() < 3)
                return solution;

            std::uniform_int_distribution<size_t> pos_dist(0, solution.size() - 1);
            size_t pos1 = pos_dist(rng_);
            size_t pos2 = pos_dist(rng_);

            // Make sure pos1 < pos2
            if (pos1 > pos2) {
                std::swap(pos1, pos2);
            }

            // Make sure segment has at least 2 elements
            if (pos2 - pos1 < 1 && solution.size() > 2) {
                pos2 = std::min(solution.size() - 1, pos1 + 2);
            }

            // Reverse the segment
            std::reverse(modified_values.begin() + pos1, modified_values.begin() + pos2 + 1);
        }

        return df::Serie<T>(modified_values);
    }

    // Calculate population diversity
    template <typename T>
    double BeeAlgorithm::calculateDiversity(const std::vector<df::Serie<T>>& solutions)
    {
        if (solutions.empty() || solutions[0].size() == 0) {
            return 0.0;
        }

        // For numerical types, calculate standard deviation of each parameter
        if constexpr (std::is_arithmetic_v<T>) {
            double avg_std_dev = 0.0;

            for (size_t j = 0; j < solutions[0].size(); ++j) {
                // Calculate mean for this parameter
                double mean = 0.0;
                for (size_t i = 0; i < solutions.size(); ++i) {
                    mean += solutions[i][j] / static_cast<double>(solutions.size());
                }

                // Calculate variance for this parameter
                double variance = 0.0;
                for (size_t i = 0; i < solutions.size(); ++i) {
                    double diff = solutions[i][j] - mean;
                    variance += (diff * diff) / static_cast<double>(solutions.size());
                }

                // Add std dev to average
                avg_std_dev += std::sqrt(variance) / static_cast<double>(solutions[0].size());
            }

            return avg_std_dev;
        } else {
            // For non-numerical types, calculate a similarity index
            double similarity_sum = 0.0;
            double count = 0.0;

            for (size_t i = 0; i < solutions.size(); ++i) {
                for (size_t j = i + 1; j < solutions.size(); ++j) {
                    // Count number of matching elements
                    size_t matches = 0;
                    for (size_t k = 0; k < solutions[i].size(); ++k) {
                        if (solutions[i][k] == solutions[j][k]) {
                            matches++;
                        }
                    }

                    // Add similarity (as percentage of matches) to sum
                    similarity_sum
                        += static_cast<double>(matches) / static_cast<double>(solutions[i].size());
                    count += 1.0;
                }
            }

            // Convert similarity to diversity (1 - avg_similarity)
            return count > 0.0 ? 1.0 - (similarity_sum / count) : 0.0;
        }
    }

    // Main optimization method for continuous problems
    template <typename T>
    std::pair<df::Serie<T>, double> BeeAlgorithm::optimize(
        std::function<double(const df::Serie<T>&)> fitness_function,
        const df::Serie<T>& lower_bounds, const df::Serie<T>& upper_bounds, bool minimize)
    {
        if (lower_bounds.size() != upper_bounds.size()) {
            throw std::invalid_argument("Lower bounds and upper bounds must have the same size");
        }

        // Start a timer
        auto start_time = std::chrono::high_resolution_clock::now();

        // Initialize evolution history
        evolution_history_ = df::Dataframe();
        evolution_history_.add("cycle", df::Serie<size_t> {});
        evolution_history_.add("best_fitness", df::Serie<double> {});
        evolution_history_.add("avg_fitness", df::Serie<double> {});
        evolution_history_.add("diversity", df::Serie<double> {});

        // Initialize food sources (solutions)
        std::vector<df::Serie<T>> food_sources;
        std::vector<double> fitness_values;
        std::vector<size_t> trial_counters;

        food_sources.reserve(colony_size_);
        fitness_values.reserve(colony_size_);
        trial_counters.resize(colony_size_, 0);

        // Generate initial food sources
        for (size_t i = 0; i < colony_size_; ++i) {
            food_sources.push_back(generateRandomSolution(lower_bounds, upper_bounds));
            fitness_values.push_back(fitness_function(food_sources[i]));
        }

        // Find initial best food source
        size_t best_idx = 0;
        double best_fitness = fitness_values[0];

        for (size_t i = 1; i < colony_size_; ++i) {
            if ((minimize && fitness_values[i] < best_fitness)
                || (!minimize && fitness_values[i] > best_fitness)) {
                best_fitness = fitness_values[i];
                best_idx = i;
            }
        }

        // Calculate initial diversity and average fitness
        double diversity = calculateDiversity(food_sources);
        double avg_fitness
            = std::accumulate(fitness_values.begin(), fitness_values.end(), 0.0) / colony_size_;

        // Add initial stats to evolution history
        evolution_history_.get<size_t>("cycle").add(0);
        evolution_history_.get<double>("best_fitness").add(best_fitness);
        evolution_history_.get<double>("avg_fitness").add(avg_fitness);
        evolution_history_.get<double>("diversity").add(diversity);

        // Call cycle callback if provided
        if (cycle_callback_) {
            cycle_callback_(0, best_fitness, avg_fitness, diversity);
        }

        // Verbose output
        if (verbose_) {
            std::cout << "Cycle 0: Best fitness = " << best_fitness
                      << ", Avg fitness = " << avg_fitness << ", Diversity = " << diversity
                      << std::endl;
        }

        // Main optimization loop
        for (size_t cycle = 0; cycle < max_cycles_; ++cycle) {
            // EMPLOYED BEE PHASE
            // Each employed bee visits a food source and produces a new solution
            for (size_t i = 0; i < employed_bees_; ++i) {
                // Map employed bee index to food source index
                size_t source_idx = i % colony_size_;

                // Choose a random parameter to modify
                std::uniform_int_distribution<size_t> param_dist(0, lower_bounds.size() - 1);
                size_t param_idx = param_dist(rng_);

                // Create a modified solution
                df::Serie<T> new_solution = modifySolution(
                    food_sources[source_idx], lower_bounds, upper_bounds, param_idx);

                // Evaluate the new solution
                double new_fitness = fitness_function(new_solution);

                // Apply greedy selection
                bool improved = (minimize && new_fitness < fitness_values[source_idx])
                    || (!minimize && new_fitness > fitness_values[source_idx]);

                if (improved) {
                    // Replace the old solution with the new one
                    food_sources[source_idx] = new_solution;
                    fitness_values[source_idx] = new_fitness;
                    trial_counters[source_idx] = 0;

                    // Update best solution if needed
                    if ((minimize && new_fitness < best_fitness)
                        || (!minimize && new_fitness > best_fitness)) {
                        best_fitness = new_fitness;
                        best_idx = source_idx;
                    }
                } else {
                    // Increment trial counter
                    trial_counters[source_idx]++;
                }
            }

            // ONLOOKER BEE PHASE
            // Calculate selection probabilities for food sources
            std::vector<double> probabilities;
            probabilities.reserve(colony_size_);

            double sum_fitness = 0.0;
            double max_fitness = *std::max_element(fitness_values.begin(), fitness_values.end());
            double min_fitness = *std::min_element(fitness_values.begin(), fitness_values.end());

            // Adjust fitnesses for probability calculation
            for (size_t i = 0; i < colony_size_; ++i) {
                double adjusted_fitness;

                if (minimize) {
                    // For minimization, invert the fitness values
                    adjusted_fitness = max_fitness + min_fitness - fitness_values[i];
                } else {
                    // For maximization, use fitness values directly
                    adjusted_fitness = fitness_values[i];
                }

                // Ensure non-negative values
                adjusted_fitness = std::max(adjusted_fitness, 1e-10);
                sum_fitness += adjusted_fitness;
                probabilities.push_back(adjusted_fitness);
            }

            // Normalize probabilities
            for (size_t i = 0; i < colony_size_; ++i) {
                probabilities[i] /= sum_fitness;
            }

            // Onlooker bees choose food sources based on probabilities
            for (size_t i = 0; i < onlooker_bees_; ++i) {
                // Select a food source based on probability
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                double r = dist(rng_);
                size_t selected_idx = 0;
                double cumulative_prob = probabilities[0];

                while (r > cumulative_prob && selected_idx < colony_size_ - 1) {
                    selected_idx++;
                    cumulative_prob += probabilities[selected_idx];
                }

                // Choose a random parameter to modify
                std::uniform_int_distribution<size_t> param_dist(0, lower_bounds.size() - 1);
                size_t param_idx = param_dist(rng_);

                // Create a modified solution
                df::Serie<T> new_solution = modifySolution(
                    food_sources[selected_idx], lower_bounds, upper_bounds, param_idx);

                // Evaluate the new solution
                double new_fitness = fitness_function(new_solution);

                // Apply greedy selection
                bool improved = (minimize && new_fitness < fitness_values[selected_idx])
                    || (!minimize && new_fitness > fitness_values[selected_idx]);

                if (improved) {
                    // Replace the old solution with the new one
                    food_sources[selected_idx] = new_solution;
                    fitness_values[selected_idx] = new_fitness;
                    trial_counters[selected_idx] = 0;

                    // Update best solution if needed
                    if ((minimize && new_fitness < best_fitness)
                        || (!minimize && new_fitness > best_fitness)) {
                        best_fitness = new_fitness;
                        best_idx = selected_idx;
                    }
                } else {
                    // Increment trial counter
                    trial_counters[selected_idx]++;
                }
            }

            // SCOUT BEE PHASE
            // Scout bees check for abandoned food sources
            for (size_t i = 0; i < colony_size_; ++i) {
                if (trial_counters[i] > limit_) {
                    // Generate a new random solution
                    food_sources[i] = generateRandomSolution(lower_bounds, upper_bounds);
                    fitness_values[i] = fitness_function(food_sources[i]);
                    trial_counters[i] = 0;

                    // Update best solution if needed
                    if ((minimize && fitness_values[i] < best_fitness)
                        || (!minimize && fitness_values[i] > best_fitness)) {
                        best_fitness = fitness_values[i];
                        best_idx = i;
                    }
                }
            }

            // Calculate cycle diversity and average fitness
            diversity = calculateDiversity(food_sources);
            avg_fitness
                = std::accumulate(fitness_values.begin(), fitness_values.end(), 0.0) / colony_size_;

            // Add cycle stats to evolution history
            evolution_history_.get<size_t>("cycle").add(cycle + 1);
            evolution_history_.get<double>("best_fitness").add(best_fitness);
            evolution_history_.get<double>("avg_fitness").add(avg_fitness);
            evolution_history_.get<double>("diversity").add(diversity);

            // Call cycle callback if provided
            if (cycle_callback_) {
                cycle_callback_(cycle + 1, best_fitness, avg_fitness, diversity);
            }

            // Verbose output
            if (verbose_) {
                std::cout << "Cycle " << (cycle + 1) << ": Best fitness = " << best_fitness
                          << ", Avg fitness = " << avg_fitness << ", Diversity = " << diversity
                          << std::endl;
            }

            // Check for early convergence if diversity is too low
            if (diversity < 1e-6) {
                if (verbose_) {
                    std::cout << "Early stopping due to low diversity." << std::endl;
                }
                break;
            }
        }

        // Stop the timer
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        optimization_time_seconds_ = elapsed.count();

        if (verbose_) {
            std::cout << "Optimization completed in " << optimization_time_seconds_ << " seconds."
                      << std::endl;
        }

        // Return the best solution and its fitness
        return std::make_pair(food_sources[best_idx], best_fitness);
    }

    // Main optimization method for combinatorial problems
    template <typename T>
    std::pair<df::Serie<T>, double> BeeAlgorithm::optimize_combinatorial(
        std::function<double(const df::Serie<T>&)> fitness_function,
        const df::Serie<T>& candidate_set, size_t solution_length, bool allow_repetition,
        bool minimize)
    {
        // Start a timer
        auto start_time = std::chrono::high_resolution_clock::now();

        // Initialize evolution history
        evolution_history_ = df::Dataframe();
        evolution_history_.add("cycle", df::Serie<size_t> {});
        evolution_history_.add("best_fitness", df::Serie<double> {});
        evolution_history_.add("avg_fitness", df::Serie<double> {});
        evolution_history_.add("diversity", df::Serie<double> {});

        // Initialize food sources (solutions)
        std::vector<df::Serie<T>> food_sources;
        std::vector<double> fitness_values;
        std::vector<size_t> trial_counters;

        food_sources.reserve(colony_size_);
        fitness_values.reserve(colony_size_);
        trial_counters.resize(colony_size_, 0);

        // Generate initial food sources
        for (size_t i = 0; i < colony_size_; ++i) {
            food_sources.push_back(
                generateCombinatorial(candidate_set, solution_length, allow_repetition));
            fitness_values.push_back(fitness_function(food_sources[i]));
        }

        // Find initial best food source
        size_t best_idx = 0;
        double best_fitness = fitness_values[0];

        for (size_t i = 1; i < colony_size_; ++i) {
            if ((minimize && fitness_values[i] < best_fitness)
                || (!minimize && fitness_values[i] > best_fitness)) {
                best_fitness = fitness_values[i];
                best_idx = i;
            }
        }

        // Calculate initial diversity and average fitness
        double diversity = calculateDiversity(food_sources);
        double avg_fitness
            = std::accumulate(fitness_values.begin(), fitness_values.end(), 0.0) / colony_size_;

        // Add initial stats to evolution history
        evolution_history_.get<size_t>("cycle").add(0);
        evolution_history_.get<double>("best_fitness").add(best_fitness);
        evolution_history_.get<double>("avg_fitness").add(avg_fitness);
        evolution_history_.get<double>("diversity").add(diversity);

        // Call cycle callback if provided
        if (cycle_callback_) {
            cycle_callback_(0, best_fitness, avg_fitness, diversity);
        }

        // Verbose output
        if (verbose_) {
            std::cout << "Cycle 0: Best fitness = " << best_fitness
                      << ", Avg fitness = " << avg_fitness << ", Diversity = " << diversity
                      << std::endl;
        }

        // Main optimization loop
        for (size_t cycle = 0; cycle < max_cycles_; ++cycle) {
            // EMPLOYED BEE PHASE
            // Each employed bee visits a food source and produces a new solution
            for (size_t i = 0; i < employed_bees_; ++i) {
                // Map employed bee index to food source index
                size_t source_idx = i % colony_size_;

                // Create a modified solution
                df::Serie<T> new_solution = modifyCombinatorial(
                    food_sources[source_idx], candidate_set, allow_repetition);

                // Evaluate the new solution
                double new_fitness = fitness_function(new_solution);

                // Apply greedy selection
                bool improved = (minimize && new_fitness < fitness_values[source_idx])
                    || (!minimize && new_fitness > fitness_values[source_idx]);

                if (improved) {
                    // Replace the old solution with the new one
                    food_sources[source_idx] = new_solution;
                    fitness_values[source_idx] = new_fitness;
                    trial_counters[source_idx] = 0;

                    // Update best solution if needed
                    if ((minimize && new_fitness < best_fitness)
                        || (!minimize && new_fitness > best_fitness)) {
                        best_fitness = new_fitness;
                        best_idx = source_idx;
                    }
                } else {
                    // Increment trial counter
                    trial_counters[source_idx]++;
                }
            }

            // ONLOOKER BEE PHASE
            // Calculate selection probabilities for food sources
            std::vector<double> probabilities;
            probabilities.reserve(colony_size_);

            double sum_fitness = 0.0;
            double max_fitness = *std::max_element(fitness_values.begin(), fitness_values.end());
            double min_fitness = *std::min_element(fitness_values.begin(), fitness_values.end());

            // Adjust fitnesses for probability calculation
            for (size_t i = 0; i < colony_size_; ++i) {
                double adjusted_fitness;

                if (minimize) {
                    // For minimization, invert the fitness values
                    adjusted_fitness = max_fitness + min_fitness - fitness_values[i];
                } else {
                    // For maximization, use fitness values directly
                    adjusted_fitness = fitness_values[i];
                }

                // Ensure non-negative values
                adjusted_fitness = std::max(adjusted_fitness, 1e-10);
                sum_fitness += adjusted_fitness;
                probabilities.push_back(adjusted_fitness);
            }

            // Normalize probabilities
            for (size_t i = 0; i < colony_size_; ++i) {
                probabilities[i] /= sum_fitness;
            }

            // Onlooker bees choose food sources based on probabilities
            for (size_t i = 0; i < onlooker_bees_; ++i) {
                // Select a food source based on probability
                std::uniform_real_distribution<double> dist(0.0, 1.0);
                double r = dist(rng_);
                size_t selected_idx = 0;
                double cumulative_prob = probabilities[0];

                while (r > cumulative_prob && selected_idx < colony_size_ - 1) {
                    selected_idx++;
                    cumulative_prob += probabilities[selected_idx];
                }

                // Create a modified solution
                df::Serie<T> new_solution = modifyCombinatorial(
                    food_sources[selected_idx], candidate_set, allow_repetition);

                // Evaluate the new solution
                double new_fitness = fitness_function(new_solution);

                // Apply greedy selection
                bool improved = (minimize && new_fitness < fitness_values[selected_idx])
                    || (!minimize && new_fitness > fitness_values[selected_idx]);

                if (improved) {
                    // Replace the old solution with the new one
                    food_sources[selected_idx] = new_solution;
                    fitness_values[selected_idx] = new_fitness;
                    trial_counters[selected_idx] = 0;

                    // Update best solution if needed
                    if ((minimize && new_fitness < best_fitness)
                        || (!minimize && new_fitness > best_fitness)) {
                        best_fitness = new_fitness;
                        best_idx = selected_idx;
                    }
                } else {
                    // Increment trial counter
                    trial_counters[selected_idx]++;
                }
            }

            // SCOUT BEE PHASE
            // Scout bees check for abandoned food sources
            for (size_t i = 0; i < colony_size_; ++i) {
                if (trial_counters[i] > limit_) {
                    // Generate a new random solution
                    food_sources[i]
                        = generateCombinatorial(candidate_set, solution_length, allow_repetition);
                    fitness_values[i] = fitness_function(food_sources[i]);
                    trial_counters[i] = 0;

                    // Update best solution if needed
                    if ((minimize && fitness_values[i] < best_fitness)
                        || (!minimize && fitness_values[i] > best_fitness)) {
                        best_fitness = fitness_values[i];
                        best_idx = i;
                    }
                }
            }

            // Calculate cycle diversity and average fitness
            diversity = calculateDiversity(food_sources);
            avg_fitness
                = std::accumulate(fitness_values.begin(), fitness_values.end(), 0.0) / colony_size_;

            // Add cycle stats to evolution history
            evolution_history_.get<size_t>("cycle").add(cycle + 1);
            evolution_history_.get<double>("best_fitness").add(best_fitness);
            evolution_history_.get<double>("avg_fitness").add(avg_fitness);
            evolution_history_.get<double>("diversity").add(diversity);

            // Call cycle callback if provided
            if (cycle_callback_) {
                cycle_callback_(cycle + 1, best_fitness, avg_fitness, diversity);
            }

            // Verbose output
            if (verbose_) {
                std::cout << "Cycle " << (cycle + 1) << ": Best fitness = " << best_fitness
                          << ", Avg fitness = " << avg_fitness << ", Diversity = " << diversity
                          << std::endl;
            }

            // Check for early convergence if diversity is too low
            if (diversity < 0.1) { // For combinatorial problems, use a higher threshold
                if (verbose_) {
                    std::cout << "Early stopping due to low diversity." << std::endl;
                }
                break;
            }
        }

        // Stop the timer
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        optimization_time_seconds_ = elapsed.count();

        if (verbose_) {
            std::cout << "Optimization completed in " << optimization_time_seconds_ << " seconds."
                      << std::endl;
        }

        // Return the best solution and its fitness
        return std::make_pair(food_sources[best_idx], best_fitness);
    }

} // namespace ml