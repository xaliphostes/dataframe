# TODO list

1. OPTIMIZATION (for sure)!
2. Is there a way to get rid of the difference between Array and double ?
In `Serie`, we have "two methods" to deal with this problem:
```c++
class Serie {
public:

    Array value(uint32_t i) const;   // for itemSize > 1

    double scalar(uint32_t i) const; // for itemSize = 1
};
```
If we know that the Serie is with `itemSize=1`, then we can use the second method. Otherwise, the first one have to be used.

3. The file `inferring.h` might be helpful to deal with `double` / `Array` (or not).

4. Finish the `Manager` / `Decomposer` with some examples...

5. Code coverage
See [this link](https://github.com/pyarmak/cmake-gtest-coverage-example/blob/master/cmake/modules/CodeCoverage.cmake)
See also [this link](https://www.danielsieger.com/blog/2022/03/06/code-coverage-for-cpp.html#:~:text=What's%20Code%20Coverage%3F,blocks%2C%20or%20lines%20being%20covered.)