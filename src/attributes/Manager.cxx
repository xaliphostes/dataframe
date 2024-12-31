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

#include <algorithm>
#include <dataframe/attributes/Decomposer.h>
#include <dataframe/attributes/DecomposerFactory.h>
#include <dataframe/attributes/Manager.h>
#include <dataframe/types.h>
#include <set>

namespace df {
namespace attributes {

// -----------------------------------------

Manager createManager(const Series &series, const Strings names,
                      const Decomposers &decomposers, uint dim) {
    // Create the dataframe
    Dataframe df;
    uint id = 0;
    for (auto name : names) {
        df.add(name, series[id]);
    }
    return Manager(df, decomposers, dim);
}

// -----------------------------------------

Manager::Manager(const Dataframe &dataframe, const Decomposers &decomposers,
                 uint dimension)
    : df_(dataframe), dimension_(dimension) {
    registerAllDecomposer();

    for (auto d : decomposers) {
        ds_.push_back(d->clone());
    }
}

Manager::Manager(const Dataframe &dataframe, const Strings &decomposers,
                 uint dimension)
    : df_(dataframe), dimension_(dimension) {
    registerAllDecomposer();

    for (auto d : decomposers) {
        auto decomp = DecomposerFactory::create(d);
        if (decomp.get() == nullptr) {
            throw std::runtime_error("Decomposer " + d + " not found");
        }
        ds_.push_back(std::move(decomp));
    }
}

Manager::Manager(const Manager &mng)
    : df_(mng.df_), dimension_(mng.dimension_) {
    for (const auto& d : mng.ds_) {
        ds_.push_back(d->clone());
    }
}

Manager::~Manager() { clear(); }

void Manager::add(Decomposer *decomposer) {
    ds_.push_back(decomposer->clone());
}

void Manager::clear() { ds_.clear(); }

Serie Manager::serie(uint32_t itemSize, const String &name) const {
    for (auto const &x : df_.series()) {
        auto sname = x.first;
        const Serie &serie = x.second;
        if (serie.itemSize() == itemSize) // TODO: use dimension
        {
            if (name == sname) {
                return serie.clone();
            }
        }
    }

    for (const auto& d : ds_) {
        Serie serie = d->serie(df_, itemSize, name);
        if (serie.isValid()) {
            return serie;
        }
    }
    return Serie();
}

Strings Manager::names(uint32_t itemSize) const {
    std::set<String> names;

    // Add series with same itemSize
    for (auto const &x : df_.series()) {
        auto name = x.first;
        const Serie &serie = x.second;
        if (serie.itemSize() == itemSize && serie.dimension() == dimension_) {
            if (name != "positions" && name != "indices") {
                names.insert(name);
            }
        }

        // Add names from decomposers
        for (const auto& d : ds_) {
            Strings otherNames = d->names(df_, itemSize, serie, name);
            for (const auto &s : otherNames) {
                names.insert(s);
            }
        }
    }

    return Strings(names.begin(), names.end());
}

bool Manager::contains(uint32_t itemSize, const String &name) const {
    auto n = names(itemSize);
    return std::find(n.begin(), n.end(), name) != n.end();
}

} // namespace attributes
} // namespace df
