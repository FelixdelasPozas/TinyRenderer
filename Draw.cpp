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

// C++
#include <list>
#include <cmath>

using namespace Image;

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
    auto t = (x - x0) / (float) (x1 - x0);
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
Vector3f barycentric(Vector2i *pts, Vector2i P)
{
    auto u = Vector3f(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]) ^ Vector3f(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
    if (std::abs(u[2])<1) return Vector3f(-1,1,1); // triangle is degenerate, in this case return smth with negative coordinates
    return Vector3f(1.f-(u[0]+u[1])/u[2], u[1]/u[2], u[0]/u[2]);
}

//--------------------------------------------------------------------
void Draw::triangle(Vector2i *pts, const Image::Color &color, Image::TGA &image)
{
  Vector2i bboxmin(image.getWidth() - 1, image.getHeight() - 1);
  Vector2i bboxmax(0, 0);
  Vector2i clamp(image.getWidth() - 1, image.getHeight() - 1);

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      bboxmin[j] = std::max(0, std::min(bboxmin[j], pts[i][j]));
      bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
    }
  }

  Vector2i P;
  for (P[0] = bboxmin[0]; P[0] <= bboxmax[0]; ++P[0])
  {
    for (P[1] = bboxmin[1]; P[1] <= bboxmax[1]; ++P[1])
    {
      auto bc_screen = barycentric(pts, P);
      if (bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;
      image.set(P[0], P[1], color);
    }
  }
}
