#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/algos/harmonic_diffusion.h>
#include <dataframe/geo/mesh/grid2d_mesh.h>
#include <dataframe/math/random.h>
#include <dataframe/types.h>
#include <fstream>

using namespace df;

/*
// Example usage of harmonic diffusion with Mesh
int main() {
    // Create a simple triangulated surface (a small patch)
    std::vector<Vector3> positions = {{0, 0, 0}, {1, 0, 0}, {2, 0, 0},
                                      {0, 1, 0}, {1, 1, 0}, {2, 1, 0},
                                      {0, 2, 0}, {1, 2, 0}, {2, 2, 0}};

    std::vector<iVector3> triangles = {{0, 1, 3}, {1, 4, 3}, {1, 2, 4},
                                       {2, 5, 4}, {3, 4, 6}, {4, 7, 6},
                                       {4, 5, 7}, {5, 8, 7}};

    // Create mesh
    df::Mesh3D mesh(positions, triangles);

    // Create diffusion solver for scalar field
    df::HarmonicDiffusion<3> diffusion(mesh, 0.0);

    // Add constraints
    diffusion.constrainBorders(-1.0);        // Set border values to -1
    diffusion.addConstraint({1, 1, 0}, 1.0); // Set center point to 1

    // Set parameters
    diffusion.setMaxIter(1000);
    diffusion.setEps(1e-6);
    diffusion.setEpsilon(0.5);

    // Solve and get results
    df::Dataframe result = diffusion.solve("temperature", true, 100);
    result.dump();

    // Example with vector field
    std::vector<double> init_vector = {0.0, 0.0, 0.0};
    df::HarmonicDiffusion<3> vector_diffusion(mesh, init_vector);

    // Add vector constraints
    vector_diffusion.constrainBorders<Vector3>({-1.0, -1.0, -1.0});
    vector_diffusion.addConstraint<Vector3>({1, 1, 0}, {1.0, 1.0, 1.0});

    // Solve vector field
    df::Dataframe vector_result = vector_diffusion.solve("displacement");

    vector_result.dump();

    return 0;
}
*/

int main() {
    // Create a square grid of points (51x51 = 2601 points)
    // This will generate 2500 squares which become 5000 triangles
    const size_t n = 21;      // points per side
    const double size = 10.0; // square side length
    Mesh2D mesh = generate_grid2d_mesh(n, size);

    // Create harmonic diffusion solver
    HarmonicDiffusion<2> diffusion(mesh, 0.0);

    // Add interesting constraints
    // Border constraint
    diffusion.constrainBorders(0.0);

    // Add some hot spots
    diffusion.addConstraint({-2.0, -2.0}, 4.0); // Bottom left
    diffusion.addConstraint({2.0, 4.0}, 5.0);   // Top right
    diffusion.addConstraint({-0.0, 1.0}, -7.0); // Top left
    diffusion.addConstraint({4.0, -4.0}, -1.0); // Bottom right

    // Set parameters
    diffusion.setMaxIter(2000); // Increased for larger mesh
    diffusion.setEps(1e-6);
    diffusion.setEpsilon(0.05);

    // Solve and get results with intermediate steps recorded
    Dataframe result = diffusion.solve("temperature", true, 100);

    {
        // Print mesh statistics
        std::cout << "Mesh statistics:\n";
        std::cout << "- Number of vertices: " << mesh.vertexCount() << "\n";
        std::cout << "- Number of triangles: " << mesh.triangleCount() << "\n";
        std::cout << "- Number of border nodes: " << mesh.borderNodes().size()
                  << "\n";

        // Print result statistics
        const auto &temp = result.get<double>("temperature");
        std::cout << "\nTemperature field statistics:\n";
        std::cout << "- Number of values: " << temp.size() << "\n";
    }

    // Export the results to CSV files
    std::ofstream vertices_file("vertices.csv");
    vertices_file << "x,y\n";
    const auto &verts = mesh.vertices();
    for (size_t i = 0; i < verts.size(); ++i) {
        vertices_file << verts[i][0] << "," << verts[i][1] << "\n";
    }
    vertices_file.close();

    std::ofstream triangles_file("triangles.csv");
    triangles_file << "v1,v2,v3\n";
    const auto &tris = mesh.triangles();
    for (size_t i = 0; i < tris.size(); ++i) {
        triangles_file << static_cast<int>(tris[i][0]) << ","
                       << static_cast<int>(tris[i][1]) << ","
                       << static_cast<int>(tris[i][2]) << "\n";
    }
    triangles_file.close();

    std::ofstream temperature_file("temperature.csv");
    temperature_file << "temperature\n";
    const auto &temp = result.get<double>("temperature");
    for (size_t i = 0; i < temp.size(); ++i) {
        temperature_file << temp[i] << "\n";
    }
    temperature_file.close();

    // Export intermediate steps if they exist
    size_t step = 1;
    while (result.has("temperature" + std::to_string(step))) {
        std::ofstream step_file("temperature_step" + std::to_string(step) +
                                ".csv");
        step_file << "temperature\n";
        const auto &step_temp =
            result.get<double>("temperature" + std::to_string(step));
        for (size_t i = 0; i < step_temp.size(); ++i) {
            step_file << step_temp[i] << "\n";
        }
        step_file.close();
        step++;
    }

    return 0;
}