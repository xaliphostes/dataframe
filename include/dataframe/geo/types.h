#pragma once
#include <dataframe/Serie.h>
#include <map>

namespace df {

// using Attribute = Serie<double>;
// using Attributes = std::map<std::string, Attribute>;

using Positions2 = Serie<Vector2>;
using Positions3 = Serie<Vector3>;
using Segments = Serie<iVector2>;
using Triangles = Serie<iVector3>;

} // namespace df
