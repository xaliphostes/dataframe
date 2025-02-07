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

#pragma once
#include <cmath>
#include <dataframe/Serie.h>
#include <dataframe/geo/types.h>

namespace df {

/**
 * @brief Compute InSAR (Interferometric Synthetic Aperture Radar) values from displacement vectors
 * 
 * InSAR values represent the projection of displacement vectors onto the satellite's line of sight (LOS).
 * This is useful for comparing model predictions with satellite observations.
 * 
 * @param u Displacement vector field as Serie of Vector3 (x,y,z components)
 * @param los Satellite line of sight direction vector (normalized 3D vector)
 * @return Serie<double> Scalar values representing displacement in LOS direction
 * 
 * @example
 * Serie<Vector3> displacements = {{1,0,0}, {0,1,0}, {0,0,1}};
 * Vector3 los = {1,0,0};  // Satellite looking in x direction
 * auto insar_values = insar(displacements, los);
 */
Serie<double> insar(const Serie<Vector3> &u, const Vector3 &los);

/**
 * @brief Compute interferometric fringes from InSAR values
 * 
 * Converts InSAR displacement values into cyclical fringes based on the specified spacing.
 * Each fringe represents one complete phase cycle (2π).
 * 
 * @param insar InSAR values computed from the insar() function
 * @param fringeSpacing Distance in displacement units per fringe cycle
 * @return Serie<double> Fringe values ranging from 0 to fringeSpacing
 * 
 * @example
 * auto insar_values = insar(displacements, los);
 * auto fringe_values = fringes(insar_values, 0.5);  // 0.5 units per fringe
 */
Serie<double> fringes(const Serie<double> &insar, double fringeSpacing);

} // namespace df

#include "inline/insar.hxx"