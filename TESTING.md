# How to write and test the code
We use a special wrapper around the cmake test. Therefore, nothing special to include or link to write and execute tests.

A test suite (given in one file) is composed of one to many fixtures (in the same file). A fixture is testing a specific functionality of the test suite.

## Where to put test files
Anywhere in fact.

Well...Not really! You can put your test files anywhere except in the source directory (usually `src/`, which is the case here).

## How to name a test suite
A test file **must finish** with `_test.cxx`. Otherwise, it will not be *detected*.

## How to write test suite
A test suite is composed of one or more fixtures. Concequently, it is defined by 2 names: the test suite name, and the fixture name.

A test suite is defined in one file. All fixtures are defined in this file. Therefore, for each test suite (one file), you can define one to many fixtures (redit, I know):

File `my_test.cxx` (note the suffix `_test.cxx`!)
```cpp
TEST(suite_name, fixture_name1) {
    // my testing
}

TEST(suite_name, fixture_name2) {
    // my testing
}

RUN_TESTS()
```
Note the `RUN_TESTS()` call at the end of the file (see below)!

## A test suite must call RUN_TESTS()
If not, you will have a compilation error.
The call **must** be at the end of the file of the test suite.

## Available templates and MACROs for testing

- `EXPECT_ARRAY_NEAR(arr1, arr2, tol)`: compare two arrays at tol
  
- `EXPECT_ARRAY_EQ(arr1, arr2)`: check that 2 arrays are equals

- `COMPARE_SERIE_VECTOR(serie, vector)`: compare a serie with a vector

- `COMPARE_SERIES(actual, expected)`: compare two series

- `TIMING(fct)`: for timing measurements of a function

- `RANDOM(size, min, max)`: to generate a serie with random numbers. Similar to df::random(n, min, max)

- `EXPECT_EQ(v1, v2)`: check that v1 == v2

- `EXPECT_GT(v1, v2)`: check that v1 > v2

- `EXPECT_LT(v1, v2)`: check that v1 < v2

- `EXPECT_NOT_EQ(v1, v2)`: check that v1 != v2
  
- `EXPECT_NEAR(v1, v2, tol)`: check that v1 is close to v2 at tol
  
- `EXPECT_THROW(statement, exception_type)`: expect the statement to throw a specific error
  
- `EXPECT_NO_THROW(statement)`: expect the statement NOT to throw a specific error
  
- `EXPECT_STREQ(s1, s2)`: check that strings s1 equals s2
  
- `EXPECT_TRUE(condition)`: check boolean expression to be true
  
- `EXPECT_FALSE(condition)`: check boolean expression to be false
  
- `CHECK(condition)`: check the condition

## How to run tests
Just call `ctest` ot `make test` in the build directory.
