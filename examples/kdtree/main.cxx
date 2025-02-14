#include <dataframe/Serie.h>
#include <dataframe/geo/utils/kdtree.h>
#include <dataframe/types.h>
#include <dataframe/utils/unzip.h>
#include <dataframe/utils/zip.h>
#include <iomanip>
#include <iostream>

// Example 1: 2D City Data
void example_1() {
    // Create sample data for cities with population and temperature
    df::Serie<std::string> cityNames{"New York", "London", "Paris",  "Tokyo",
                                     "Sydney",   "Berlin", "Moscow", "Beijing",
                                     "Dubai",    "Mumbai"};

    using CityData = std::tuple<int, double>;
    df::Serie<CityData> cityData{
        std::make_tuple(8400000, 15.5), // Population, Avg Temperature
        std::make_tuple(8900000, 11.3),  std::make_tuple(2100000, 12.3),
        std::make_tuple(9200000, 15.4),  std::make_tuple(5300000, 17.7),
        std::make_tuple(3700000, 9.8),   std::make_tuple(12500000, 5.8),
        std::make_tuple(21500000, 12.9), std::make_tuple(3300000, 27.1),
        std::make_tuple(20400000, 27.2)};

    // City coordinates (latitude, longitude simplified to 2D for example)
    df::Serie<Vector2> cityPositions{
        {40.7, -74.0},  // New York
        {51.5, -0.1},   // London
        {48.9, 2.3},    // Paris
        {35.7, 139.7},  // Tokyo
        {-33.9, 151.2}, // Sydney
        {52.5, 13.4},   // Berlin
        {55.8, 37.6},   // Moscow
        {39.9, 116.4},  // Beijing
        {25.2, 55.3},   // Dubai
        {19.1, 72.9}    // Mumbai
    };

    // Combine names with data
    auto combinedData = df::zip(cityNames, cityData);

    // Create KDTree
    using KDTree = df::KDTree<std::tuple<std::string, CityData>, 2>;
    KDTree tree2d(combinedData, cityPositions);

    // Find k nearest cities to a point
    KDTree::point_t queryPoint{45.0, 10.0}; // Somewhere in Europe
    size_t k = 3;

    std::cout << "Finding " << k << " nearest cities to coordinates ("
              << queryPoint[0] << ", " << queryPoint[1] << "):\n\n";

    auto nearestCities = tree2d.findNearest({queryPoint}, k);

    for (const auto &[index, cityInfo] : nearestCities) {
        const auto &[name, data] = cityInfo;
        const auto &[population, temperature] = data;
        const auto &position = cityPositions[index];

        std::cout << "City: " << std::left << std::setw(10) << name
                  << " | Position: (" << std::fixed << std::setprecision(1)
                  << position[0] << ", " << position[1] << ")"
                  << " | Population: " << std::setw(10) << population
                  << " | Temp: " << temperature << "°C\n";
    }
}

// Example 2: 3D Point Cloud with Material Properties
void example_2() {
    struct MaterialProperties {
        double density;
        double conductivity;
        std::string material;

        MaterialProperties(double d, double c, std::string m)
            : density(d), conductivity(c), material(std::move(m)) {}
    };

    // Create sample data for material points
    df::Serie<MaterialProperties> materials{
        MaterialProperties(7.87, 80.2, "Steel"),
        MaterialProperties(2.70, 235.0, "Aluminum"),
        MaterialProperties(8.96, 401.0, "Copper"),
        MaterialProperties(19.32, 318.0, "Gold"),
        MaterialProperties(11.34, 35.3, "Lead")};

    // 3D positions of material samples
    df::Serie<Vector3> materialPositions{{0.0, 0.0, 0.0},
                                         {1.0, 1.0, 1.0},
                                         {2.0, 0.0, 2.0},
                                         {0.0, 2.0, 2.0},
                                         {1.0, 1.0, 0.0}};

    // Create 3D KDTree
    df::KDTree<MaterialProperties, 3> tree3d(materials, materialPositions);

    // Find k nearest materials to a 3D point
    Vector3 queryPoint3d{1.0, 0.5, 1.0};
    size_t k = 2;

    std::cout << "\nFinding " << k << " nearest materials to point ("
              << queryPoint3d[0] << ", " << queryPoint3d[1] << ", "
              << queryPoint3d[2] << "):\n\n";

    auto nearestMaterial = tree3d.findNearest(queryPoint3d);
    auto nearestMaterials = tree3d.findNearest({queryPoint3d}, k);

    for (const auto &[index, material] : nearestMaterials) {
        const auto &position = materialPositions[index];

        std::cout << "Material: " << std::left << std::setw(10)
                  << material.material << " | Position: (" << std::fixed
                  << std::setprecision(1) << position[0] << ", " << position[1]
                  << ", " << position[2] << ")"
                  << " | Density: " << material.density
                  << " | Conductivity: " << material.conductivity << "\n";
    }
}

int main() {
    example_1();
    example_2();
    return 0;
}