#pragma once
#include "Serie.h"

namespace df
{

    template <typename F>
    Serie apply(const Serie &serie, F &&fn)
    {
        return serie.map(fn);
    }

}
