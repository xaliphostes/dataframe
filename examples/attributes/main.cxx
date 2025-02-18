
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/Manager.h>

int main() {
    // Create some sample data
    df::Serie<Vector3> positions = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    df::Serie<Stress3D> stresses = {{1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12}};

    // Create a dataframe
    df::Dataframe dataframe;
    dataframe.add("positions", positions);
    dataframe.add("stresses", stresses);

    // Create manager with decomposers
    df::attributes::Manager manager(dataframe);
    manager.addDecomposer(std::make_unique<df::attributes::Coordinates>());
    manager.addDecomposer(std::make_unique<df::attributes::Components>());

    // Access decomposed attributes
    // auto posX = manager.getSerie<double>("positions_x");
    // auto stressXX = manager.getSerie<double>("stresses_0");

    // Get all available attribute names
    auto names = manager.getNames(sizeof(double));
    for (auto name: names) {
        std::cout << name << std::endl;
    }

    return 0;
}
