#include "dot.h"

namespace df {

    Serie dot(const Serie &a, const Serie &b) {
        uint32_t itemSize = a.itemSize();
        return a.map([itemSize, b](const Array& arr, uint32_t i) { // ieme item
            Array r = createArray(1, 0) ;
            const Array& bb = b.value(i) ;
            for (uint32_t k=0; k<itemSize; ++k) {
                r[0] += arr[k] * bb[k];
            }
            return r;
        });
    }

}
