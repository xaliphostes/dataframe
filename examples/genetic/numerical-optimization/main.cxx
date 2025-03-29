#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/map.h>
#include <dataframe/ml/genetic_algorithm.h>
#include <functional>
#include <iomanip>
#include <iostream>

// Rastrigin function - a non-convex function with many local minima
// Global minimum is at (0,0,...,0) with f(x) = 0
double rastrigin(const df::Serie<double> &x) {
    double A = 10.0;
    double result = A * x.size();

    for (size_t i = 0; i < x.size(); ++i) {
        result += x[i] * x[i] - A * std::cos(2 * M_PI * x[i]);
    }

    return result;
}

// Rosenbrock function (banana function)
// Global minimum is at (1,1,...,1) with f(x) = 0
double rosenbrock(const df::Serie<double> &x) {
    double sum = 0.0;

    for (size_t i = 0; i < x.size() - 1; ++i) {
        double term1 = 100.0 * std::pow(x[i + 1] - x[i] * x[i], 2);
        double term2 = std::pow(1 - x[i], 2);
        sum += term1 + term2;
    }

    return sum;
}

// Ackley function
// Global minimum is at (0,0,...,0) with f(x) = 0
double ackley(const df::Serie<double> &x) {
    double a = 20.0;
    double b = 0.2;
    double c = 2 * M_PI;

    double sum1 = 0.0;
    double sum2 = 0.0;

    for (size_t i = 0; i < x.size(); ++i) {
        sum1 += x[i] * x[i];
        sum2 += std::cos(c * x[i]);
    }

    sum1 = -a * std::exp(-b * std::sqrt(sum1 / x.size()));
    sum2 = -std::exp(sum2 / x.size());

    return sum1 + sum2 + a + std::exp(1.0);
}

int main() {
    // Number of dimensions for the optimization problem
    const size_t dimensions = 5;

    // Create lower and upper bounds for the variables
    std::vector<double> lower_values(dimensions, -5.0);
    std::vector<double> upper_values(dimensions, 5.0);

    df::Serie<double> lower_bounds(lower_values);
    df::Serie<double> upper_bounds(upper_values);

    std::cout << "Numerical Optimization using Genetic Algorithm" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Dimensions: " << dimensions << std::endl;
    std::cout << "Search range: [" << lower_bounds[0] << ", " << upper_bounds[0]
              << "] for each dimension" << std::endl;

    // List of test functions
    std::vector<std::pair<std::string,
                          std::function<double(const df::Serie<double> &)>>>
        functions = {{"Rastrigin", rastrigin},
                     {"Rosenbrock", rosenbrock},
                     {"Ackley", ackley}};

    // Test each function
    for (const auto &[func_name, func] : functions) {
        std::cout << "\nOptimizing " << func_name << " function:" << std::endl;
        std::cout << "----------------------------" << std::endl;

        // Create a genetic algorithm
        ml::GeneticAlgorithm ga =
            ml::create_genetic_algorithm(200,         // population_size
                                         0.8,         // crossover_rate
                                         0.1,         // mutation_rate
                                         10,          // elite_count
                                         100,         // max_generations
                                         "tournament" // selection_method
            );

        // Use arithmetic crossover and gaussian mutation for numerical
        // optimization
        ga.setCrossoverMethod("arithmetic");
        ga.setMutationMethod("gaussian");

        // Optional: Set a callback to monitor progress
        size_t progress_counter = 0;
        ga.setGenerationCallback(
            [&progress_counter](size_t generation, double best_fitness,
                                double avg_fitness, double diversity) {
                // Show progress
                if (generation % 10 == 0) {
                    std::cout << ".";
                    std::cout.flush();
                    progress_counter++;
                    if (progress_counter % 10 == 0) {
                        std::cout << " " << generation
                                  << " generations (best = " << best_fitness
                                  << ")" << std::endl;
                    }
                }
            });

        // Run the optimization
        auto [best_solution, best_fitness] =
            ga.optimize(func,         // Function to minimize
                        lower_bounds, // Lower bounds
                        upper_bounds, // Upper bounds
                        true          // Minimize = true
            );

        // Print results
        std::cout << "\n\nBest solution found:" << std::endl;
        std::cout << "x = [";
        for (size_t i = 0; i < best_solution.size(); ++i) {
            std::cout << std::fixed << std::setprecision(6) << best_solution[i];
            if (i < best_solution.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "]" << std::endl;

        std::cout << "Function value: " << best_fitness << std::endl;

        // Print optimization metrics
        auto metrics = ga.get_population_metrics();
        std::cout << "Generations: " << metrics["generations"] << std::endl;
        std::cout << "Time: " << metrics["time_seconds"] << " seconds"
                  << std::endl;
        std::cout << "Final diversity: " << metrics["diversity"] << std::endl;
    }

    return 0;
}