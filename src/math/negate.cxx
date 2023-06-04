#include "dot.h"

namespace df {

    Serie negate(const Serie &serie) {
        uint32_t itemSize = serie.itemSize();
        return serie.map([itemSize](const Array& a, uint32_t i) { // ieme item
            Array r = a ;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[k] *= -1;
            }
            return r;
        });
    }

}
