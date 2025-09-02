#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/core/forEach.h>
#include <dataframe/core/map.h>
#include <dataframe/core/pipe.h>
#include <dataframe/io/csv.h>
#include <dataframe/math/random.h>
#include <dataframe/ml/genetic_algorithm.h>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// Structure to represent a city
struct City {
    std::string name;
    double x; // x-coordinate
    double y; // y-coordinate
};

// Function to calculate Euclidean distance between two cities
double distance(const City &city1, const City &city2) {
    double dx = city1.x - city2.x;
    double dy = city1.y - city2.y;
    return std::sqrt(dx * dx + dy * dy);
}

int main() {
    // Define cities (name, x, y coordinates)
    std::vector<City> cities = {
        {"New York", 40.7128, -74.0060},    {"Los Angeles", 34.0522, -118.2437},
        {"Chicago", 41.8781, -87.6298},     {"Houston", 29.7604, -95.3698},
        {"Phoenix", 33.4484, -112.0740},    {"Philadelphia", 39.9526, -75.1652},
        {"San Antonio", 29.4241, -98.4936}, {"San Diego", 32.7157, -117.1611},
        {"Dallas", 32.7767, -96.7970},      {"San Jose", 37.3382, -121.8863}};

    // Calculate distance matrix
    std::vector<std::vector<double>> distance_matrix(
        cities.size(), std::vector<double>(cities.size()));
    for (size_t i = 0; i < cities.size(); ++i) {
        for (size_t j = 0; j < cities.size(); ++j) {
            distance_matrix[i][j] = distance(cities[i], cities[j]);
        }
    }

    // Print distance matrix
    std::cout << "Distance Matrix:" << std::endl;
    std::cout << std::setw(15) << "";
    for (size_t i = 0; i < cities.size(); ++i) {
        std::cout << std::setw(15) << cities[i].name;
    }
    std::cout << std::endl;

    for (size_t i = 0; i < cities.size(); ++i) {
        std::cout << std::setw(15) << cities[i].name;
        for (size_t j = 0; j < cities.size(); ++j) {
            std::cout << std::setw(15) << std::fixed << std::setprecision(2)
                      << distance_matrix[i][j];
        }
        std::cout << std::endl;
    }

    // Create city indices (0 to cities.size()-1)
    std::vector<int> indices;
    for (size_t i = 0; i < cities.size(); ++i) {
        indices.push_back(static_cast<int>(i));
    }

    // Create a Serie of indices
    df::Serie<int> city_indices(indices);

    // Define the fitness function (total tour distance)
    auto fitness_function = [&cities,
                             &distance_matrix](const df::Serie<int> &route) {
        double total_distance = 0.0;

        // Calculate route distance
        for (size_t i = 0; i < route.size() - 1; ++i) {
            int city1 = route[i];
            int city2 = route[i + 1];
            total_distance += distance_matrix[city1][city2];
        }

        // Add distance from last city back to first city (complete the tour)
        total_distance += distance_matrix[route[route.size() - 1]][route[0]];

        return total_distance;
    };

    // Create a genetic algorithm for combinatorial optimization
    ml::GeneticAlgorithm ga = ml::create_genetic_algorithm_combinatorial(
        100,         // population_size
        0.8,         // crossover_rate
        0.2,         // mutation_rate
        5,           // elite_count (keep the 5 best solutions)
        200,         // max_generations
        "tournament" // selection_method
    );

    // Use order-based crossover and inversion mutation for TSP
    ga.setCrossoverMethod("order_based");
    ga.setMutationMethod("inversion");

    // Enable verbose output
    ga.setVerbose(true);

    // Set generation callback to track progress
    ga.setGenerationCallback([](size_t generation, double best_fitness,
                                double avg_fitness, double diversity) {
        // Only print every 10 generations to reduce output
        if (generation % 10 == 0) {
            std::cout << "Generation " << generation
                      << ": Best distance = " << best_fitness
                      << ", Diversity = " << diversity << std::endl;
        }
    });

    std::cout << "\nSolving TSP with " << cities.size()
              << " cities using Genetic Algorithm...\n"
              << std::endl;

    // Run the optimization (minimize total distance)
    auto [best_route, best_distance] = ga.optimize_combinatorial<int>(
        fitness_function, // Fitness function
        city_indices,     // Candidate set (city indices)
        cities.size(),    // Solution length (all cities)
        false,            // allow_repetition (false for TSP)
        true              // minimize (we want to minimize distance)
    );

    // Print results
    std::cout << "\nBest route found:" << std::endl;
    std::cout << "----------------" << std::endl;

    for (size_t i = 0; i < best_route.size(); ++i) {
        int city_idx = best_route[i];
        std::cout << cities[city_idx].name;
        if (i < best_route.size() - 1) {
            std::cout << " -> ";
        }
    }
    std::cout << " -> " << cities[best_route[0]].name << " (return to start)"
              << std::endl;

    std::cout << "\nTotal distance: " << best_distance << std::endl;

    // Print metrics
    auto metrics = ga.get_population_metrics();
    std::cout << "\nOptimization metrics:" << std::endl;
    std::cout << "--------------------" << std::endl;
    std::cout << "Generations: " << metrics["generations"] << std::endl;
    std::cout << "Final diversity: " << metrics["diversity"] << std::endl;
    std::cout << "Time: " << metrics["time_seconds"] << " seconds" << std::endl;

    return 0;
}