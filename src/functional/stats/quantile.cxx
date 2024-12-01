/*
 * Copyright (c) 2023 fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <dataframe/functional/stats/quantile.h>
#include <dataframe/functional/conditional/check.h>
#include <dataframe/functional/cut.h>
#include <dataframe/functional/sort.h>
#include <cmath>

namespace df
{

    std::tuple<double, double> __ouliers__(const Serie& s, double mustache) ;

    double quantile(const Serie &s, double q) {
        if (!s.isValid()) {
            throw std::invalid_argument("series is not valid");
        }
        if (s.itemSize() != 1) {
            throw std::invalid_argument("quantile algorithm: itemSize must be 1");
        }
        if (q < 0 || q > 1) {
            throw std::invalid_argument("quantile must be in [0,1]");
        }

        auto newSerie = sort(s);
        const Array& sorted = newSerie.asArray();
        double pos = (sorted.size() - 1) * q;
        double base = std::floor(pos);
        double rest = pos - base;
        if (uint32_t(base + 1) < sorted.size()) {
            return sorted[uint32_t(base)] + rest * (sorted[uint32_t(base + 1)] - sorted[uint32_t(base)]);
        } else {
            return sorted[uint32_t(base)];
        }
    }

    double q25(const Serie &s) {
        return quantile(s, 0.25);
    }

    double q50(const Serie &s) {
        return quantile(s, 0.5);
    }

    double q75(const Serie &s) {
        return quantile(s, 0.75);
    }

    double IQR(const Serie &s) {
        return quantile(s, 0.75) - quantile(s, 0.25);
    }

    Serie outliers(const Serie &serie, double mustache) {
        auto o = __ouliers__(serie, mustache);
        return cut([o](double v, uint32_t) {
            return v < std::get<0>(o) || v > std::get<1>(o);
        }, serie);
    }

    /**
     * Return a serie of boolean indicating if an item of the serie s is an outliers or not
     * @category Dataframe/stats
     */
    Serie isOutliers(const Serie &serie, double mustache) {
        auto o = __ouliers__(serie, mustache);
        return check(serie, [o](double v, uint32_t) {
            return v < std::get<0>(o) || v > std::get<1>(o);
        });
    }

    /**
     * @see https://en.wikipedia.org/wiki/Interquartile_range
     * @see https://en.wikipedia.org/wiki/Box_plot
     * @param mustache The statistical distance for which a point is considered as outlier. Default 1.5
     * @category Dataframe/stats
     */
    Serie notOutliers(const Serie &serie, double mustache) {
        auto o = __ouliers__(serie, mustache);
        return cut([o](double v, uint32_t) {
            return v >= std::get<0>(o) && v <= std::get<1>(o);
        }, serie);
    }

    // --------------------------------------

    std::tuple<double, double> __ouliers__(const Serie& s, double mustache) {
        double Q25 = q25(s);
        double Q75 = q75(s);
        double iqr = Q75 - Q25;
        return std::make_tuple(
            Q25 - mustache * iqr,
            Q75 + mustache * iqr
        );
    }

}
