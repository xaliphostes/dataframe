#pragma once
#include "Decomposer.h"

namespace df {

    class ComponentsDecomposer: public Decomposer {
    public:
        Strings names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const override ;
        Serie serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const override;
    };

}