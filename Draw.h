/*
 File: Draw.h
 Created on: 18 jul. 2016
 Author: Felix de las Pozas Alvarez

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef DRAW_H_
#define DRAW_H_

// Project
#include "TGA.h"
#include "Algebra.h"

namespace Utils
{
  class zBuffer;
}

namespace Draw
{
  /** \brief Draws the line (x0,y0)-(x1,y1) in the given color on the given image.
   * \param[in] x0 first point x coordinate.
   * \param[in] y0 first point y coordinate.
   * \param[in] x1 second point x coordinate.
   * \param[in] y1 second point y coordinate.
   * \param[inout] image TGA image raw pointer.
   * \param[in] color line color.
   *
   */
  void line(int x0, int y0, int x1, int y1, Image::TGA &image, const Image::Color &color);

  /** \brief Draws a given triangle in the given color on the given image.
   * \param[in] pts pointer to 3d triangle points.
   * \param[inout] buffer z-buffer vector.
   * \param[in] intensity color intensity.
   * \param[inout] image TGA image raw pointer.
   * \param[in] uv uv coords of vertices.
   * \param[in] texture triangle texture.
   *
   */
  void triangle(Vector3f *wPts, std::shared_ptr<Utils::zBuffer> buffer, const float intensity, Image::TGA &image, Vector2f *uv, Image::TGA &texture);

  /** \brief Helper method to covert world coordinates to screen coordinates.
   * \param[in] coords world coordinates vector.
   * \param[in] width screen width.
   * \param[in] height screen height.
   *
   */
  const Vector3f toScreen(const Vector3f coords, const unsigned short width, const unsigned short height);

} // namespace Draw

#endif // DRAW_H_
