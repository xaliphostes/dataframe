# Traveling Salesman Problem (TSP)
The first example solves the classic Traveling Salesman Problem:

- It defines 10 cities with their coordinates (in this case, major US cities)
- Creates a distance matrix between all cities
- Sets up a fitness function that calculates the total tour distance
- Uses the `optimize_combinatorial` method with parameters specifically tuned for TSP:
  - Uses order-based crossover (ideal for permutation problems)
  - Uses inversion mutation (good for TSP specifically)
  - Sets `allow_repetition` to false since each city must be visited exactly once


# Numerical Optimization
The second example demonstrates optimization of complex mathematical functions:

- Implements three challenging test functions (Rastrigin, Rosenbrock, and Ackley)
- These functions have many local minima, making them difficult to optimize
- Uses arithmetic crossover and Gaussian mutation, which are well-suited for continuous parameter optimization
- Sets bounds for the search space and minimizes each function


# Custom Genetic Operators
The third example solves a knapsack problem with custom genetic operators:

- Defines a problem where we need to select items to maximize value while keeping weight under a limit
- Implements a custom crossover operator that prefers high value-to-weight ratio items
- Creates a custom mutation operator with a bell-curve probability centered around medium value-to-weight items
- Shows how to register these custom operators with the genetic algorithm

These examples demonstrate the flexibility of the genetic algorithm implementation and how it can be applied to diverse optimization problems. They highlight key features like:

- Handling both permutation and numerical problems
- Supporting custom genetic operators
- Tracking optimization progress
- Outputting metrics about the optimization run