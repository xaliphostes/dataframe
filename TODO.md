# TODO list


## Classical

- **interleave** - Combines multiple series by alternating elements

## Surface/Mesh Operations:

- curvature - Compute surface curvature
- smoothing - Smooth mesh vertices or field values
- decimate - Reduce mesh complexity
- centerline - Extract centerline from surface
- tangents - Compute tangent vectors


## Point Cloud Operations:

- nearest_neighbors - Find nearest neighbors for points
- distance_field - Compute distance field from points
- cluster - Cluster points based on distance
- outliers - Detect and remove outlier points


## Field Interpolation:

- extrapolate - Extend field values beyond data points
- resample - Resample field onto new grid
- contours - Generate contour lines from scalar field for **REGULAR SAMPLING**


# OTHERS

## Flatten/FlatMap
```cpp
// Map then flatten results
auto flatMap(transform, collection);
```

## Compose/Curry/Partial
```cpp
// Create single-argument versions
auto curried = curry(function);

// Fix some arguments
auto partial = partial(function, arg1, arg2);
```
