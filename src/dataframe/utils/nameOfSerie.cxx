#include <dataframe/utils/nameOfSerie.h>
#include <dataframe/Serie.h>
#include <dataframe/Dataframe.h>

namespace df {

    String nameOfSerie(const Dataframe& dataframe, const Serie& serie) {
        for (auto it = dataframe.series().cbegin(); it != dataframe.series().cend(); ++it)
        {
            const Serie& other = it->second;
            if (&serie == &other) {
                return it->first;
            }
        }
        return "";
    }

}