// In conditional.h
#pragma once
#include "../../Serie.h"

namespace df
{
    /**
     * @brief Check condition on each element of a Serie
     */
    template <typename F>
    auto check(const Serie &serie, F &&cb)
    {
        if constexpr (detail::has_scalar_input_v<F>)
        {
            // Input is scalar
            Array results;
            results.reserve(serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i)
            {
                results.push_back(cb(serie.template get<double>(i), i) ? 1.0 : 0.0);
            }
            return Serie(1, results);
        }
        else
        {
            // Input is vector
            Array results;
            results.reserve(serie.count());
            for (uint32_t i = 0; i < serie.count(); ++i)
            {
                results.push_back(cb(serie.template get<Array>(i), i) ? 1.0 : 0.0);
            }
            return Serie(1, results);
        }
    }

    /**
     * @brief Creates a reusable check function
     */
    MAKE_OP(check);

}
