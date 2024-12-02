# TODO list

1. Optimization?

2. Iterators?: See https://github.com/VinGarcia/Simple-Iterator-Template
for an implementation of iterators using some macros

4. Code coverage
See [this link](https://github.com/pyarmak/cmake-gtest-coverage-example/blob/master/cmake/modules/CodeCoverage.cmake)
See also [this link](https://www.danielsieger.com/blog/2022/03/06/code-coverage-for-cpp.html#:~:text=What's%20Code%20Coverage%3F,blocks%2C%20or%20lines%20being%20covered.)

6. Check the `inv` unittest !!!

7. In `map`, we have a problem when the itemSize=1. We have to use `const Array&` instead of `double`. See `unitTest/map.cxx` at the end of the test.

7. `make_filterAll` test as it seems not to compile with multiple series

7. `filterAll` to be merged with `filter`

7. `rejectAll` to be merged with `reject`

7. Make sur the return type for `filterAll` and `rejectAll` can be decomposed:
    > 
    ```cpp
    auto [r1, r2] = filterAll([](){...}, s1, s2)
    ```
    and better
    ```cpp
    auto [r1, r2] = filter([](){...}, s1, s2)
    ```
    Same thing for `reject`

7. `reduce` for multiple series?

# Other operations to implement
These operations help create expressive and composable code by:
- Avoiding mutation and side effects
- Breaking problems into smaller pieces
- Making data transformations explicit
- Supporting composition of operations

## Filter/Find/Reject
```cpp
// Keep elements matching predicate
auto filter(predicate, collection);
// Find first element matching predicate
auto find(predicate, collection); 
// Remove elements matching predicate
auto reject(predicate, collection);
```

## Flatten/FlatMap
```cpp
// Convert nested collections to single level
auto flatten(nested_collection);
// Map then flatten results
auto flatMap(transform, collection);
```

## Take/Drop/Slice
```cpp
// Get first n elements
auto take(n, collection);
// Skip first n elements 
auto drop(n, collection);
// Get subset of elements
auto slice(start, end, collection);
```

## Compose/Curry/Partial
```cpp
// Combine functions
auto composed = compose(f, g, h);
// Create single-argument versions
auto curried = curry(function);
// Fix some arguments
auto partial = partial(function, arg1, arg2);
```

## Group/Partition
```cpp
// Group by key function
auto grouped = groupBy(keyFn, collection);
// Split into matching/non-matching
auto [matched, unmatched] = partition(predicate, collection);
```

## Sort/OrderBy
```cpp
// Sort by comparator
auto sorted = sort(comparator, collection);
// Sort by key function
auto ordered = orderBy(keyFn, collection);
```

## Unique/Distinct
```cpp
// Remove duplicates
auto unique = unique(collection);
```

## Chain/Sequence
```cpp
// Chain multiple collections
auto chained = chain(collection1, collection2);
```

## Memoize
```cpp
// Cache function results
auto memoized = memoize(expensive_function);
```
