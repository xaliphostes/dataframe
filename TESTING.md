# How to write and test the code
We use a special wrapper around the cmake test. Therefore, nothing special to include as library to write tests.

## Where to put test files
Anywhere in fact.

Well...Not really! You can put your test files anywhere except from the source directory (usually `src/`).

## How to name a test suite
A test file must finish with `_test.cxx`. Otherwise, it will not be compiled nor executed.

## How to write test suite
A test suite is composed of one or more fixtures. Concequently, it is defined by 2 names: the test suite name, ans the fixture name.

A test suite is defined in one file. All fixtures are defined in this file. Therefore, for each test suite (one file), you can define one to many fixtures:

File `my_test.cxx` (note the suffix `_test.cxx`!)
```cpp
TEST(test_suite_name, fixture_name1) {
    // my testing
}

TEST(test_suite_name, fixture_name2) {
    // my testing
}

RUN_TESTS()
```
Note the `RUN_TESTS()` call at the end of the file (see below)!

## A test suite must call RUN_TESTS()
If not, you will have a compilation error.
The call **must** be at the end of the file.

## How to run tests
Just call `ctest` ot `make test` in the build directory.