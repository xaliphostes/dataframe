# Basic Statistics:
```cpp
- median()         // Middle value or average of two middle values
- mode()           // Most frequent value(s)
- variance()       // Measure of variability
- skewness()       // Measure of asymmetry
- kurtosis()       // Measure of "tailedness"
- std_dev()        // Standard deviation (sqrt of variance)
```

# Range Statistics:
```cpp
- range()          // Difference between max and min values
- percentile(p)    // Get value at specified percentile
- zscore()         // Get standardized values ((x - mean)/std_dev)
```

# Distribution Analysis:
```cpp
- entropy()        // Information entropy of the distribution
- is_normal()      // Test for normality (e.g., Shapiro-Wilk test)
- kde()            // Kernel Density Estimation
- ecdf()           // Empirical Cumulative Distribution Function
```

# Correlation and Relationships:
```cpp
- autocorrelation()     // Serial correlation
- rank_correlation()    // Spearman's rank correlation
- pearson_correlation() // Linear correlation coefficient
```

# Rolling/Window Statistics:
```cpp
- rolling_mean(window) // Moving average
- rolling_std(window)  // Moving standard deviation
- rolling_max(window)  // Moving maximum
- rolling_min(window)  // Moving minimum
```

# Data Quality:
```cpp
- count_nulls()  // Count missing values
- count_unique() // Count unique values
- value_counts() // Frequency of each unique value
- describe()     // Summary statistics (min, max, mean, std, quartiles)
```

# Outlier Detection:
```cpp
- winsorize()    // Cap extreme values at specified percentiles
- robust_scale() // Scale data using robust statistics
```

# Time Series Specific (if time series data):
```cpp
- seasonality()  // Detect seasonal patterns
- trend()        // Detect trend component
- decompose()    // Decompose into trend, seasonal, and residual components
```