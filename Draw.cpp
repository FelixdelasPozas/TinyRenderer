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
#include "Draw.h"
#include "Utils.h"

// C++
#include <list>
#include <cmath>
#include <limits>

using namespace Image;
using namespace Utils;

//--------------------------------------------------------------------
void Draw::line(int x0, int y0, int x1, int y1, TGA &image, const Color &color)
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
    s[i][0] = static_cast<int>(pts[2][i]) - static_cast<int>(pts[0][i]);
    s[i][1] = static_cast<int>(pts[1][i]) - static_cast<int>(pts[0][i]);
    s[i][2] = static_cast<int>(pts[0][i]) - static_cast<int>(P[i]);
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
void Draw::triangle(Vector3f *wPts, Vector3f *sPts, std::shared_ptr<zBuffer> buffer, const float intensity, Image::TGA &image, Vector2f *uv, Image::TGA &texture)
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

  Vector3f P;

  for (P[0] = min[0]; P[0] <= max[0]; ++P[0])
  {
    for (P[1] = min[1]; P[1] <= max[1]; ++P[1])
    {
      auto bc_screen = barycentric(sPts, P);

      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

      P[2] = 0;
      for (int i = 0; i < 3; i++) P[2] += wPts[i][2] * bc_screen[i];

      if (buffer->get(P[0], P[1]) < P[2])
      {
        auto fPoint = (uv[0]*bc_screen[0]) + (uv[1]*bc_screen[1]) + (uv[2]*bc_screen[2]);
        auto iPoint = Vector2i{fPoint[0]*texture.getWidth(), fPoint[1]*texture.getHeight()};
        auto tColor = texture.get(iPoint[0], iPoint[1]);
        auto iColor = Color(tColor.r*intensity, tColor.g*intensity, tColor.b*intensity, tColor.a*intensity);

        buffer->set(P[0], P[1], P[2]);
        image.set(P[0], P[1], iColor);
      }
    }
  }
}
