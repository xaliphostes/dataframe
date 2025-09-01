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
#include <dataframe/attributes/Manager.h>
#include <optional>


namespace df {
namespace attributes {

/**
 * @brief Components decomposer with specialized naming for vectors and tensors
 */
class Components : public GenDecomposer<Components> {
  public:
    Strings names(const Dataframe &dataframe, DecompDimension targetDim,
                  const SerieBase &serie, const String &name) const override {
        Strings result;

        if (targetDim == DecompDimension::Scalar) {
            // Try each type category
            if (auto names = getVectorComponentNames(dataframe, name)) {
                return *names;
            }
            if (auto names = getTensorComponentNames(dataframe, name)) {
                return *names;
            }
            if (auto names = getMatrixComponentNames(dataframe, name)) {
                return *names;
            }
        }
        return result;
    }

    Serie<double> serie(const Dataframe &dataframe, DecompDimension targetDim,
                        const std::string &name) const override {
        // Parse the component from the name
        auto [baseName, index] = parseComponentName(dataframe, name);
        MSG(baseName, *index);

        if (index) {
            return extractAnyComponent<Vector2, Vector3, Vector4, Vector6,
                                       Matrix2D, Matrix3D, Matrix4D, Strain2D,
                                       Strain3D, Stress2D, Stress3D, SMatrix2D,
                                       SMatrix3D, SMatrix4D>(dataframe,
                                                             baseName, *index);
        }

        throw std::runtime_error("Unsupported type or component index");
    }

  private:
    // Helper to get vector component names (e.g., Ux, Uy, Uz)
    std::optional<Strings>
    getVectorComponentNames(const Dataframe &dataframe,
                            const std::string &name) const {
        if (dataframe.has<Vector2>(name)) {
            return Strings{name + "x", name + "y"};
        }
        if (dataframe.has<Vector3>(name)) {
            return Strings{name + "x", name + "y", name + "z"};
        }
        if (dataframe.has<Vector4>(name)) {
            return Strings{name + "x", name + "y", name + "z", name + "w"};
        }
        return std::nullopt;
    }

    // Helper to get tensor component names (e.g., Sxx, Sxy, etc.)
    std::optional<Strings>
    getTensorComponentNames(const Dataframe &dataframe,
                            const std::string &name) const {
        if (dataframe.has<Stress2D>(name) || dataframe.has<Strain2D>(name)) {
            return Strings{name + "xx", name + "xy", name + "yy"};
        }
        if (dataframe.has<Stress3D>(name) || dataframe.has<Strain3D>(name)) {
            return Strings{name + "xx", name + "xy", name + "xz",
                           name + "yy", name + "yz", name + "zz"};
        }
        return std::nullopt;
    }

    // Helper to get matrix component names (e.g., M11, M12, etc.)
    std::optional<Strings>
    getMatrixComponentNames(const Dataframe &dataframe,
                            const std::string &name) const {
        if (dataframe.has<Matrix2D>(name)) {
            return Strings{name + "11", name + "12", name + "21", name + "22"};
        }
        if (dataframe.has<Matrix3D>(name)) {
            return Strings{name + "11", name + "12", name + "13",
                           name + "21", name + "22", name + "23",
                           name + "31", name + "32", name + "33"};
        }
        if (dataframe.has<Matrix4D>(name)) {
            return Strings{name + "11", name + "12", name + "13", name + "14",
                           name + "21", name + "22", name + "23", name + "24",
                           name + "31", name + "32", name + "33", name + "34",
                           name + "41", name + "42", name + "43", name + "44"};
        }
        return std::nullopt;
    }

    // Helper to parse component name into base name and index
    std::pair<std::string, std::optional<size_t>>
    parseComponentName(const Dataframe &dataframe,
                       const std::string &name) const {
        // Handle vector components (e.g., Ux, Uy, Uz)
        if (name.length() >= 2 && (name.back() == 'x' || name.back() == 'y' ||
                                   name.back() == 'z' || name.back() == 'w')) {
            std::string baseName = name.substr(0, name.length() - 1);
            char comp = name.back();
            size_t index = (comp == 'x')   ? 0
                           : (comp == 'y') ? 1
                           : (comp == 'z') ? 2
                                           : 3;
            return {baseName, index};
        }

        // Handle tensor components (e.g., Sxx, Sxy, etc.)
        if (name.length() >= 3 && (name.substr(name.length() - 2) == "xx" ||
                                   name.substr(name.length() - 2) == "xy" ||
                                   name.substr(name.length() - 2) == "xz" ||
                                   name.substr(name.length() - 2) == "yy" ||
                                   name.substr(name.length() - 2) == "yz" ||
                                   name.substr(name.length() - 2) == "zz")) {
            std::string baseName = name.substr(0, name.length() - 2);
            std::string comp = name.substr(name.length() - 2);

            // Map tensor components to linear index
            std::map<std::string, size_t> tensorMap = {{"xx", 0}, {"xy", 1},
                                                       {"xz", 2}, {"yy", 3},
                                                       {"yz", 4}, {"zz", 5}};

            return {baseName, tensorMap[comp]};
        }

        // Handle matrix components (e.g., M11, M12, etc.)
        if (name.length() >= 3 && std::isdigit(name[name.length() - 1]) &&
            std::isdigit(name[name.length() - 2])) {
            std::string baseName = name.substr(0, name.length() - 2);
            int row = name[name.length() - 2] - '1';
            int col = name[name.length() - 1] - '1';
            size_t dim = 3; // Assume 3x3 matrix by default
            if (dataframe.has<Matrix2D>(baseName))
                dim = 2;

            size_t index = row * dim + col;
            return {baseName, index};
        }

        return {name, std::nullopt};
    }

    // Template helper to try extracting component
    template <typename T>
    std::optional<Serie<double>> tryType(const Dataframe &dataframe,
                                         const std::string &name,
                                         size_t index) const {
        if (dataframe.has<T>(name)) {
            return extractComponent(dataframe.get<T>(name), index);
        }
        return std::nullopt;
    }

    // Template to try multiple types
    template <typename... Ts>
    Serie<double> extractAnyComponent(const Dataframe &dataframe,
                                      const std::string &name,
                                      size_t index) const {
        std::optional<Serie<double>> result;
        ((result = tryType<Ts>(dataframe, name, index)) || ...);

        if (!result) {
            throw std::runtime_error("No matching type found for: " + name);
        }
        return *result;
    }
};

} // namespace attributes
} // namespace df