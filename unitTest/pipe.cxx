#include <dataframe/algos/pipe.h>
#include <dataframe/Serie.h>

int main()
{
    df::Serie s(1, {1, 2, 3, 4, 5});

    // Using pipe directly
    auto result1 = pipe(s,
        [](const df::Serie& s) { return s.mapScalar([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filterScalar([](double v, uint32_t) { return v > 4; }); }
    );
    
    // Creating and using a reusable pipeline
    auto doubleAndFilter = df::makePipe(
        [](const df::Serie& s) { return s.mapScalar([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filterScalar([](double v, uint32_t) { return v > 4; }); }
    );

    auto result2 = doubleAndFilter(s);

}
