#pragma once

#include "Dataframe.h"
#include "Serie.h"
#include "types.h"

namespace df
{

    /**
     * @brief Interface for a decomposer
     */
    class Decomposer
    {
    public:
        virtual Strings names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) = 0;
        virtual Serie serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) = 0;
    };

    using Decomposers = std::vector<Decomposer>;

}
