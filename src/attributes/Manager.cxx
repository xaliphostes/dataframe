#include "Manager.h"

namespace df
{

    Manager::Manager(const Dataframe &dataframe, const Decomposers &decomposers) : df_(dataframe)
    {
        for (auto d : decomposers)
        {
            ds_.push_back(d);
        }
    }

    void Manager::add(const Decomposer &decomposer)
    {
        ds_.push_back(decomposer);
    }

    void Manager::clear()
    {
        ds_.clear();
    }

    Serie Manager::serie(uint32_t itemSize, const String &name) const
    {
        for (auto const &x : df_.series())
        {
            auto sname = x.first;
            const Serie &serie = x.second;
            if (serie.itemSize() == itemSize) // TODO: use dimension
            {
                if (name == sname)
                {
                    return serie.clone();
                }
            }
        }

        for (const auto &d : ds_)
        {
        }

        for (const Decomposer &d : ds_)
        {
            Serie serie = d.serie(df_, itemSize, name);
            if (serie.isValid())
            {
                return serie;
            }
        }
        return Serie();
    }

    Strings Manager::names(uint32_t itemSize) const
    {
        Strings names;

        // Add series with same itemSize
        for (auto const &x : df_.series())
        {
            auto name = x.first;
            const Serie &serie = x.second;
            if (serie.itemSize() == itemSize)
            { // TODO: use dimension
                if (name != "positions" && name != "indices")
                {
                    names.push_back(name);
                }
            }

            // Add names from decomposers
            for (const auto &d : ds_)
            {
                const auto otherNames = d.names(df_, itemSize, serie, name);
                for (const auto &s : otherNames)
                {
                    names.push_back(s);
                }
            }
        }

        return names;
    }

    bool Manager::contains(uint32_t itemSize, const String &name) const
    {
        auto n = names(itemSize);
        return std::find(n.begin(), n.end(), name) != n.end();
    }
}
