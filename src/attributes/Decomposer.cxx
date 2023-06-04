#include "Decomposer.h"

namespace df
{

    Strings Decomposer::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const {
        return Strings();
    };

    Serie Decomposer::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const {
        return Serie();
    }

}
