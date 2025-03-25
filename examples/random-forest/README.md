# About the testing file
The file `winequality-red.csv` is similar to the actual Wine Quality dataset from the UCI Machine Learning Repository, but with a smaller number of samples for easier testing.

This CSV file contains 100 records with the following 12 columns:

- `fixed acidity`: most acids involved with wine (g/L)
- `volatile acidity`: the amount of acetic acid in wine (g/L)
- `citric acid`: amount of citric acid in wine (g/L)
- `residual sugar`: amount of sugar remaining after fermentation (g/L)
- `chlorides`: amount of salt in the wine (g/L)
- `free sulfur dioxide`: the free form of SO2 (mg/L)
- `total sulfur dioxide`: amount of free and bound forms of SO2 (mg/L)
- `density`: the density of wine (g/cm³)
- `pH`: describes how acidic or basic a wine is (0-14 scale)
- `sulphates`: amount of sulphates in wine (g/L)
- `alcohol`: percentage of alcohol (% by volume)
- `quality`: output variable - wine quality score between 0 and 10

The data represents a subset of the actual Wine Quality dataset, but it's sufficient to run the Random Forest example I provided earlier. The quality ratings range from 4 to 7, with most wines being rated as 5, which reflects the distribution in the real dataset.

# Important
Make sure the file has Unix-style line endings (LF rather than CRLF) if you're having any issues reading it.
