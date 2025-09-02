#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/ml/genetic_algorithm.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

// Simple knapsack problem
// Each item has a weight and value, and we want to maximize total value
// while keeping the total weight below a constraint
struct Item {
    int id;
    std::string name;
    double weight;
    double value;
};

int main()
{
    // Define the knapsack problem
    std::vector<Item> items = { { 1, "Laptop", 3.0, 1500 }, { 2, "Smartphone", 0.3, 1000 },
        { 3, "Tablet", 1.0, 800 }, { 4, "Camera", 0.5, 700 }, { 5, "Headphones", 0.2, 300 },
        { 6, "External HDD", 0.7, 200 }, { 7, "Power Bank", 0.4, 150 }, { 8, "Speaker", 1.5, 400 },
        { 9, "E-reader", 0.3, 250 }, { 10, "Smartwatch", 0.1, 350 },
        { 11, "Portable Monitor", 2.0, 600 }, { 12, "Gaming Console", 2.5, 450 },
        { 13, "Wireless Earbuds", 0.1, 200 }, { 14, "Bluetooth Mouse", 0.2, 80 },
        { 15, "USB Hub", 0.3, 100 } };

    const double weight_limit = 5.0; // Weight capacity constraint

    // Create a Serie with 0/1 values (not included/included)
    df::Serie<int> candidate_set(std::vector<int> { 0, 1 });

    // Define fitness function for knapsack problem
    auto fitness_function = [&items, weight_limit](const df::Serie<int>& solution) {
        double total_weight = 0.0;
        double total_value = 0.0;

        // Sum weights and values of included items
        for (size_t i = 0; i < solution.size(); ++i) {
            if (solution[i] == 1) {
                total_weight += items[i].weight;
                total_value += items[i].value;
            }
        }

        // Penalize solutions that exceed weight limit
        if (total_weight > weight_limit) {
            // Return negative fitness to ensure invalid solutions are ranked
            // worse
            return -1000.0 + (weight_limit - total_weight);
        }

        return total_value;
    };

    // Custom crossover operator designed specifically for knapsack problems
    // This performs a weighted crossover that prefers to keep high value/weight
    // ratio items
    auto custom_crossover = [&items](const df::Serie<int>& parent1, const df::Serie<int>& parent2) {
        std::vector<int> child1_values(parent1.size());
        std::vector<int> child2_values(parent2.size());

        // Random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Calculate value/weight ratios
        std::vector<double> ratios;
        for (const auto& item : items) {
            ratios.push_back(item.value / item.weight);
        }

        // Crossover based on value/weight ratio
        for (size_t i = 0; i < parent1.size(); ++i) {
            double ratio = ratios[i];
            double preference = dis(gen); // Random value between 0 and 1

            // Higher value/weight ratio items have higher chance to inherit
            // from parent1
            if (preference < 0.5 + (ratio / 10.0)) {
                child1_values[i] = parent1[i];
                child2_values[i] = parent2[i];
            } else {
                child1_values[i] = parent2[i];
                child2_values[i] = parent1[i];
            }
        }

        return std::make_pair(df::Serie<int>(child1_values), df::Serie<int>(child2_values));
    };

    // Custom mutation operator that prefers to flip items with medium
    // value/weight ratio
    auto custom_mutation = [&items](const df::Serie<int>& individual) {
        std::vector<int> mutated_values(individual.data());

        // Random generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);

        // Calculate value/weight ratios
        std::vector<double> ratios;
        double max_ratio = 0.0;
        for (const auto& item : items) {
            double ratio = item.value / item.weight;
            ratios.push_back(ratio);
            max_ratio = std::max(max_ratio, ratio);
        }

        // Normalize ratios to [0,1]
        for (auto& ratio : ratios) {
            ratio /= max_ratio;
        }

        // Mutation probability should be a bell curve centered at medium
        // value/weight ratio This way we tend to keep very high and very low
        // value/weight items the same, but are more likely to experiment with
        // medium value items
        for (size_t i = 0; i < mutated_values.size(); ++i) {
            double mutation_prob = 0.1 * std::exp(-10 * std::pow(ratios[i] - 0.5, 2));

            if (dis(gen) < mutation_prob) {
                mutated_values[i] = 1 - mutated_values[i]; // Flip 0 to 1 or 1 to 0
            }
        }

        return df::Serie<int>(mutated_values);
    };

    // Create a genetic algorithm
    ml::GeneticAlgorithm ga(100, // population_size
        0.8, // crossover_rate
        0.1, // mutation_rate
        5, // elite_count
        100, // max_generations
        "tournament" // selection_method
    );

    // Set custom operators
    ga.setCustomCrossoverFunction<int>(custom_crossover);
    ga.setCustomMutationFunction<int>(custom_mutation);

    // Enable verbose output
    ga.setVerbose(true);

    std::cout << "Solving Knapsack Problem with " << items.size() << " items" << std::endl;
    std::cout << "Weight limit: " << weight_limit << " kg" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    // Run the optimization (maximize total value)
    auto [best_solution, best_fitness]
        = ga.optimize_combinatorial<int>(fitness_function, // Fitness function
            candidate_set, // Candidate set (0 or 1)
            items.size(), // Solution length (1 bit per item)
            true, // allow_repetition (true for binary encoding)
            false // minimize (false - we want to maximize value)
        );

    // Print results
    std::cout << "\nBest solution found:" << std::endl;
    std::cout << "-------------------" << std::endl;
    std::cout << std::left << std::setw(20) << "Item" << std::setw(10) << "Weight" << std::setw(10)
              << "Value"
              << "Selected" << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    double total_weight = 0.0;
    double total_value = 0.0;

    for (size_t i = 0; i < best_solution.size(); ++i) {
        bool selected = (best_solution[i] == 1);

        std::cout << std::left << std::setw(20) << items[i].name << std::setw(10) << items[i].weight
                  << std::setw(10) << items[i].value << (selected ? "Yes" : "No") << std::endl;

        if (selected) {
            total_weight += items[i].weight;
            total_value += items[i].value;
        }
    }

    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::left << std::setw(20) << "Total:" << std::setw(10) << total_weight
              << std::setw(10) << total_value << std::endl;

    std::cout << "\nWeight limit: " << weight_limit << " kg" << std::endl;
    std::cout << "Weight usage: " << (total_weight / weight_limit * 100) << "%" << std::endl;

    return 0;
}