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


# Idea

```
scalar = number
```

```
vector = [number, number, ...]
```

```
         | number number ... |
         | .                 |
matrix = | .                 |
         | number number ... |
```

## Example for vector
```
v1 = [1 2 3 4 5 6 7 8 9]
v2a = [1 2 3]
```

## Example for matrix
```
    | 1 2 3 |
m = | 4 5 6 | === [1 2 3 4 5 6 7 8 9] in dim=3
    | 7 8 9 |

    | 1 2 3 |
n = | 2 4 5 | === [1 2 3 4 5 6] in dim=3
    | 3 5 6 |
```