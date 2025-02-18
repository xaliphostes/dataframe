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
 * @brief Coordinates decomposer - extracts x,y,z components from vector types
 */
class Coordinates : public Decomposer {
  public:
    explicit Coordinates(const std::vector<std::string> &coordNames = {"x", "y",
                                                                       "z"})
        : coordNames_(coordNames) {}

    std::unique_ptr<Decomposer> clone() const override {
        return std::make_unique<Coordinates>(coordNames_);
    }

    std::vector<std::string> names(const Dataframe &dataframe,
                                   uint32_t itemSize, const SerieBase &serie,
                                   const std::string &name) const override {
        std::vector<std::string> result;

        // Only decompose vector types
        if (itemSize >= sizeof(double) && itemSize <= sizeof(double) * 3) {
            const size_t numComponents = itemSize / sizeof(double);
            for (size_t i = 0; i < numComponents && i < coordNames_.size();
                 ++i) {
                result.push_back(name + "_" + coordNames_[i]);
            }
        }

        return result;
    }

    Serie<double> serie(const Dataframe &dataframe, uint32_t itemSize,
                        const std::string &name) const override {
        // Parse the original serie name and coordinate component
        size_t pos = name.rfind('_');
        if (pos == std::string::npos) {
            throw std::runtime_error("Invalid coordinate attribute name: " +
                                     name);
        }

        std::string baseName = name.substr(0, pos);
        std::string coord = name.substr(pos + 1);

        // Find the coordinate index
        auto it = std::find(coordNames_.begin(), coordNames_.end(), coord);
        if (it == coordNames_.end()) {
            throw std::runtime_error("Invalid coordinate component: " + coord);
        }
        size_t index = std::distance(coordNames_.begin(), it);

        // Get the original serie and extract the component
        if (itemSize == sizeof(Vector2)) {
            return extractComponent(dataframe.get<Vector2>(baseName), index);
        } else if (itemSize == sizeof(Vector3)) {
            return extractComponent(dataframe.get<Vector3>(baseName), index);
        } else {
            throw std::runtime_error("Unsupported vector size for coordinates");
        }
    }

  private:
    std::vector<std::string> coordNames_;
};

} // namespace attributes
} // namespace df