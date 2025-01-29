#pragma once
#include <dataframe/Serie.h>
#include <map>

namespace df {
namespace geo {

using Attribute = Serie<double>;
using Attributes = std::map<std::string, Attribute>;
using Positions = Serie<double>;
using Indices = Serie<uint32_t>;

} // namespace geo
} // namespace df
