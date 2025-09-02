#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/ml/bee_algorithm.h>
#include <iomanip>
#include <iostream>
#include <vector>

// Define some test functions for optimization
namespace test_functions {

// Sphere function (minimum at [0, 0, ..., 0])
double sphere(const df::Serie<double> &x) {
    double sum = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        sum += x[i] * x[i];
    }
    return sum;
}

// Rosenbrock function (minimum at [1, 1, ..., 1])
double rosenbrock(const df::Serie<double> &x) {
    double sum = 0.0;
    for (size_t i = 0; i < x.size() - 1; ++i) {
        sum += 100.0 * std::pow(x[i + 1] - x[i] * x[i], 2) +
               std::pow(1.0 - x[i], 2);
    }
    return sum;
}

// Rastrigin function (minimum at [0, 0, ..., 0])
double rastrigin(const df::Serie<double> &x) {
    double A = 10.0;
    double sum = A * x.size();
    for (size_t i = 0; i < x.size(); ++i) {
        sum += x[i] * x[i] - A * std::cos(2 * M_PI * x[i]);
    }
    return sum;
}

// Ackley function (minimum at [0, 0, ..., 0])
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

} // namespace test_functions

// Function to compare different optimization algorithms
void compare_optimization(size_t dimension, size_t runs) {
    std::cout << "=== Comparison of Bee Algorithm on benchmark functions ==="
              << std::endl;
    std::cout << "Dimension: " << dimension << ", Runs: " << runs << std::endl
              << std::endl;

    // Define bounds
    df::Serie<double> lower_bounds(dimension, -5.0);
    df::Serie<double> upper_bounds(dimension, 5.0);

    // Test functions
    std::vector<std::pair<std::string,
                          std::function<double(const df::Serie<double> &)>>>
        functions = {{"Sphere", test_functions::sphere},
                     {"Rosenbrock", test_functions::rosenbrock},
                     {"Rastrigin", test_functions::rastrigin},
                     {"Ackley", test_functions::ackley}};

    // Print header
    std::cout << std::left << std::setw(15) << "Function" << std::setw(15)
              << "Avg. Fitness" << std::setw(15) << "Best Fitness"
              << std::setw(15) << "Avg. Time (s)" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // Test each function
    for (const auto &[func_name, func] : functions) {
        double total_fitness = 0.0;
        double best_fitness = std::numeric_limits<double>::max();
        double total_time = 0.0;

        for (size_t run = 0; run < runs; ++run) {
            // Create Bee Algorithm
            ml::BeeAlgorithm ba(50, 25, 25, 100, 30);

            // Run optimization
            auto [solution, fitness] =
                ba.optimize(func, lower_bounds, upper_bounds, true);

            // Get metrics
            auto metrics = ba.get_population_metrics();

            // Update statistics
            total_fitness += fitness;
            best_fitness = std::min(best_fitness, fitness);
            total_time += metrics["time_seconds"];
        }

        // Calculate averages
        double avg_fitness = total_fitness / runs;
        double avg_time = total_time / runs;

        // Print results
        std::cout << std::left << std::setw(15) << func_name << std::setw(15)
                  << avg_fitness << std::setw(15) << best_fitness
                  << std::setw(15) << avg_time << std::endl;
    }
}

// Example for solving the Traveling Salesman Problem
void tsp_example() {
    std::cout << "\n=== Traveling Salesman Problem Example ===" << std::endl;

    // Define cities (x, y coordinates)
    std::vector<std::pair<double, double>> city_coords = {
        {60, 200},  {180, 200}, {80, 180},  {140, 180}, {20, 160},
        {100, 160}, {200, 160}, {140, 140}, {40, 120},  {100, 120},
        {180, 100}, {60, 80},   {120, 80},  {180, 60},  {20, 40},
        {100, 40},  {200, 40},  {20, 20},   {60, 20},   {160, 20}};

    // Create city indices
    df::Serie<int> indices = df::Serie<int>();
    for (size_t i = 0; i < city_coords.size(); ++i) {
        indices.add(static_cast<int>(i));
    }

    // Define the objective function (total distance)
    auto calculate_distance = [&city_coords](const df::Serie<int> &route) {
        double total_distance = 0.0;

        for (size_t i = 0; i < route.size() - 1; ++i) {
            const auto &[x1, y1] = city_coords[route[i]];
            const auto &[x2, y2] = city_coords[route[i + 1]];

            double dx = x2 - x1;
            double dy = y2 - y1;
            total_distance += std::sqrt(dx * dx + dy * dy);
        }

        // Return to starting city
        const auto &[x1, y1] = city_coords[route[route.size() - 1]];
        const auto &[x2, y2] = city_coords[route[0]];

        double dx = x2 - x1;
        double dy = y2 - y1;
        total_distance += std::sqrt(dx * dx + dy * dy);

        return total_distance;
    };

    // Create and configure Bee Algorithm
    ml::BeeAlgorithm ba(40, 20, 20, 200, 40, 1.0, true);

    // Solve the TSP
    auto [best_route, min_distance] = ba.optimize_combinatorial<int>(
        calculate_distance, indices, indices.size(), false, true);

    // Print results
    std::cout << "Number of cities: " << city_coords.size() << std::endl;
    std::cout << "Best route found: ";
    for (size_t i = 0; i < best_route.size(); ++i) {
        std::cout << best_route[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "Total distance: " << min_distance << std::endl;

    // Get optimization metrics
    auto metrics = ba.get_population_metrics();
    std::cout << "Cycles executed: " << metrics["cycles"] << std::endl;
    std::cout << "Time: " << metrics["time_seconds"] << " seconds" << std::endl;
}

int main() {
    // Run the comparison on benchmark functions
    compare_optimization(10, 5);

    // Run the TSP example
    tsp_example();

    return 0;
}