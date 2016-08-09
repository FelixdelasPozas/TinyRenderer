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

#ifndef GL_IMPL_H_
#define GL_IMPL_H_

// Project
#include "Images.h"
#include "Algebra.h"

extern Vector3f Light;
extern Matrix4f ViewPort;
extern Matrix4f ModelView;
extern Matrix4f Projection;

class Mesh;

namespace Utils
{
  class zBuffer;
}

namespace GL_Impl
{
  struct Shader
  {
      virtual ~Shader() {};
      virtual Vector3f vertex(int iface, int nthvert) = 0;
      virtual bool fragment(Vector3f bar, Images::Color &color) = 0;

      std::shared_ptr<Mesh> uniform_mesh;
  };

  /** \brief Creates the viewport matrix.
   * \param[in] x
   * \param[in] y
   * \param[in] width viewport width
   * \param[in] height viewport height.
   * \param[in] depth view depth.
   *
   */
  void viewport(const int x, const int y, const int width, const int height, const int depth = 255);

  /** \brief Creates the projection matrix.
   * \param[in] coeff equals to -1/(camera-focalpoint distance).
   *
   */
  void projection(const float coeff = 0.);

  /** \brief Creates the model view matrix.
   * \param[in] eye camera position.
   * \param[in] center focal point?
   * \param[in] up up vector.
   */
  void lookAt(const Vector3f &eye, const Vector3f &center = Vector3f{0,0,0}, const Vector3f &up = Vector3f{0,1,0});

  /** \brief Draws the line (x0,y0)-(x1,y1) in the given color on the given image.
   * \param[in] x0 first point x coordinate.
   * \param[in] y0 first point y coordinate.
   * \param[in] x1 second point x coordinate.
   * \param[in] y1 second point y coordinate.
   * \param[inout] image TGA image raw pointer.
   * \param[in] color line color.
   *
   */
  void line(int x0, int y0, int x1, int y1, Images::Image &image, const Images::Color &color);

  /** \brief Draws a given triangle in the given color on the given image.
   * \param[in] sPts pointer to 2d triangle points.
   * \param[in] shader vertex & fragment shader.
   * \param[inout] buffer zBuffer object.
   * \param[inout] image TGA image raw pointer.
   *
   */
  void triangle(Vector3f *sPts, Shader &shader, Utils::zBuffer &buffer, Images::Image &image);

  /** \brief Computes the slope of the given pixel p in the direction dir with the information of the z-buffer.
   * \param[in] buffer zBuffer object.
   * \param[in] point point coordinates.
   * \param[in] direction 2D direction.
   *
   */
  float max_elevation_angle(Utils::zBuffer &buffer, Vector2f point, Vector2f direction);

} // namespace GL_Impl

#endif // GL_IMPL_H_
