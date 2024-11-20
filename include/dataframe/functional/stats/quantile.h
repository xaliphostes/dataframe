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

#pragma once
#include <dataframe/Serie.h>

namespace df
{

    /**
     * @brief Get the quantile q of a serie. The serie must have itemSize = 1,
     * and q must be in [0,1].
     */
    double quantile(const Serie &serie, double q);

    /**
     * @brief Get the quantile for q = 0.25
     */
    double q25(const Serie &serie);

    /**
     * @brief Get the quantile for q = 0.5
     */
    double q50(const Serie &serie);

    /**
     * @brief Get the quantile for q = 0.75
     */
    double q75(const Serie &serie);

    /**
     * Get the q75 - q25
     */
    double IQR(const Serie &serie);

    /**
     * Get the outliers of a serie given the mustache
     */
    Serie outliers(const Serie &serie, double mustache);

    /**
     * To be done !
     */
    Serie isOutliers(const Serie &serie, double mustache);

    /**
     * To be done !
     */
    Serie notOutliers(const Serie &serie, double mustache = 1.5);

}
