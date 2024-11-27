// In conditional.h
#pragma once
#include "../Serie.h"

namespace df
{
    

    /**
     * @brief Conditional operation on Serie elements
     * @param serie Input Serie
     * @param check_fn Predicate function
     * @param true_fn Function called when check returns true
     * @param false_fn Function called when check returns false
     * @return Serie
     *
     * @example
     * ```cpp
     * auto result = if_then_else(s,
     *     [](const auto& v) { return v[0] < 0; },
     *     [](const auto& v, uint32_t, const Serie&) {
     *         auto r = v;
     *         r[2] = 1;
     *         return r;
     *     },
     *     [](const auto& v, uint32_t, const Serie&) {
     *         auto r = v;
     *         r[2] = -1;
     *         return r;
     *     }
     * );
     * ```
     */
    template <typename Check, typename True, typename False>
    Serie if_then_else(const Serie &serie, Check &&check_fn, True &&true_fn, False &&false_fn)
    {
        return map(serie, [check_fn = std::forward<Check>(check_fn),
                           true_fn = std::forward<True>(true_fn),
                           false_fn = std::forward<False>(false_fn),
                           &serie](const auto &value, uint32_t i)
                   { return check_fn(value) ? true_fn(value, i, serie) : false_fn(value, i, serie); });
    }

}
