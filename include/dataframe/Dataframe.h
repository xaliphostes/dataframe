/*
 * Copyright (c) 2024-now fmaerten@gmail.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 */

#pragma once
#include "Serie.h"
#include "types.h"
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace df {

/**
 * @brief A Dataframe is a collection of series.
 */
class Dataframe {
  public:
    Dataframe() = default;
    ~Dataframe() = default;

    /**
     * @brief Add a serie to the Dataframe with the given name
     * @throws std::runtime_error if a serie with this name already exists
     */
    template <typename T>
    void add(const std::string &name, const Serie<T> &serie);

    /**
     * @brief Add a serie to the Dataframe with the given name
     */
    template <typename T>
    void add(const std::string &name, const ArrayType<T> &array);

    /**
     * Remove a serie from the Dataframe
     * @throws std::runtime_error if the serie doesn't exist
     */
    void remove(const std::string &name);

    /**
     * Get a serie by name and type
     * @throws std::runtime_error if the serie doesn't exist or if there's a
     * type mismatch
     */
    template <typename T> const Serie<T> &get(const std::string &name) const;

    /**
     * Get the type info for a serie
     * @throws std::runtime_error if the serie doesn't exist
     */
    std::type_index type(const std::string &name) const;

    /**
     * Get the type name for a serie
     * @throws std::runtime_error if the serie doesn't exist
     */
    String type_name(const std::string &name) const;

    /**
     * Check if a serie exists with the given name
     */
    bool has(const std::string &name) const;

    /**
     * Get the number of series in the Dataframe
     */
    size_t size() const;

    /**
     * Get all serie names in the Dataframe
     */
    std::vector<std::string> names() const;

    /**
     * Clear all series from the Dataframe
     */
    void clear();

    /**
     * @brief Dump the content of the Dataframe to a stream with detailed
     * statistics
     * @param os Output stream (defaults to std::cout)
     * @param max_preview Maximum number of preview values to show
     */
    void dump(std::ostream &os = std::cout, size_t max_preview = 5) const;

  private:
    struct SerieInfo {
        std::shared_ptr<SerieBase> data;
        std::type_index type;

        template <typename T>
        SerieInfo(const Serie<T> &serie)
            : data(std::make_shared<Serie<T>>(serie)), type(typeid(Serie<T>)) {}
    };

    std::map<std::string, SerieInfo> series_;

    void printSeriesOverview(std::ostream &, int, int, int) const;

    void printSerieDetails(std::ostream &, const std::string &, size_t,
                           int) const;

    template <typename T>
    void printNumericStats(std::ostream &, const std::string &, int) const;

    void printSeriePreview(std::ostream &, const std::string &, size_t) const;

    template <typename T>
    void printNumericPreview(std::ostream &, const std::string &, size_t) const;
};

} // namespace df

#include "inline/Dataframe.hxx"
