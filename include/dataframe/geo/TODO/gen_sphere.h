#pragma once
#include "../../Serie.h"
#include "../../Dataframe.h"

namespace df {
namespace geo {

/**
 * Generate a sphere with the specified subdivision level. The returned series
 * are positions of the e vertices and indices of the triangles.
 * @code
 * auto sphere = df::geo::generateSphere<double>(4);
 *
 * df::print(sphere.get<double>("positions");
 * df::print(sphere.get<uint32_t>("indices");
 * @endcode
 */
template <typename T>
df::Dataframe generateSphere(int subdivision, bool shared = true);

} // namespace geo
} // namespace df

#include "inline/gen_sphere.hxx"