#include "scale.h"

namespace df {

    Serie scale(const Serie& s, double scale) {
        uint32_t size = s.itemSize();
        return s.map([scale, size](const Array& a, uint32_t i) {
            Array r = a ;
            for (uint32_t k=0; k<size; ++k) {
                r[k] *= scale;
            }
            return r;
        });
    }

    Serie scale(const Serie& s, const Array& scales) {
        if (s.itemSize() != scales.size()) {
            throw std::invalid_argument("(scale) Serie itemSize (" +
                std::to_string(s.itemSize()) +
                " differs from scale size (" +
                std::to_string(scales.size()) +
                ")");
        }

        uint32_t size = s.itemSize();
        return s.map([scales, size](const Array& a, uint32_t i) {
            Array r = a ;
            for (uint32_t k=0; k<size; ++k) {
                r[k] *= scales[k];
            }
            return r;
        });
    }

}
