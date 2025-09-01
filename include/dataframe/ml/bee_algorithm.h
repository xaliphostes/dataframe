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
#include <chrono>
#include <cmath>
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <functional>
#include <limits>
#include <random>

namespace ml {

/**
 * @brief Implementation of Artificial Bee Colony (ABC) algorithm
 *
 * The Artificial Bee Colony algorithm is a swarm intelligence-based
 * optimization algorithm inspired by the foraging behavior of honey bees. It
 * consists of three phases: employed bee phase, onlooker bee phase, and scout
 * bee phase.
 *
 * Features:
 * - Support for both continuous and combinatorial optimization problems
 * - Automatic adaptation to minimization or maximization problems
 * - Configurable parameters for fine-tuning the algorithm behavior
 * - Integration with DataFrame library for efficient data handling
 * - Comprehensive evolution history and metrics tracking
 *
 * Example usage for continuous optimization:
 * @code
 * // Create a Bee Algorithm optimizer
 * ml::BeeAlgorithm ba(50, 50, 15, 100);
 *
 * // Define the objective function (minimize Rosenbrock function)
 * auto objective = [](const df::Serie<double>& x) {
 *     double a = 1.0, b = 100.0;
 *     return b * std::pow(x[1] - x[0]*x[0], 2) + std::pow(a - x[0], 2);
 * };
 *
 * // Define variable bounds
 * df::Serie<double> lower_bounds{-5.0, -5.0};
 * df::Serie<double> upper_bounds{5.0, 5.0};
 *
 * // Run optimization
 * auto [solution, fitness] = ba.optimize(objective, lower_bounds, upper_bounds,
 * true);
 *
 * // Print results
 * std::cout << "Best solution: " << solution << std::endl;
 * std::cout << "Fitness: " << fitness << std::endl;
 * @endcode
 *
 * Example usage for combinatorial optimization (TSP):
 * @code
 * // Create a Bee Algorithm for TSP
 * ml::BeeAlgorithm ba(50, 30, 15, 150);
 *
 * // Define cities as points
 * df::Serie<df::Vector2> cities{
 *     {0, 0}, {1, 5}, {2, 3}, {5, 2}, {6, 8}, {7, 4}, {9, 7}
 * };
 *
 * // Create indices for permutation
 * df::Serie<int> indices = df::range<int>(0, cities.size());
 *
 * // Define the objective function (total distance)
 * auto objective = [&cities](const df::Serie<int>& route) {
 *     double total_distance = 0.0;
 *     for (size_t i = 0; i < route.size() - 1; ++i) {
 *         const auto& city1 = cities[route[i]];
 *         const auto& city2 = cities[route[i+1]];
 *         double dx = city1[0] - city2[0];
 *         double dy = city1[1] - city2[1];
 *         total_distance += std::sqrt(dx*dx + dy*dy);
 *     }
 *     // Return to starting city
 *     const auto& first_city = cities[route[0]];
 *     const auto& last_city = cities[route[route.size()-1]];
 *     double dx = first_city[0] - last_city[0];
 *     double dy = first_city[1] - last_city[1];
 *     total_distance += std::sqrt(dx*dx + dy*dy);
 *     return total_distance;
 * };
 *
 * // Run optimization (minimize distance)
 * auto [best_route, distance] = ba.optimize_combinatorial(
 *     objective, indices, indices.size(), false, true
 * );
 *
 * // Print result
 * std::cout << "Best route: " << best_route << std::endl;
 * std::cout << "Total distance: " << distance << std::endl;
 * @endcode
 */
class BeeAlgorithm {
  public:
    /**
     * @brief Constructor for BeeAlgorithm
     *
     * @param colony_size Number of food sources (solutions)
     * @param employed_bees Number of employed bees (typically equals
     * colony_size)
     * @param onlooker_bees Number of onlooker bees
     * @param max_cycles Maximum number of optimization cycles
     * @param limit Maximum number of trials before abandoning a food source
     * @param neighborhood_size Size of neighborhood for continuous optimization
     * (default: 1.0)
     * @param verbose Whether to print progress information
     */
    BeeAlgorithm(size_t colony_size = 40, size_t employed_bees = 20,
                 size_t onlooker_bees = 20, size_t max_cycles = 100,
                 size_t limit = 20, double neighborhood_size = 1.0,
                 bool verbose = false);

    /**
     * @brief Set the neighborhood size for continuous optimization
     *
     * @param size Neighborhood size (scale factor for modifications)
     * @return Reference to this object for method chaining
     */
    BeeAlgorithm &setNeighborhoodSize(double size);

    /**
     * @brief Set the maximum number of trials before abandoning a food source
     *
     * @param limit Maximum trials
     * @return Reference to this object for method chaining
     */
    BeeAlgorithm &setLimit(size_t limit);

    /**
     * @brief Set whether to print progress information
     *
     * @param verbose Whether to print progress information
     * @return Reference to this object for method chaining
     */
    BeeAlgorithm &setVerbose(bool verbose);

    /**
     * @brief Set a callback function to be called after each cycle
     *
     * @param callback Function that takes (cycle, best_fitness, avg_fitness,
     * diversity)
     * @return Reference to this object for method chaining
     */
    BeeAlgorithm &setCycleCallback(
        std::function<void(size_t, double, double, double)> callback);

    /**
     * @brief Optimize a function with bounds (continuous optimization)
     *
     * @tparam T Type of parameters to optimize (typically double)
     * @param fitness_function Function that takes a Serie of parameters and
     * returns a fitness value
     * @param lower_bounds Lower bounds for parameters
     * @param upper_bounds Upper bounds for parameters
     * @param minimize Whether to minimize (true) or maximize (false) the
     * fitness function
     * @return std::pair<df::Serie<T>, double> Best solution and its fitness
     */
    template <typename T>
    std::pair<df::Serie<T>, double>
    optimize(std::function<double(const df::Serie<T> &)> fitness_function,
             const df::Serie<T> &lower_bounds, const df::Serie<T> &upper_bounds,
             bool minimize = true);

    /**
     * @brief Optimize a combinatorial problem
     *
     * @tparam T Type of elements in the candidate set
     * @param fitness_function Function that takes a Serie of elements and
     * returns a fitness value
     * @param candidate_set Set of candidate elements to use in solutions
     * @param solution_length Length of solutions to generate
     * @param allow_repetition Whether to allow the same element to appear
     * multiple times in a solution
     * @param minimize Whether to minimize (true) or maximize (false) the
     * fitness function
     * @return std::pair<df::Serie<T>, double> Best solution and its fitness
     */
    template <typename T>
    std::pair<df::Serie<T>, double> optimize_combinatorial(
        std::function<double(const df::Serie<T> &)> fitness_function,
        const df::Serie<T> &candidate_set, size_t solution_length,
        bool allow_repetition = false, bool minimize = true);

    /**
     * @brief Get the evolution history
     *
     * @return df::Dataframe with columns: "cycle", "best_fitness",
     * "avg_fitness", "diversity"
     */
    df::Dataframe get_evolution_history() const;

    /**
     * @brief Get metrics about the final population
     *
     * @return std::map<std::string, double> with keys: "best_fitness",
     * "avg_fitness", "diversity", "cycles", "time_seconds"
     */
    std::map<std::string, double> get_population_metrics() const;

  private:
    // Configuration parameters
    size_t colony_size_;
    size_t employed_bees_;
    size_t onlooker_bees_;
    size_t max_cycles_;
    size_t limit_;
    double neighborhood_size_;
    bool verbose_;

    // Random number generator
    std::mt19937 rng_;

    // Evolution history
    df::Dataframe evolution_history_;

    // Custom callback function
    std::function<void(size_t, double, double, double)> cycle_callback_;

    // Time tracking
    double optimization_time_seconds_ = 0.0;

    // Helper methods
    template <typename T>
    df::Serie<T> generateRandomSolution(const df::Serie<T> &lower_bounds,
                                        const df::Serie<T> &upper_bounds);

    template <typename T>
    df::Serie<T> generateCombinatorial(const df::Serie<T> &candidate_set,
                                       size_t solution_length,
                                       bool allow_repetition);

    template <typename T>
    df::Serie<T> modifySolution(const df::Serie<T> &solution,
                                const df::Serie<T> &lower_bounds,
                                const df::Serie<T> &upper_bounds,
                                size_t param_idx);

    template <typename T>
    df::Serie<T> modifyCombinatorial(const df::Serie<T> &solution,
                                     const df::Serie<T> &candidate_set,
                                     bool allow_repetition);

    template <typename T>
    double calculateDiversity(const std::vector<df::Serie<T>> &solutions);
};

/**
 * @brief Create a Bee Algorithm for continuous optimization
 *
 * @param colony_size Number of food sources (solutions)
 * @param max_cycles Maximum number of optimization cycles
 * @param limit Maximum number of trials before abandoning a food source
 * @return BeeAlgorithm Configured for continuous optimization
 */
inline BeeAlgorithm create_bee_algorithm(size_t colony_size = 40,
                                         size_t max_cycles = 100,
                                         size_t limit = 20) {
    return BeeAlgorithm(colony_size, colony_size / 2, colony_size / 2,
                        max_cycles, limit);
}

/**
 * @brief Create a Bee Algorithm for combinatorial optimization
 *
 * @param colony_size Number of food sources (solutions)
 * @param max_cycles Maximum number of optimization cycles
 * @param limit Maximum number of trials before abandoning a food source
 * @return BeeAlgorithm Configured for combinatorial optimization
 */
inline BeeAlgorithm create_bee_algorithm_combinatorial(size_t colony_size = 40,
                                                       size_t max_cycles = 150,
                                                       size_t limit = 30) {
    // For combinatorial problems, we typically want more exploration
    BeeAlgorithm ba(colony_size, colony_size / 2, colony_size / 2, max_cycles,
                    limit);
    return ba;
}

} // namespace ml

#include "inline/bee_algorithm.hxx"