#include <dataframe/Serie.h>
#include <dataframe/algebra/matrices.h>
#include <dataframe/algebra/vector.h>
#include <dataframe/math/mult.h>
#include <dataframe/print.h>
#include <iostream>

double r() { return (std::rand() - 0.5) / RAND_MAX * 200.0; }

template <bool FULL> struct matrix_traits {};

template <> struct matrix_traits<true> {
    using type = df::FullMatrix<double, 3>;
};

template <> struct matrix_traits<false> {
    using type = df::SymmetricMatrix<double, 3>;
};

// ---------------------------------------------

template <bool IsFull> void run() {
    using Matrix = matrix_traits<IsFull>::type;
    using Vec = df::Vector<double, 3>;

    // -----------------------------------

    df::Serie<Matrix> m1;
    for (size_t i = 0; i < 20; ++i) {
        m1.add({{{r(), r(), r()}, {r(), r(), r()}, {r(), r(), r()}}});
    }
    df::print(m1);

    auto inv = m1.map([](const Matrix &m) { return m.inverse(); });
    df::print(inv);

    auto det = m1.map([](const Matrix &m) { return m.determinant(); });
    df::print(det);

    // -----------------------------------

    df::Serie<Matrix> m2;
    for (size_t i = 0; i < 20; ++i) {
        m2.add({{{r(), r(), r()}, {r(), r(), r()}, {r(), r(), r()}}});
    }

    auto m3 = m1 * m2;
    df::print(m3);

    // -----------------------------------

    Vec v{1, 2, 3};
    df::print( m2.map([v](const Matrix &m) { return m * v; }) );
}

int main() {

    run<true>();
    run<false>();

    return 0;
}
