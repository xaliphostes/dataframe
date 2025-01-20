#pragma once
#include <dataframe/Serie.h>
#include <map>

namespace df {
namespace geo {

using Attribute = GenSerie<double>;
using Attributes = std::map<std::string, Attribute>;
using Positions = GenSerie<double>;
using Indices = GenSerie<uint32_t>;

} // namespace geo
} // namespace df
