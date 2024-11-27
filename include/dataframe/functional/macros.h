#pragma once

/**
 * @brief Macro that allows to generate a function to be used in pipe (for example).
 * @example
 * ```cpp
 * Serie scale(const Serie&, double sc);
 * MAKE_OP(scale); // will generate: make_scale
 * 
 * auto result = pipe(
 *      serie,
 *      make_scale(2),
 *      make_scale(10)
 * )
 * ```
 */
#define MAKE_OP(op) \
    template <typename F> \
    auto make_##op(F &&cb) \
    { \
        return [cb = std::forward<F>(cb)](const auto &serie) { return op(serie, cb); }; \
    }
