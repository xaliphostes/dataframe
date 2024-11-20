#include <dataframe/functional/zip.h>
#include <dataframe/Serie.h>

int main() {
    // Create some test Series
    df::Serie s1(1, {1, 2, 3});                 // scalar serie
    df::Serie s2(2, {4,5, 6,7, 8,9});          // 2D serie
    df::Serie s3(1, {10, 11, 12});             // scalar serie
    
    // Using variadic zip
    auto result1 = df::zip(s1, s2, s3);
    result1.dump();  // Will show the combined values
    
    // Using vector zip
    std::vector<df::Serie> series = {s1, s2, s3};
    auto result2 = df::zipVector(series);
    result2.dump();  // Will show the same combined values
    
    // The results can be used in further operations
    result1.forEach([](const Array& values, uint32_t i) {
        // Process the combined values
        // values will contain {1,4,5,10} for i=0, {2,6,7,11} for i=1, etc.
    });
}
