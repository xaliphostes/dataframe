#include <dataframe/operations/algebra/inv.h>

namespace df {

Serie inv(const Serie& serie) {
    if (!serie.isValid()) return Serie();

    // Check valid dimensions
    if (serie.dimension() != 3 || (serie.itemSize() != 6 && serie.itemSize() != 9)) {
        return Serie();
    }

    // For 6-component symmetric tensor
    if (serie.itemSize() == 6) {
        return serie.map([](const Array& t, uint32_t) {
            // t = [xx, xy, xz, yy, yz, zz]
            const double xx = t[0], xy = t[1], xz = t[2];
            const double yy = t[3], yz = t[4];
            const double zz = t[5];

            // Calculate determinant
            double det = xx*(yy*zz - yz*yz) - xy*(xy*zz - yz*xz) + xz*(xy*yz - yy*xz);
            if (std::abs(det) < 1e-10) {
                throw std::runtime_error("Matrix is singular");
            }

            // Calculate inverse
            double invDet = 1.0/det;
            Array inv(6);
            inv[0] = (yy*zz - yz*yz) * invDet;  // xx
            inv[1] = (xz*yz - xy*zz) * invDet;  // xy
            inv[2] = (xy*yz - xz*yy) * invDet;  // xz
            inv[3] = (xx*zz - xz*xz) * invDet;  // yy
            inv[4] = (xy*xz - xx*yz) * invDet;  // yz
            inv[5] = (xx*yy - xy*xy) * invDet;  // zz

            return inv;
        });
    }
    
    // For 9-component tensor
    return serie.map([](const Array& t, uint32_t) {
        // t = [xx, xy, xz, yx, yy, yz, zx, zy, zz]
        const double xx = t[0], xy = t[1], xz = t[2];
        const double yx = t[3], yy = t[4], yz = t[5];
        const double zx = t[6], zy = t[7], zz = t[8];

        // Calculate determinant
        double det = xx*(yy*zz - yz*zy) - xy*(yx*zz - yz*zx) + xz*(yx*zy - yy*zx);
        if (std::abs(det) < 1e-10) {
            throw std::runtime_error("Matrix is singular");
        }

        // Calculate inverse
        double invDet = 1.0/det;
        Array inv(9);
        inv[0] = (yy*zz - yz*zy) * invDet;  // xx
        inv[1] = (xz*zy - xy*zz) * invDet;  // xy
        inv[2] = (xy*yz - xz*yy) * invDet;  // xz
        inv[3] = (yz*zx - yx*zz) * invDet;  // yx
        inv[4] = (xx*zz - xz*zx) * invDet;  // yy
        inv[5] = (zx*xy - xx*zy) * invDet;  // yz
        inv[6] = (yx*zy - yy*zx) * invDet;  // zx
        inv[7] = (xy*zx - xx*zy) * invDet;  // zy
        inv[8] = (xx*yy - xy*yx) * invDet;  // zz

        return inv;
    });
}

} // namespace df