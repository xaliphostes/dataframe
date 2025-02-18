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
namespace attributes {

inline Manager::Manager(const Dataframe &df) : dataframe_(df) {}

inline Manager::Manager(const Manager &other) : dataframe_(other.dataframe_) {
    // Deep copy decomposers
    for (const auto &decomposer : other.decomposers_) {
        if (decomposer) {
            decomposers_.push_back(decomposer->clone());
        }
    }
}

// Add a decomposer
inline void Manager::addDecomposer(std::unique_ptr<Decomposer> decomposer) {
    if (decomposer) {
        decomposers_.push_back(std::move(decomposer));
    }
}

// Get all available attribute names for a given item size
inline std::vector<std::string> Manager::getNames(uint32_t itemSize) const {
    std::vector<std::string> result;

    // Iterate through all series in the dataframe
    for (const auto &name : dataframe_.names()) {
        // For each decomposer
        for (const auto &decomposer : decomposers_) {
            if (!decomposer)
                continue;

            // Get decomposed names for this serie
            const auto &serie = dataframe_.get<SerieBase>(name);
            auto decomposed_names =
                decomposer->names(dataframe_, itemSize, serie, name);

            // Add to result if matching itemSize
            result.insert(result.end(), decomposed_names.begin(),
                          decomposed_names.end());
        }
    }
    return result;
}

// Get a specific serie by name
template <typename T>
inline Serie<T> Manager::getSerie(const std::string &name) const {
    // Try each decomposer
    for (const auto &decomposer : decomposers_) {
        if (!decomposer)
            continue;

        try {
            // Attempt to get serie using this decomposer
            auto serie = decomposer->serie(dataframe_, sizeof(T), name);
            if (!serie.empty()) {
                return serie.template as<T>();
            }
        } catch (const std::exception &) {
            // Continue to next decomposer if this one fails
            continue;
        }
    }
    throw std::runtime_error("Unable to find serie with name: " + name);
}

// Check if a specific attribute exists
inline bool Manager::hasAttribute(uint32_t itemSize,
                                  const std::string &name) const {
    for (const auto &decomposer : decomposers_) {
        if (!decomposer)
            continue;

        try {
            auto serie = decomposer->serie(dataframe_, itemSize, name);
            return !serie.empty();
        } catch (const std::exception &) {
            continue;
        }
    }
    return false;
}

// Clear all decomposers
inline void Manager::clear() { decomposers_.clear(); }

// Get number of registered decomposers
inline size_t Manager::decomposerCount() const { return decomposers_.size(); }

// Helper function to create a Manager from series and names
template <typename T>
inline Manager createManager(const std::vector<Serie<T>> &series,
                             const std::vector<std::string> &names,
                             Decomposers decomposers = {}) {
    if (series.size() != names.size()) {
        throw std::runtime_error("Number of series must match number of names");
    }

    Dataframe df;
    for (size_t i = 0; i < series.size(); ++i) {
        df.add(names[i], series[i]);
    }

    Manager manager(df);
    for (auto &decomposer : decomposers) {
        manager.addDecomposer(std::move(decomposer));
    }

    return manager;
}

} // namespace attributes
} // namespace df