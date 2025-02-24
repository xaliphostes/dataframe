
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/Manager.h>

int main() {

    // --------------------------------------------

    df::Serie<Vector3> positions = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    df::Serie<Stress3D> stresses = {{1, 2, 3, 4, 5, 6}, {7, 8, 9, 10, 11, 12}};
    df::Serie<Matrix4D> truc = {
        {1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0},
        {1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0}};

    // --------------------------------------------

    df::Dataframe dataframe;
    dataframe.add("P", positions);
    dataframe.add("S", stresses);
    dataframe.add("T", truc);
    dataframe.dump();

    // --------------------------------------------

    df::attributes::Manager manager(dataframe);
    manager.addDecomposer(df::attributes::Components());

    // --------------------------------------------

    // Get all available attribute names
    MSG("------------1-----------");
    for (auto name :
         manager.getNames(df::attributes::DecompDimension::Scalar)) {
        std::cout << name << std::endl;
    }
    MSG("------------2-----------");
    for (auto name :
         manager.getNames(df::attributes::DecompDimension::Vector)) {
        std::cout << name << std::endl;
    }
    MSG("------------3-----------");
    for (auto name :
         manager.getNames(df::attributes::DecompDimension::Matrix)) {
        std::cout << name << std::endl;
    }
    MSG("-----------------------");

    // Access decomposed attributes
    auto posX = manager.getSerie<double>("Px");
    auto stressXX = manager.getSerie<double>("Sxx");
    auto tXX = manager.getSerie<double>("Txx");

    return 0;
}
