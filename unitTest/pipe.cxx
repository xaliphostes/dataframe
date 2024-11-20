#include <dataframe/Serie.h>
#include <dataframe/algos/pipe.h>
#include <dataframe/operations/math/equals.h>
#include "assertions.h"

int main()
{
    df::Serie s(1, {1, 2, 3, 4, 5});
    df::Serie solution(1, {6, 8, 10});

    // Using pipe directly
    auto result1 = pipe(s,
        [](const df::Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); }
    );
    assertCondition(df::equals(solution, result1));
    
    // Creating and using a reusable pipeline
    auto doubleAndFilter = df::makePipe(
        [](const df::Serie& s) { return s.map([](double v, uint32_t) { return v * 2; }); },
        [](const df::Serie& s) { return s.filter([](double v, uint32_t) { return v > 4; }); }
    );

    auto result2 = doubleAndFilter(s);
    assertCondition(df::equals(solution, result2));

}
