/*
 File: Draw.cpp
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

// Project
#include <GL_Impl.h>
#include "Utils.h"

// C++
#include <list>
#include <cmath>
#include <limits>

using namespace Images;
using namespace Utils;

//--------------------------------------------------------------------
void GL_Impl::projection(const float coeff)
{
  Projection.identity();
  Projection[3][2] = coeff;
}

//--------------------------------------------------------------------
void GL_Impl::lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up)
{
  auto z = (eye - center).normalize();
  auto x = (up ^ z).normalize();
  auto y = (z ^ x).normalize();

  Matrix4f Minv, Tr;
  Minv.identity();
  Tr.identity();

  for (int i = 0; i < 3; i++)
  {
    Minv[0][i] = x[i];
    Minv[1][i] = y[i];
    Minv[2][i] = z[i];
      Tr[i][3] = -center[i];
  }

  ModelView = Minv * Tr;
}
//--------------------------------------------------------------------
void GL_Impl::viewport(int x, int y, int width, int height, int depth)
{
  ViewPort.identity();
  ViewPort[0][3] = x + width / 2.f;
  ViewPort[1][3] = y + height / 2.f;
  ViewPort[2][3] = depth / 2.f;

  ViewPort[0][0] = width / 2.f;
  ViewPort[1][1] = height / 2.f;
  ViewPort[2][2] = depth / 2.f;
}

//--------------------------------------------------------------------
void GL_Impl::line(int x0, int y0, int x1, int y1, Image &image, const Color &color)
{
  auto steep = false;
  if (std::abs(x0 - x1) < std::abs(y0 - y1))
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  for (int x = x0; x <= x1; x++)
  {
    auto t = (x - x0) / static_cast<float>(x1 - x0);
    int y = y0 * (1. - t) + y1 * t;

    if (steep)
    {
      image.set(y, x, color);
    }
    else
    {
      image.set(x, y, color);
    }
  }
}

//--------------------------------------------------------------------
Vector3f barycentric(Vector3f *pts, const Vector3f &P)
{
  Vector3f s[2];

  for (auto i: {0,1})
  {
    s[i][0] = pts[2][i] - pts[0][i];
    s[i][1] = pts[1][i] - pts[0][i];
    s[i][2] = pts[0][i] - P[i];
  }

  auto u = s[0] ^ s[1];

  // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
  if (std::abs(u[2]) <= 0)
  {
    return Vector3f{-1, 1, 1}; // in this case generate negative coordinates, it will be thrown away by the rasterizator
  }

  return Vector3f{1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]};
}

//--------------------------------------------------------------------
void GL_Impl::triangle(Vector3f *sPts, Shader &shader, zBuffer &buffer, Images::Image &image)
{
  const auto width  = image.getWidth();
  const auto height = image.getHeight();

  Vector2i min{ std::numeric_limits<int>::max(),  std::numeric_limits<int>::max()};
  Vector2i max{-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max()};
  Vector2i clamp{width-1, height-1};

  for (int i: {0,1,2})
  {
    for (int j: {0,1})
    {
      min[j] = std::max(0, std::min(min[j], static_cast<int>(sPts[i][j])));
      max[j] = std::min(clamp[j], std::max(max[j], static_cast<int>(sPts[i][j])));
    }
  }

  Vector3f P{0,0,0};

  for (P[0] = min[0]; P[0] <= max[0]; ++P[0])
  {
    for (P[1] = min[1]; P[1] <= max[1]; ++P[1])
    {
      auto bc_screen = barycentric(sPts, P);
      P[2] = sPts[0][2]*bc_screen[0] + sPts[1][2]*bc_screen[1] + sPts[2][2]*bc_screen[2];

      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0 || !buffer.checkAndSet(P[0], P[1], P[2])) continue;

      Color color;
      bool discard = shader.fragment(bc_screen, color);
      if (!discard)
      {
        image.set(P[0], P[1], color);
      }
    }
  }
}

//--------------------------------------------------------------------
float GL_Impl::max_elevation_angle(zBuffer &buffer, Vector2f point, Vector2f direction)
{
  float max_angle = 0;
  for (float t = 0.; t < 1000.; t += 1.)
  {
    auto current = point + direction * t;
    if (current[0] >= buffer.getWidth() || current[1] >= buffer.getHeight() || current[0] < 0 || current[1] < 0)
    {
      return max_angle;
    }

    auto distance = (point - current).norm();
    if (distance < 1.f) continue;

    auto poscurr  = static_cast<unsigned short>(current[1]) * buffer.getWidth() + static_cast<unsigned short>(current[0]);
    auto pospoint = static_cast<unsigned short>(  point[1]) * buffer.getWidth() + static_cast<unsigned short>(  point[0]);

    float elevation = buffer.getBuffer()[poscurr]-buffer.getBuffer()[pospoint];

    max_angle = std::max(max_angle, ::atanf(elevation / distance));
  }

  return max_angle;
}
