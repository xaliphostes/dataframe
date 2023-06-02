#pragma once

#include "Dataframe.h"
#include "Serie.h"
#include "types.h"
#include "Decomposer.h"

namespace df
{

    /**
     * @brief Manager of decomposers
     */
    class Manager
    {
    public:
        /**
         * By default, no decomposer...
         */
        Manager(const Dataframe &dataframe, const Decomposers &decomposers = {});

        void add(const Decomposer &decomposer);
        void clear();

        Serie serie(uint32_t itemSize, const String &name) const;
        Strings names(uint32_t itemSize) const;
        bool contains(uint32_t itemSize, const String &name) const;

    private:
        const Dataframe &df_;
    };

}
