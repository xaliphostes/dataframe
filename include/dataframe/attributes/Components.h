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

#pragma once
#include <dataframe/attributes/Decomposer.h>

namespace df {
namespace attributes {

/**
 * @brief Components decomposer - handles general component extraction for
 * vectors and matrices
 */
class Components : public Decomposer {
  public:
    std::unique_ptr<Decomposer> clone() const override {
        return std::make_unique<Components>();
    }

    std::vector<std::string> names(const Dataframe &dataframe,
                                   uint32_t itemSize, const SerieBase &serie,
                                   const std::string &name) const override {
        std::vector<std::string> result;

        // Handle different types of components
        const size_t numComponents = itemSize / sizeof(double);
        if (numComponents > 1) {
            for (size_t i = 0; i < numComponents; ++i) {
                result.push_back(name + "_" + std::to_string(i));
            }
        }

        return result;
    }

    Serie<double> serie(const Dataframe &dataframe, uint32_t itemSize,
                        const std::string &name) const override {
        // Parse the component index
        size_t pos = name.rfind('_');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid component attribute name: " +
                                     name);
        }

        std::string baseName = name.substr(0, pos);
        size_t index = std::stoul(name.substr(pos + 1));

        // Handle different types based on itemSize
        if (itemSize == sizeof(Vector2)) {
            return extractComponent(dataframe.get<Vector2>(baseName), index);
        } else if (itemSize == sizeof(Vector3)) {
            return extractComponent(dataframe.get<Vector3>(baseName), index);
        } else if (itemSize == sizeof(Matrix2D)) {
            return extractComponent(dataframe.get<Matrix2D>(baseName), index);
        } else if (itemSize == sizeof(Matrix3D)) {
            return extractComponent(dataframe.get<Matrix3D>(baseName), index);
        } else if (itemSize == sizeof(Stress2D)) {
            return extractComponent(dataframe.get<Stress2D>(baseName), index);
        } else if (itemSize == sizeof(Stress3D)) {
            return extractComponent(dataframe.get<Stress3D>(baseName), index);
        } else {
            throw std::runtime_error("Unsupported component type size");
        }
    }
};

} // namespace attributes
} // namespace df