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
         * @brief Components decomposer with numeric indexing for scalars, vectors, and matrices
         */
        class Components : public GenDecomposer<Components> {
        public:
            Strings names(const Dataframe& dataframe, DecompDimension targetDim,
                const SerieBase& serie, const String& name) const override
            {
                Strings result;

                if (targetDim == DecompDimension::Scalar) {
                    // Original scalar decomposition with x,y,z,w and xx,xy,etc.
                    if (auto names = getVectorComponentNames(dataframe, name)) {
                        return *names;
                    }
                    if (auto names = getTensorComponentNames(dataframe, name)) {
                        return *names;
                    }
                    if (auto names = getMatrixComponentNames(dataframe, name)) {
                        return *names;
                    }
                } else if (targetDim == DecompDimension::Vector) {
                    // Vector decomposition with numeric indices: name_1, name_2, etc.
                    return getVectorIndexNames(dataframe, name);
                } else if (targetDim == DecompDimension::Matrix) {
                    // Matrix decomposition with row-column indices: name_11, name_12, etc.
                    return getMatrixIndexNames(dataframe, name);
                }

                return result;
            }

            Serie<double> serie(const Dataframe& dataframe, DecompDimension targetDim,
                const std::string& name) const override
            {
                if (targetDim == DecompDimension::Scalar) {
                    // Parse scalar component name (existing logic)
                    auto [baseName, index] = parseComponentName(dataframe, name);

                    if (index) {
                        return extractAnyComponent<Vector2, Vector3, Vector4, Vector6, Matrix2D,
                            Matrix3D, Matrix4D, Strain2D, Strain3D, Stress2D, Stress3D, SMatrix2D,
                            SMatrix3D, SMatrix4D>(dataframe, baseName, *index);
                    }
                } else if (targetDim == DecompDimension::Vector) {
                    // Parse vector component name: name_1, name_2, etc.
                    auto [baseName, index] = parseVectorIndexName(name);
                    if (index) {
                        return extractAnyComponent<Vector2, Vector3, Vector4, Vector6, Matrix2D,
                            Matrix3D, Matrix4D, Strain2D, Strain3D, Stress2D, Stress3D, SMatrix2D,
                            SMatrix3D, SMatrix4D>(dataframe, baseName, *index);
                    }
                } else if (targetDim == DecompDimension::Matrix) {
                    // Parse matrix component name: name_11, name_12, etc.
                    auto [baseName, index] = parseMatrixIndexName(dataframe, name);
                    if (index) {
                        return extractAnyComponent<Vector2, Vector3, Vector4, Vector6, Matrix2D,
                            Matrix3D, Matrix4D, Strain2D, Strain3D, Stress2D, Stress3D, SMatrix2D,
                            SMatrix3D, SMatrix4D>(dataframe, baseName, *index);
                    }
                }

                throw std::runtime_error("Unsupported type or component index");
            }

        private:
            // ===== SCALAR DECOMPOSITION (original methods) =====

            std::optional<Strings> getVectorComponentNames(
                const Dataframe& dataframe, const std::string& name) const
            {
                if (dataframe.has<Vector2>(name)) {
                    return Strings { name + "x", name + "y" };
                }
                if (dataframe.has<Vector3>(name)) {
                    return Strings { name + "x", name + "y", name + "z" };
                }
                if (dataframe.has<Vector4>(name)) {
                    return Strings { name + "x", name + "y", name + "z", name + "w" };
                }
                return std::nullopt;
            }

            std::optional<Strings> getTensorComponentNames(
                const Dataframe& dataframe, const std::string& name) const
            {
                if (dataframe.has<Stress2D>(name) || dataframe.has<Strain2D>(name)) {
                    return Strings { name + "xx", name + "xy", name + "yy" };
                }
                if (dataframe.has<Stress3D>(name) || dataframe.has<Strain3D>(name)) {
                    return Strings { name + "xx", name + "xy", name + "xz", name + "yy",
                        name + "yz", name + "zz" };
                }
                return std::nullopt;
            }

            std::optional<Strings> getMatrixComponentNames(
                const Dataframe& dataframe, const std::string& name) const
            {
                if (dataframe.has<Matrix2D>(name)) {
                    return Strings { name + "11", name + "12", name + "21", name + "22" };
                }
                if (dataframe.has<Matrix3D>(name)) {
                    return Strings { name + "11", name + "12", name + "13", name + "21",
                        name + "22", name + "23", name + "31", name + "32", name + "33" };
                }
                if (dataframe.has<Matrix4D>(name)) {
                    return Strings { name + "11", name + "12", name + "13", name + "14",
                        name + "21", name + "22", name + "23", name + "24", name + "31",
                        name + "32", name + "33", name + "34", name + "41", name + "42",
                        name + "43", name + "44" };
                }
                return std::nullopt;
            }

            // ===== VECTOR DECOMPOSITION (numeric indices) =====

            Strings getVectorIndexNames(const Dataframe& dataframe, const std::string& name) const
            {
                if (dataframe.has<Vector2>(name) || dataframe.has<Vector3>(name)
                    || dataframe.has<Vector4>(name) || dataframe.has<Vector6>(name)) {
                    return { name };
                }
                return {};
            }

            // ===== MATRIX DECOMPOSITION (row-column indices) =====

            Strings getMatrixIndexNames(const Dataframe& dataframe, const std::string& name) const
            {
                if (dataframe.has<Matrix2D>(name) || dataframe.has<SMatrix2D>(name)
                    || dataframe.has<Strain2D>(name) || dataframe.has<Stress2D>(name)
                    || dataframe.has<Strain3D>(name) || dataframe.has<Stress3D>(name)
                    || dataframe.has<SMatrix3D>(name) || dataframe.has<Matrix3D>(name)
                    || dataframe.has<SMatrix4D>(name) || dataframe.has<Matrix4D>(name)) {
                    return { name };
                }
                return {};
            }

            // ===== PARSING METHODS =====

            // Parse scalar component name (original method)
            std::pair<std::string, std::optional<size_t>> parseComponentName(
                const Dataframe& dataframe, const std::string& name) const
            {
                // Handle vector components (e.g., Px, Py, Pz)
                if (name.length() >= 2
                    && (name.back() == 'x' || name.back() == 'y' || name.back() == 'z'
                        || name.back() == 'w')) {
                    std::string baseName = name.substr(0, name.length() - 1);
                    char comp = name.back();
                    size_t index = (comp == 'x') ? 0 : (comp == 'y') ? 1 : (comp == 'z') ? 2 : 3;
                    return { baseName, index };
                }

                // Handle tensor components (e.g., Sxx, Sxy, etc.)
                if (name.length() >= 3
                    && (name.substr(name.length() - 2) == "xx"
                        || name.substr(name.length() - 2) == "xy"
                        || name.substr(name.length() - 2) == "xz"
                        || name.substr(name.length() - 2) == "yy"
                        || name.substr(name.length() - 2) == "yz"
                        || name.substr(name.length() - 2) == "zz")) {
                    std::string baseName = name.substr(0, name.length() - 2);
                    std::string comp = name.substr(name.length() - 2);

                    std::map<std::string, size_t> tensorMap = { { "xx", 0 }, { "xy", 1 },
                        { "xz", 2 }, { "yy", 3 }, { "yz", 4 }, { "zz", 5 } };

                    return { baseName, tensorMap[comp] };
                }

                // Handle matrix components (e.g., T11, T12, etc.)
                if (name.length() >= 3 && std::isdigit(name[name.length() - 1])
                    && std::isdigit(name[name.length() - 2])) {
                    std::string baseName = name.substr(0, name.length() - 2);
                    int row = name[name.length() - 2] - '1';
                    int col = name[name.length() - 1] - '1';

                    size_t dim = 4; // Default to 4x4
                    if (dataframe.has<Matrix2D>(baseName))
                        dim = 2;
                    else if (dataframe.has<Matrix3D>(baseName))
                        dim = 3;

                    size_t index = row * dim + col;
                    return { baseName, index };
                }

                return { name, std::nullopt };
            }

            // Parse vector component name: name_1, name_2, etc.
            std::pair<std::string, std::optional<size_t>> parseVectorIndexName(
                const std::string& name) const
            {
                size_t pos = name.rfind('_');
                if (pos == std::string::npos) {
                    return { name, std::nullopt };
                }

                std::string baseName = name.substr(0, pos);
                std::string indexStr = name.substr(pos + 1);

                try {
                    size_t index = std::stoul(indexStr) - 1; // Convert to 0-based index
                    return { baseName, index };
                } catch (...) {
                    return { name, std::nullopt };
                }
            }

            // Parse matrix component name: name_11, name_12, etc.
            std::pair<std::string, std::optional<size_t>> parseMatrixIndexName(
                const Dataframe& dataframe, const std::string& name) const
            {
                size_t pos = name.rfind('_');
                if (pos == std::string::npos || name.length() - pos != 3) {
                    return { name, std::nullopt };
                }

                std::string baseName = name.substr(0, pos);
                std::string indexStr = name.substr(pos + 1);

                if (indexStr.length() == 2 && std::isdigit(indexStr[0])
                    && std::isdigit(indexStr[1])) {
                    int row = indexStr[0] - '1';
                    int col = indexStr[1] - '1';

                    size_t dim = 4; // Default to 4x4
                    if (dataframe.has<Matrix2D>(baseName) || dataframe.has<SMatrix2D>(baseName))
                        dim = 2;
                    else if (dataframe.has<Matrix3D>(baseName)
                        || dataframe.has<SMatrix3D>(baseName))
                        dim = 3;

                    size_t index = row * dim + col;
                    return { baseName, index };
                }

                return { name, std::nullopt };
            }

            // ===== EXTRACTION METHODS (reusing existing logic) =====

            template <typename T>
            std::optional<Serie<double>> tryType(
                const Dataframe& dataframe, const std::string& name, size_t index) const
            {
                if (dataframe.has<T>(name)) {
                    return extractComponent(dataframe.get<T>(name), index);
                }
                return std::nullopt;
            }

            template <typename... Ts>
            Serie<double> extractAnyComponent(
                const Dataframe& dataframe, const std::string& name, size_t index) const
            {
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