#include "TEST.h"
#include <array>
#include <complex>
#include <dataframe/meta.h>
#include <dataframe/math/add.h>
// #include <dataframe/math/sub.h>
#include <vector>

using namespace df;

TEST(add, opp) {
    MSG("Testing series addition");

    // Test integer addition
    Serie<int> s1{1, 2, 3, 4};
    Serie<int> s2{10, 20, 30, 40};
    auto result = add(s1, s2);
    EXPECT_ARRAY_EQ(result.asArray(), std::vector<int>({11, 22, 33, 44}));

    // Test float addition
    Serie<float> f1{1.5f, 2.5f, 3.5f};
    Serie<float> f2{0.5f, 1.5f, 2.5f};
    auto float_result = add(f1, f2);
    EXPECT_ARRAY_NEAR(float_result.asArray(),
                      std::vector<float>({2.0f, 4.0f, 6.0f}), 1e-6f);

    // Test mixed type addition
    Serie<int> i1{1, 2, 3};
    Serie<double> d1{0.5, 1.5, 2.5};
    auto mixed_result = add(i1, d1);
    EXPECT_ARRAY_NEAR(mixed_result.asArray(),
                      std::vector<double>({1.5, 3.5, 5.5}), 1e-6);

    // Test error on size mismatch
    Serie<int> s3{1, 2};
    EXPECT_THROW(add(s1, s3), std::runtime_error);

    // Test pipeline operation
    auto result_pipe = s1 | bind_add(s2);
    EXPECT_ARRAY_EQ(result_pipe.asArray(), std::vector<int>({11, 22, 33, 44}));
}

// ---------------------------------------

// Custom vector type example
struct MyVector2 {
    double x, y;

    MyVector2(double x_ = 0, double y_ = 0) : x(x_), y(y_) {}

    MyVector2 operator+(const MyVector2 &other) const {
        return MyVector2(x + other.x, y + other.y);
    }

    bool operator==(const MyVector2 &other) const {
        return x == other.x && y == other.y;
    }
};

TEST(add, complex) {
    MSG("Testing scalar addition");

    // Integer addition
    Serie<int> s1{1, 2, 3};
    Serie<int> s2{4, 5, 6};
    auto int_result = add(s1, s2);
    EXPECT_ARRAY_EQ(int_result.asArray(), std::vector<int>({5, 7, 9}));

    // Mixed numeric types
    Serie<double> d1{1.5, 2.5, 3.5};
    Serie<float> f1{0.5f, 1.5f, 2.5f};
    auto mixed_result = add(d1, f1);
    EXPECT_ARRAY_NEAR(mixed_result.asArray(),
                      std::vector<double>({2.0, 4.0, 6.0}), 1e-6);

    // Complex numbers
    Serie<std::complex<double>> c1{{1.0, 1.0}, {2.0, 2.0}};
    Serie<std::complex<double>> c2{{1.0, -1.0}, {-2.0, 2.0}};
    auto complex_result = add(c1, c2);
    std::vector<std::complex<double>> expected_complex{{2.0, 0.0}, {0.0, 4.0}};
    for (size_t i = 0; i < complex_result.size(); ++i) {
        EXPECT_NEAR(complex_result[i].real(), expected_complex[i].real(),
                    1e-10);
        EXPECT_NEAR(complex_result[i].imag(), expected_complex[i].imag(),
                    1e-10);
    }
}

TEST(add, BasicScalarAddition) {
    MSG("Testing basic scalar addition");

    // Integer addition
    Serie<int> s1{1, 2, 3};
    Serie<int> s2{4, 5, 6};
    auto int_result = add(s1, s2);
    EXPECT_ARRAY_EQ(int_result.asArray(), std::vector<int>({5, 7, 9}));

    // Mixed numeric types
    Serie<double> d1{1.5, 2.5, 3.5};
    Serie<float> f1{0.5f, 1.5f, 2.5f};
    auto mixed_result = add(d1, f1);
    EXPECT_ARRAY_NEAR(mixed_result.asArray(),
                      std::vector<double>({2.0, 4.0, 6.0}), 1e-6);

    // Complex numbers
    Serie<std::complex<double>> c1{{1.0, 1.0}, {2.0, 2.0}};
    Serie<std::complex<double>> c2{{1.0, -1.0}, {-2.0, 2.0}};
    auto complex_result = add(c1, c2);
    std::vector<std::complex<double>> expected_complex{{2.0, 0.0}, {0.0, 4.0}};
    for (size_t i = 0; i < complex_result.size(); ++i) {
        EXPECT_NEAR(complex_result[i].real(), expected_complex[i].real(),
                    1e-10);
        EXPECT_NEAR(complex_result[i].imag(), expected_complex[i].imag(),
                    1e-10);
    }
}

TEST(add, StdArrayAddition) {
    MSG("Testing std::array addition");

    // Fixed size array
    using Vector3D = std::array<double, 3>;
    Serie<Vector3D> v1{Vector3D{1.0, 0.0, 0.0}, Vector3D{0.0, 1.0, 0.0}};
    Serie<Vector3D> v2{Vector3D{0.0, 1.0, 0.0}, Vector3D{1.0, 0.0, 1.0}};

    auto array_result = add(v1, v2);
    Serie<Vector3D> expected_array{Vector3D{1.0, 1.0, 0.0},
                                   Vector3D{1.0, 1.0, 1.0}};

    for (size_t i = 0; i < array_result.size(); ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_NEAR(array_result[i][j], expected_array[i][j], 1e-10);
        }
    }
}

TEST(add, StdVectorAddition) {
    MSG("Testing std::vector addition");

    // Dynamic size vectors
    Serie<std::vector<double>> v1{std::vector<double>{1.0, 2.0, 3.0},
                                  std::vector<double>{4.0, 5.0, 6.0}};
    Serie<std::vector<double>> v2{std::vector<double>{0.1, 0.2, 0.3},
                                  std::vector<double>{0.4, 0.5, 0.6}};

    auto vector_result = add(v1, v2);
    std::vector<std::vector<double>> expected_vector{{1.1, 2.2, 3.3},
                                                     {4.4, 5.5, 6.6}};

    for (size_t i = 0; i < vector_result.size(); ++i) {
        EXPECT_EQ(vector_result[i].size(), expected_vector[i].size());
        for (size_t j = 0; j < vector_result[i].size(); ++j) {
            EXPECT_NEAR(vector_result[i][j], expected_vector[i][j], 1e-10);
        }
    }

    // Test different size vectors
    Serie<std::vector<double>> v3{std::vector<double>{1.0, 2.0},
                                  std::vector<double>{3.0, 4.0, 5.0}};
    EXPECT_THROW(add(v1, v3), std::runtime_error);
}

TEST(add, MyVector2) {
    MSG("Testing custom type addition");

    // Custom MyVector2 type
    Serie<MyVector2> vec1{MyVector2(1.0, 2.0), MyVector2(3.0, 4.0)};
    Serie<MyVector2> vec2{MyVector2(0.1, 0.2), MyVector2(0.3, 0.4)};

    auto custom_result = add(vec1, vec2);
    Serie<MyVector2> expected_custom{MyVector2(1.1, 2.2), MyVector2(3.3, 4.4)};

    for (size_t i = 0; i < custom_result.size(); ++i) {
        EXPECT_NEAR(custom_result[i].x, expected_custom[i].x, 1e-10);
        EXPECT_NEAR(custom_result[i].y, expected_custom[i].y, 1e-10);
    }
}

// ------------------------------------------------

class Matrix2x2 {
  public:
    using value_type = std::array<double, 4>;

    Matrix2x2() = default;
    Matrix2x2(const value_type &data) : data_(data) {}

    Matrix2x2 operator+(const Matrix2x2 &other) const {
        std::array<double, 4> result;
        for (size_t i = 0; i < 4; ++i) {
            result[i] = data_[i] + other.data_[i];
        }
        return Matrix2x2(result);
    }

    bool operator==(const Matrix2x2 &other) const {
        return data_ == other.data_;
    }

    const value_type &data() const { return data_; }

  private:
    value_type data_;
};

TEST(add, Matrix2x2) {
    // Custom Matrix type
    Serie<Matrix2x2> m1{Matrix2x2({1.0, 2.0, 3.0, 4.0}),
                        Matrix2x2({5.0, 6.0, 7.0, 8.0})};
    Serie<Matrix2x2> m2{Matrix2x2({0.1, 0.2, 0.3, 0.4}),
                        Matrix2x2({0.5, 0.6, 0.7, 0.8})};

    auto matrix_result = add(m1, m2);
    Serie<Matrix2x2> expected_matrix{Matrix2x2({1.1, 2.2, 3.3, 4.4}),
                                     Matrix2x2({5.5, 6.6, 7.7, 8.8})};

    for (size_t i = 0; i < matrix_result.size(); ++i) {
        for (size_t j = 0; j < 4; ++j) {
            EXPECT_NEAR(matrix_result[i].data()[j],
                        expected_matrix[i].data()[j], 1e-10);
        }
    }
}

TEST(add, Vector4D) {
    MSG("Testing Vector4D");

    // 4D vectors
    using Vector4D = std::array<double, 4>;
    Serie<Vector4D> v4d1{Vector4D{1, 2, 3, 4}, Vector4D{5, 6, 7, 8}};
    Serie<Vector4D> v4d2{Vector4D{0.1, 0.2, 0.3, 0.4},
                         Vector4D{0.5, 0.6, 0.7, 0.8}};
    auto result4d = add(v4d1, v4d2);

    EXPECT_ARRAY_NEAR(result4d[0], Vector4D({1.1, 2.2, 3.3, 4.4}), 1e-10);
    EXPECT_ARRAY_NEAR(result4d[1], Vector4D({5.5, 6.6, 7.7, 8.8}), 1e-10);
}

TEST(add, vector_double) {
    MSG("Testing std::vector addition");

    Serie<std::vector<double>> v1{std::vector<double>{1.0, 2.0, 3.0},
                                  std::vector<double>{4.0, 5.0, 6.0}};
    Serie<std::vector<double>> v2{std::vector<double>{0.1, 0.2, 0.3},
                                  std::vector<double>{0.4, 0.5, 0.6}};

    auto result = add(v1, v2);

    EXPECT_ARRAY_NEAR(result[0], std::vector<double>({1.1, 2.2, 3.3}), 1e-10);
    EXPECT_ARRAY_NEAR(result[1], std::vector<double>({4.4, 5.5, 6.6}), 1e-10);

    // Different sized vectors should throw
    Serie<std::vector<double>> v3{std::vector<double>{1.0, 2.0},
                                  std::vector<double>{4.0, 5.0}};
    EXPECT_THROW(add(v1, v3), std::runtime_error);
}

// --------------------------------------------------

struct MyVector3 {
    using value_type = std::array<double, 3>;
    value_type data;

    MyVector3(double x = 0, double y = 0, double z = 0) : data{x, y, z} {}

    MyVector3 operator+(const MyVector3 &other) const {
        return MyVector3(data[0] + other.data[0], data[1] + other.data[1],
                         data[2] + other.data[2]);
    }

    double &operator[](size_t i) { return data[i]; }
    const double &operator[](size_t i) const { return data[i]; }
    size_t size() const { return 3; }
};

// Custom dynamic-size vector type
class DynamicVector {
  public:
    using value_type = std::vector<double>;

    // Default constructor creates an empty vector
    DynamicVector() : data_() {}

    // Copy constructor
    DynamicVector(const DynamicVector &other) = default;

    // Constructor from vector
    explicit DynamicVector(const value_type &data) : data_(data) {}

    // Constructor from initializer list - critical for test cases
    DynamicVector(std::initializer_list<double> init)
        : data_(init.begin(), init.end()) {}

    // Addition operator
    DynamicVector operator+(const DynamicVector &other) const {
        if (data_.size() != other.data_.size()) {
            throw std::runtime_error("Vector sizes must match for addition");
        }
        value_type result;
        result.reserve(data_.size());
        for (size_t i = 0; i < data_.size(); ++i) {
            result.push_back(data_[i] + other.data_[i]);
        }
        return DynamicVector(result);
    }

    // Assignment operator
    DynamicVector &operator=(const DynamicVector &) = default;

    // Index operator with bounds checking
    double &operator[](size_t i) {
        if (i >= data_.size()) {
            throw std::runtime_error(
                format("Index out of bounds: ", i, " >= ", data_.size()));
        }
        return data_[i];
    }

    const double &operator[](size_t i) const {
        if (i >= data_.size()) {
            throw std::runtime_error(
                format("Index out of bounds: ", i, " >= ", data_.size()));
        }
        return data_[i];
    }

    // Size accessor
    size_t size() const { return data_.size(); }

    // Data accessor (useful for debugging)
    const value_type &data() const { return data_; }

  private:
    value_type data_;
};

TEST(Serie, MyVector3) {
    // Fixed-size custom vector
    Serie<MyVector3> vec1{MyVector3(1.0, 2.0, 3.0), MyVector3(4.0, 5.0, 6.0)};
    Serie<MyVector3> vec2{MyVector3(0.1, 0.2, 0.3), MyVector3(0.4, 0.5, 0.6)};

    MSG("  MyVector3");
    auto fixed_result = add(vec1, vec2);
    EXPECT_ARRAY_NEAR(fixed_result[0].data,
                      (std::array<double, 3>{1.1, 2.2, 3.3}), 1e-10);
    EXPECT_ARRAY_NEAR(fixed_result[1].data,
                      (std::array<double, 3>{4.4, 5.5, 6.6}), 1e-10);
}

// TEST(Serie, DynamicVector) {
//     Serie<DynamicVector> dyn1{
//         DynamicVector{1.0, 2.0, 3.0},
//         DynamicVector{4.0, 5.0, 6.0}
//     };

//     Serie<DynamicVector> dyn2{
//         DynamicVector{0.1, 0.2, 0.3},
//         DynamicVector{0.4, 0.5, 0.6}
//     };
                              
//     auto dyn_result = add(dyn1, dyn2);

//     // Verify the results
//     std::vector<double> expected1{1.1, 2.2, 3.3};
//     std::vector<double> expected2{4.4, 5.5, 6.6};

//     EXPECT_EQ(dyn_result.size(), 2);
    
//     // Check first vector
//     for (size_t j = 0; j < 3; ++j) {
//         EXPECT_NEAR(dyn_result[0][j], expected1[j], 1e-10);
//     }
    
//     // Check second vector
//     for (size_t j = 0; j < 3; ++j) {
//         EXPECT_NEAR(dyn_result[1][j], expected2[j], 1e-10);
//     }
// }

TEST(Serie, MixedTypeAddition) {
    MSG("Testing addition between different container types");

    // std::array with custom MyVector3
    using Array3D = std::array<double, 3>;
    Serie<Array3D> arr1{Array3D{1.0, 2.0, 3.0}, Array3D{4.0, 5.0, 6.0}};
    Serie<MyVector3> vec1{MyVector3(0.1, 0.2, 0.3), MyVector3(0.4, 0.5, 0.6)};

    auto mixed_result = add(arr1, vec1);
    EXPECT_ARRAY_NEAR(mixed_result[0], Array3D({1.1, 2.2, 3.3}), 1e-10);
    EXPECT_ARRAY_NEAR(mixed_result[1], Array3D({4.4, 5.5, 6.6}), 1e-10);

    // std::vector with DynamicVector
    Serie<std::vector<double>> std_vec{std::vector<double>{1.0, 2.0},
                                       std::vector<double>{3.0, 4.0}};
    Serie<DynamicVector> dyn_vec{DynamicVector{0.1, 0.2},
                                 DynamicVector{0.3, 0.4}};

    auto mixed_dyn_result = add(std_vec, dyn_vec);
    EXPECT_ARRAY_NEAR(mixed_dyn_result[0], std::vector<double>({1.1, 2.2}),
                      1e-10);
    EXPECT_ARRAY_NEAR(mixed_dyn_result[1], std::vector<double>({3.3, 4.4}),
                      1e-10);
}

TEST(Serie, PipelineOperations) {
    MSG("Testing pipeline operations with different types");

    using Vector3D = std::array<double, 3>;
    Serie<Vector3D> v1{Vector3D{1.0, 2.0, 3.0}, Vector3D{4.0, 5.0, 6.0}};
    Serie<Vector3D> v2{Vector3D{0.1, 0.2, 0.3}, Vector3D{0.4, 0.5, 0.6}};

    auto result = v1 | bind_add(v2);
    EXPECT_ARRAY_NEAR(result[0], Vector3D({1.1, 2.2, 3.3}), 1e-10);
    EXPECT_ARRAY_NEAR(result[1], Vector3D({4.4, 5.5, 6.6}), 1e-10);

    // Chain multiple additions
    auto chain_result = v1 | bind_add(v2) | bind_add(v2);
    EXPECT_ARRAY_NEAR(chain_result[0], Vector3D({1.2, 2.4, 3.6}), 1e-10);
    EXPECT_ARRAY_NEAR(chain_result[1], Vector3D({4.8, 6.0, 7.2}), 1e-10);
}

RUN_TESTS()