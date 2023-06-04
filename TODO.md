# TODO list
1. Is there a way to get rid of the difference between Array and double ?
In `Serie`, we have to methods to deal with this problem:
```c++
class Serie {
public:

    Array value(uint32_t i) const;   // for itemSize > 1

    double scalar(uint32_t i) const; // for itemSize = 1
};
```
If we know that the Serie is with `itemSize=1`, then we can use the second method. Otherwise, the first one have to be used.

The file `inferring.h` might be helpful to deal with `double` / `Array` (or not).

2. Finish the `Manager` / `Decomposer`