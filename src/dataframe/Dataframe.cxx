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

#include <dataframe/Dataframe.h>
#include <dataframe/utils/nameOfSerie.h>
#include <iostream>

namespace df
{

    Dataframe::Dataframe(uint32_t count) : count_(count)
    {
    }

    void Dataframe::clear() {
        series_.clear();
    }

    void Dataframe::create(const std::string &name, uint32_t itemSize, uint32_t count)
    {
        add(name, Serie(itemSize, count));
    }

    void Dataframe::setCount(uint32_t count)
    {
        if (count_ != count && count > 0)
        {
            count_ = count;
            if (series_.size())
            {
                for (auto it = series_.begin(); it != series_.end(); ++it)
                {

                    it->second.reCount(count);
                }
            }
        }
    }

    /**
     * @brief Add a new Serie in this Dataframe. If a Serie with the same name already exists, it is replaced.
     */
    void Dataframe::add(const std::string &name, const Serie &serie)
    {
        if (count_ == 0)
        {
            count_ = serie.count();
        }

        if (count_ != serie.count())
        {
            // throw std::invalid_argument("Cannot add a Serie in a Dataframe when its count (" + std::to_string(serie.count()) + ") is different from the Dataframe count (" + std::to_string(count_) + ")");
        }
        series_[name] = serie;
    }

    /**
     * @brief Same as {@link add}
     */
    void Dataframe::set(const std::string &name, const Serie &serie)
    {
        add(name, serie);
    }

    void Dataframe::del(const std::string &name)
    {
        auto it = series_.find(name);
        if (it != series_.end())
        {
            series_.erase(it);
        }
    }

    Serie &Dataframe::operator[](const std::string &name)
    {
        return series_[name];
    }

    const Serie &Dataframe::operator[](const std::string &name) const
    {
        for (auto it = series_.cbegin(); it != series_.cend(); ++it)
        {
            if (it->first == name)
            {
                return it->second;
            }
        }

        // Is is correct to do that?
        static const Serie fake;
        return fake;
    }

    bool Dataframe::contains(const Serie &serie) const
    {
        String name = nameOfSerie(*this, serie);
        return name.size() == 0 ? false : true;
    }

    bool Dataframe::contains(const String &name) const
    {
        for (auto it = series_.cbegin(); it != series_.cend(); ++it)
        {
            if (it->first == name)
            {
                return true;
            }
        }
        return false;
    }

    void Dataframe::dump() const
    {
        for (auto it = series_.cbegin(); it != series_.cend(); ++it)
        {
            std::cerr << "name: \"" << it->first
                      << "\", itemSize: " << it->second.itemSize()
                      << ", count: " << it->second.count()
                      << std::endl;
        }
    }

}
