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

#include <dataframe/utils/meta.h>

namespace df {
    namespace attributes {

        // Helper to get component count for a type
        template <typename T> inline size_t Decomposer::getComponentCount()
        {
            if constexpr (std::is_array_v<T> || details::is_std_array_v<T>) {
                return std::tuple_size_v<T>;
            } else {
                return 1;
            }
        }

        // Helper to extract component at index
        template <typename T>
        inline Serie<double> Decomposer::extractComponent(const Serie<T>& serie, size_t index)
        {
            return serie.map([index](const T& value, size_t) -> double {
                if constexpr (std::is_array_v<T> || details::is_std_array_v<T>) {
                    return static_cast<double>(value[index]);
                }
                // Handle struct-based vector types
                else if constexpr (std::is_same_v<T, Vector2> || std::is_same_v<T, Vector3>
                    || std::is_same_v<T, Vector4> || std::is_same_v<T, Vector6>) {
                    return static_cast<double>(value[index]);
                }
                // Handle struct-based matrix types
                else if constexpr (std::is_same_v<T, Matrix2D> || std::is_same_v<T, Matrix3D>
                    || std::is_same_v<T, Matrix4D>) {
                    return static_cast<double>(value[index]);
                }
                // Handle struct-based symmetric matrix/tensor types
                else if constexpr (std::is_same_v<T, SMatrix2D> || std::is_same_v<T, SMatrix3D>
                    || std::is_same_v<T, SMatrix4D> || std::is_same_v<T, Stress2D>
                    || std::is_same_v<T, Stress3D> || std::is_same_v<T, Strain2D>
                    || std::is_same_v<T, Strain3D>) {
                    return static_cast<double>(value[index]);
                } else {
                    return static_cast<double>(value);
                }
            });
        }

        // ---------------------------------------------------------------

        template <typename C> inline std::unique_ptr<Decomposer> GenDecomposer<C>::clone() const
        {
            return std::make_unique<C>(static_cast<const C&>(*this));
        }

        // Forward names() to derived class
        template <typename C>
        inline Strings GenDecomposer<C>::names(const Dataframe& dataframe,
            DecompDimension targetDim, const SerieBase& serie, const String& name) const
        {
            return static_cast<const C*>(this)->names(dataframe, targetDim, serie, name);
        }

        // Forward serie() to derived class
        template <typename C>
        inline Serie<double> GenDecomposer<C>::serie(
            const Dataframe& dataframe, DecompDimension targetDim, const std::string& name) const
        {
            return static_cast<const C*>(this)->serie(dataframe, targetDim, name);
        }

        // ---------------------------------------------------------------

        Manager::Manager(const Dataframe& df)
            : dataframe_(df)
        {
        }

        Manager::Manager(const Manager& other)
            : dataframe_(other.dataframe_)
        {
            for (const auto& decomposer : other.decomposers_) {
                if (decomposer) {
                    decomposers_.push_back(decomposer->clone());
                }
            }
        }

        void Manager::addDecomposer(const Decomposer& decomposer)
        {
            decomposers_.push_back(decomposer.clone());
        }

        std::vector<std::string> Manager::getNames(DecompDimension targetDim) const
        {
            std::vector<std::string> result;

            for (const auto& name : dataframe_.names()) {
                for (const auto& decomposer : decomposers_) {
                    if (!decomposer)
                        continue;

                    const auto& serie = dataframe_.get(name);

                    auto decomposed_names = decomposer->names(dataframe_, targetDim, serie, name);
                    result.insert(result.end(), decomposed_names.begin(), decomposed_names.end());
                }
            }
            return result;
        }

        template <typename T> Serie<T> Manager::getSerie(const std::string& name) const
        {
            for (const auto& decomposer : decomposers_) {
                if (!decomposer)
                    continue;

                if constexpr (details::is_std_array<T>::value) {
                    // auto DIM = details::array_dimension<T>::value;
                    // auto serie =
                    //     decomposer->serie(dataframe_, DecompDimension::Vector, name);
                    // if (!serie.empty()) {
                    //     return serie.template as<T>();
                    // }
                } else if constexpr (details::is_container<T>::value) {
                    // todo
                } else {
                    static_assert(std::is_arithmetic<T>::value || details::is_container<T>::value,
                        "Type must be either arithmetic or a container type");
                    auto serie = decomposer->serie(dataframe_, DecompDimension::Scalar, name);
                    if (!serie.empty()) {
                        return serie.template as<T>();
                    }
                }
            }
            throw std::runtime_error("Unable to find serie with name: " + name);
            // return Serie<T>();
        }

        bool Manager::hasAttribute(DecompDimension targetDim, const std::string& name) const
        {
            for (const auto& decomposer : decomposers_) {
                if (!decomposer)
                    continue;

                try {
                    auto serie = decomposer->serie(dataframe_, targetDim, name);
                    return !serie.empty();
                } catch (const std::exception&) {
                    continue;
                }
            }
            return false;
        }

        void Manager::clear() { decomposers_.clear(); }

        size_t Manager::decomposerCount() const { return decomposers_.size(); }

        // Helper function to create a Manager
        template <typename... Ts>
        inline Manager createManager(
            const std::vector<std::string>& names, const Serie<Ts>&... series)
        {
            if (sizeof...(Ts) != names.size()) {
                throw std::runtime_error("Number of series must match number of names");
            }

            Dataframe df;
            size_t i = 0;
            (df.add(names[i++], series), ...);

            Manager manager(df);
            return manager;
        }

    } // namespace attributes
} // namespace df