# TODO list


## Classical

- **chunks(size)** - Similar to split but divides the series into chunks of a specified size rather than a specified number of chunks
- **take(n)** - Takes the first n elements
- **skip(n)** - Skips the first n elements
- **slice(start, end)** - Takes a slice from start to end indices
- **interleave** - Combines multiple series by alternating elements
- **groupBy**: Group elements based on a key function


## Vector Field Operations:


## Surface/Mesh Operations:

- curvature - Compute surface curvature
- smoothing - Smooth mesh vertices or field values
- decimate - Reduce mesh complexity
- centerline - Extract centerline from surface
- tangents - Compute tangent vectors


## Point Cloud Operations:

- kdtree - Build KD-tree for point queries
- nearest_neighbors - Find nearest neighbors for points
- distance_field - Compute distance field from points
- cluster - Cluster points based on distance
- outliers - Detect and remove outlier points


## Field Interpolation:

- extrapolate - Extend field values beyond data points
- resample - Resample field onto new grid
- contours - Generate contour lines from scalar field for **REGULAR SAMPLING**


## New architecture
See if it is not judicious to have a base class for Serie which deals with any object type (e.g. a vector of any type, a int, a user defined type...), and we do not need to have the notion of itemSize.
```cpp
template <typename T>
class Serie {
public:
    using value_type = T;
    using ArrayType = std::vector<T>;

    std::string type() const;
    T& operator[](size_t index);
    const T& operator[](size_t index) const;
    uint32_t size() const;

private:
    ArrayType data;
};

// Using C++20
template <typename T> requires std::is_arithmetic_v<T> using SMatrix2 = std::array<T, 3>;
// Using C++17
template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> using SMatrix2 = std::array<T, 3>;
// For floating point number, we can use
// std::is_floating_point_v<T> instead of std::is_arithmetic_v<T>


template <typename T> requires std::is_arithmetic_v<T> using FMatrix2 = std::array<T, 4>;
template <typename T> requires std::is_arithmetic_v<T> using SMatrix3 = std::array<T, 6>;
template <typename T> requires std::is_arithmetic_v<T> using FMatrix3 = std::array<T, 9>;

// Then define some helpers for this kind of algebraic objects
template <typename Matrix>
class MatrixHelper {
public:
    MatrixHelper(const Matrix&);
    const T& operator[](uint i, uint j) const;
};
```



# OTHERS

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
