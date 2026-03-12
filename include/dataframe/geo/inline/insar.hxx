/*
 * Copyright (c) 2024-now fmaerten@gmail.com
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


namespace df {

inline Serie<double> insar(const Serie<Vector3> &u, const Vector3 &los) {
    if (u.size() == 0) {
        return Serie<double>();
    }

    Serie<double> result;
    u.forEach([&](const Vector3 &disp, size_t) { result.add(dot(disp, los)); });

    return result;
}

inline Serie<double> fringes(const Serie<double> &insar, double fringeSpacing) {
    if (insar.size() == 0 || fringeSpacing <= 0) {
        return Serie<double>();
    }

    Serie<double> result;
    insar.forEach([fringeSpacing, &result](double val, size_t) {
        double frac = val / fringeSpacing - std::floor(val / fringeSpacing);
        double fringe = std::abs(fringeSpacing * frac);
        result.add(fringe);
    });

    return result;
}

} // namespace df