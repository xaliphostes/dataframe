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

#pragma once
#include <dataframe/Dataframe.h>
#include <dataframe/Serie.h>
#include <random>
#include <utility>

namespace ml {

/**
 * @brief Selection methods for the genetic algorithm
 */
enum class SelectionMethod {
    TOURNAMENT, ///< Tournament selection
    ROULETTE,   ///< Roulette wheel selection
    RANK        ///< Rank-based selection
};

/**
 * @brief Crossover methods for the genetic algorithm
 */
enum class CrossoverMethod {
    SINGLE_POINT, ///< Single-point crossover
    TWO_POINT,    ///< Two-point crossover
    UNIFORM,      ///< Uniform crossover
    ARITHMETIC,   ///< Arithmetic crossover (for numerical optimization)
    ORDER_BASED   ///< Order-based crossover (for combinatorial problems)
};

/**
 * @brief Mutation methods for the genetic algorithm
 */
enum class MutationMethod {
    GAUSSIAN,  ///< Gaussian mutation (for numerical optimization)
    UNIFORM,   ///< Uniform mutation
    SWAP,      ///< Swap mutation (for combinatorial problems)
    INVERSION, ///< Inversion mutation (for combinatorial problems)
    SCRAMBLE   ///< Scramble mutation (for combinatorial problems)
};

/**
 * @brief A C++ implementation of a Genetic Algorithm integrated with the
 * DataFrame library
 *
 * This class implements a genetic algorithm for optimization problems,
 * supporting both numerical optimization and combinatorial problems. It is
 * designed to work seamlessly with the DataFrame library for data handling and
 * analysis.
 *
 * Example usage for numerical optimization:
 * @code
 * // Create a genetic algorithm
 * ml::GeneticAlgorithm ga(100, 0.8, 0.1, 5, 100, "tournament");
 *
 * // Define the objective function (minimize x^2 + y^2)
 * auto objective = [](const df::Serie<double>& x) {
 *     return x[0] * x[0] + x[1] * x[1];
 * };
 *
 * // Define bounds for variables
 * df::Serie<double> lower_bounds{-10.0, -10.0};
 * df::Serie<double> upper_bounds{10.0, 10.0};
 *
 * // Run optimization (minimize)
 * auto [solution, fitness] = ga.optimize(objective, lower_bounds, upper_bounds,
 * true);
 *
 * // Print result
 * std::cout << "Best solution: [" << solution[0] << ", " << solution[1] << "]"
 * << std::endl; std::cout << "Fitness: " << fitness << std::endl;
 * @endcode
 *
 * Example usage for combinatorial optimization (TSP):
 * @code
 * // Create a genetic algorithm for TSP
 * ml::GeneticAlgorithm ga(100, 0.8, 0.2, 5, 200, "tournament");
 * ga.setCrossoverMethod("order_based");
 * ga.setMutationMethod("inversion");
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
 * auto [best_route, distance] = ga.optimize_combinatorial(
 *     objective, indices, indices.size(), false, true
 * );
 *
 * // Print result
 * std::cout << "Best route: " << best_route << std::endl;
 * std::cout << "Total distance: " << distance << std::endl;
 * @endcode
 */
class GeneticAlgorithm {
  public:
    /**
     * @brief Construct a new Genetic Algorithm object
     *
     * @param population_size Number of individuals in the population
     * @param crossover_rate Probability of crossover (0.0 to 1.0)
     * @param mutation_rate Probability of mutation (0.0 to 1.0)
     * @param elite_count Number of best individuals to preserve in each
     * generation
     * @param max_generations Maximum number of generations to run
     * @param selection_method Selection method ("tournament", "roulette", or
     * "rank")
     * @param crossover_method Crossover method (default is "single_point")
     * @param mutation_method Mutation method (default is "gaussian" for
     * numerical, "swap" for combinatorial)
     * @param tournament_size Size of tournament for tournament selection
     * (default is 3)
     * @param verbose Whether to print progress information (default is false)
     */
    GeneticAlgorithm(size_t population_size = 50, double crossover_rate = 0.8,
                     double mutation_rate = 0.1, size_t elite_count = 2,
                     size_t max_generations = 100,
                     const std::string &selection_method = "tournament",
                     const std::string &crossover_method = "single_point",
                     const std::string &mutation_method = "gaussian",
                     size_t tournament_size = 3, bool verbose = false);

    /**
     * @brief Set selection method
     *
     * @param method Selection method name or enum
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setSelectionMethod(const std::string &method);
    GeneticAlgorithm &setSelectionMethod(SelectionMethod method);

    /**
     * @brief Set crossover method
     *
     * @param method Crossover method name or enum
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setCrossoverMethod(const std::string &method);
    GeneticAlgorithm &setCrossoverMethod(CrossoverMethod method);

    /**
     * @brief Set mutation method
     *
     * @param method Mutation method name or enum
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setMutationMethod(const std::string &method);
    GeneticAlgorithm &setMutationMethod(MutationMethod method);

    /**
     * @brief Set tournament size for tournament selection
     *
     * @param size Tournament size (must be at least 2)
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setTournamentSize(size_t size);

    /**
     * @brief Set whether to print progress information
     *
     * @param verbose Whether to print progress information
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setVerbose(bool verbose);

    /**
     * @brief Optimize a function with bounds
     *
     * @tparam T Type of parameters to optimize
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
             bool minimize = false);

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
        bool allow_repetition = false, bool minimize = false);

    /**
     * @brief Get the evolution history
     *
     * @return df::Dataframe with columns: "generation", "best_fitness",
     * "avg_fitness", "worst_fitness", "diversity"
     */
    df::Dataframe get_evolution_history() const;

    /**
     * @brief Get metrics about the final population
     *
     * @return std::map<std::string, double> with keys: "best_fitness",
     * "avg_fitness", "worst_fitness", "diversity", "generations",
     * "time_seconds"
     */
    std::map<std::string, double> get_population_metrics() const;

    /**
     * @brief Set a callback function to be called after each generation
     *
     * @param callback Function that takes (generation, best_fitness,
     * avg_fitness, diversity)
     * @return Reference to this object for method chaining
     */
    GeneticAlgorithm &setGenerationCallback(
        std::function<void(size_t, double, double, double)> callback);

    /**
     * @brief Set a custom crossover function
     *
     * @tparam T Type of elements in individuals
     * @param crossover_func Function that takes two parent individuals and
     * returns two child individuals
     * @return Reference to this object for method chaining
     */
    template <typename T>
    GeneticAlgorithm &setCustomCrossoverFunction(
        std::function<std::pair<df::Serie<T>, df::Serie<T>>(
            const df::Serie<T> &, const df::Serie<T> &)>
            crossover_func);

    /**
     * @brief Set a custom mutation function
     *
     * @tparam T Type of elements in individuals
     * @param mutation_func Function that takes an individual and returns a
     * mutated individual
     * @return Reference to this object for method chaining
     */
    template <typename T>
    GeneticAlgorithm &setCustomMutationFunction(
        std::function<df::Serie<T>(const df::Serie<T> &)> mutation_func);

  private:
    // Configuration parameters
    size_t population_size_;
    double crossover_rate_;
    double mutation_rate_;
    size_t elite_count_;
    size_t max_generations_;
    SelectionMethod selection_method_;
    CrossoverMethod crossover_method_;
    MutationMethod mutation_method_;
    size_t tournament_size_;
    bool verbose_;

    // Random number generator
    std::mt19937 rng_;

    // Evolution history
    df::Dataframe evolution_history_;

    // Custom callback function
    std::function<void(size_t, double, double, double)> generation_callback_;

    // Custom operator functions (type-erased via void*)
    void *custom_crossover_function_ = nullptr;
    void *custom_mutation_function_ = nullptr;

    // Time tracking
    double optimization_time_seconds_ = 0.0;

    // Helper methods for selection
    template <typename T>
    size_t tournamentSelection(const std::vector<df::Serie<T>> &population,
                               const std::vector<double> &fitness_values,
                               bool minimize);

    template <typename T>
    size_t rouletteSelection(const std::vector<df::Serie<T>> &population,
                             const std::vector<double> &fitness_values,
                             bool minimize);

    template <typename T>
    size_t rankSelection(const std::vector<df::Serie<T>> &population,
                         const std::vector<double> &fitness_values,
                         bool minimize);

    // Helper methods for crossover
    template <typename T>
    std::pair<df::Serie<T>, df::Serie<T>>
    singlePointCrossover(const df::Serie<T> &parent1,
                         const df::Serie<T> &parent2);

    template <typename T>
    std::pair<df::Serie<T>, df::Serie<T>>
    twoPointCrossover(const df::Serie<T> &parent1, const df::Serie<T> &parent2);

    template <typename T>
    std::pair<df::Serie<T>, df::Serie<T>>
    uniformCrossover(const df::Serie<T> &parent1, const df::Serie<T> &parent2);

    template <typename T>
    std::pair<df::Serie<T>, df::Serie<T>>
    arithmeticCrossover(const df::Serie<T> &parent1,
                        const df::Serie<T> &parent2);

    template <typename T>
    std::pair<df::Serie<T>, df::Serie<T>>
    orderBasedCrossover(const df::Serie<T> &parent1,
                        const df::Serie<T> &parent2);

    // Helper methods for mutation
    template <typename T>
    df::Serie<T> gaussianMutation(const df::Serie<T> &individual,
                                  const df::Serie<T> &lower_bounds,
                                  const df::Serie<T> &upper_bounds);

    template <typename T>
    df::Serie<T> uniformMutation(const df::Serie<T> &individual,
                                 const df::Serie<T> &lower_bounds,
                                 const df::Serie<T> &upper_bounds);

    template <typename T>
    df::Serie<T> swapMutation(const df::Serie<T> &individual);

    template <typename T>
    df::Serie<T> inversionMutation(const df::Serie<T> &individual);

    template <typename T>
    df::Serie<T> scrambleMutation(const df::Serie<T> &individual);

    // Helper method to calculate population diversity
    template <typename T>
    double calculateDiversity(const std::vector<df::Serie<T>> &population);

    // Helper method to generate random individual for numerical optimization
    template <typename T>
    df::Serie<T> generateRandomIndividual(const df::Serie<T> &lower_bounds,
                                          const df::Serie<T> &upper_bounds);

    // Helper method to generate random individual for combinatorial
    // optimization
    template <typename T>
    df::Serie<T> generateRandomCombinatorial(const df::Serie<T> &candidate_set,
                                             size_t solution_length,
                                             bool allow_repetition);
};

// Helper function to create a genetic algorithm for numerical optimization
GeneticAlgorithm
create_genetic_algorithm(size_t population_size = 50,
                         double crossover_rate = 0.8,
                         double mutation_rate = 0.1, size_t elite_count = 2,
                         size_t max_generations = 100,
                         const std::string &selection_method = "tournament");

// Helper function to create a genetic algorithm for combinatorial optimization
GeneticAlgorithm create_genetic_algorithm_combinatorial(
    size_t population_size = 50, double crossover_rate = 0.8,
    double mutation_rate = 0.1, size_t elite_count = 2,
    size_t max_generations = 100,
    const std::string &selection_method = "tournament");

} // namespace ml

#include "inline/genetic_algorithm.hxx"