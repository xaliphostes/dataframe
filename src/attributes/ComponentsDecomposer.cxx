#include "ComponentsDecomposer.h"

namespace df {

    Strings ComponentsDecomposer::names(const Dataframe &dataframe, uint32_t itemSize, const Serie &serie, const String &name) const {
        if (name == "positions" || name == "indices") {
            return Strings();
        }

        String names;
        // TODO...

        return names;
    }

    Serie ComponentsDecomposer::serie(const Dataframe &dataframe, uint32_t itemSize, const String &name) const {
        // TODO...
        return Serie();
    }

}