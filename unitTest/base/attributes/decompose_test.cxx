
#include "../../TEST.h"
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/attributes/Components.h>
#include <dataframe/attributes/Coordinates.h>
#include <dataframe/attributes/Manager.h>

TEST(Decompose, scalars)
{
    df::Serie<Vector3> positions = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
    df::Serie<Stress3D> stresses = { { 1, 2, 3, 4, 5, 6 }, { 7, 8, 9, 10, 11, 12 } };
    df::Serie<Matrix4D> truc = { { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 },
        { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } };

    df::Dataframe dataframe;
    dataframe.add("P", positions);
    dataframe.add("S", stresses);
    dataframe.add("T", truc);

    df::attributes::Manager manager(dataframe);
    manager.addDecomposer(df::attributes::Components());

    for (auto name : manager.getNames(df::attributes::DecompDimension::Scalar)) {
        std::cerr << name << std::endl;
    }

    std::vector<std::string> expected_scalars(
        { "Px", "Py", "Pz", "Sxx", "Sxy", "Sxz", "Syy", "Syz", "Szz", "T11", "T12", "T13", "T14",
            "T21", "T22", "T23", "T24", "T31", "T32", "T33", "T34", "T41", "T42", "T43", "T44" });
    auto scalars = manager.getNames(df::attributes::DecompDimension::Scalar);
    //CONTAINS(scalars, expected_scalars);

    // Access decomposed attributes
    // auto Px = manager.getSerie<double>("Px");
    // auto Sxx = manager.getSerie<double>("Sxx");
    // auto T42 = manager.getSerie<double>("T42");
}

TEST(Decompose, vectors)
{
    df::Serie<Vector3> positions = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
    df::Serie<Stress3D> stresses = { { 1, 2, 3, 4, 5, 6 }, { 7, 8, 9, 10, 11, 12 } };
    df::Serie<Matrix4D> truc = { { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 },
        { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } };

    df::Dataframe dataframe;
    dataframe.add("P", positions);
    dataframe.add("S", stresses);
    dataframe.add("T", truc);

    df::attributes::Manager manager(dataframe);
    manager.addDecomposer(df::attributes::Components());

    // Get all available vector attribute names
    for (auto name : manager.getNames(df::attributes::DecompDimension::Vector)) {
        std::cerr << name << std::endl;
    }
}

TEST(Decompose, matrices)
{
    df::Serie<Vector3> positions = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } };
    df::Serie<Stress3D> stresses = { { 1, 2, 3, 4, 5, 6 }, { 7, 8, 9, 10, 11, 12 } };
    df::Serie<Matrix4D> truc = { { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 },
        { 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } };

    df::Dataframe dataframe;
    dataframe.add("P", positions);
    dataframe.add("S", stresses);
    dataframe.add("T", truc);

    df::attributes::Manager manager(dataframe);
    manager.addDecomposer(df::attributes::Components());

    for (auto name : manager.getNames(df::attributes::DecompDimension::Matrix)) {
        std::cerr << name << std::endl;
    }
}

RUN_TESTS();