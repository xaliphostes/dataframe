Here's a summary of some prolem solving techniques I used to make this work:

1. The key to making it work was using a more flexible lambda approach with auto and if constexpr instead of explicit functor classes.

2. For scalar multiplication:
```cpp
[d](const auto& a, uint32_t) -> std::decay_t<decltype(a)> {
    if constexpr (std::is_same_v<std::decay_t<decltype(a)>, T>) {
        return a * d;
    } else {
        // vector case
        auto r = a;
        for (uint32_t j = 0; j < a.size(); ++j) {
            r[j] *= d;
        }
        return r;
    }
}
```

3. For series multiplication, we used a similar pattern but with more cases to handle:

- scalar-scalar multiplication
- vector-scalar multiplication
- vector-vector multiplication


4. The type system can now correctly deduce the return types and handle both scalar and vector cases appropriately.

