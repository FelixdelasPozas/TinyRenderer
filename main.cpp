/*
 File: main.cpp
 Created on: 17/07/2016
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
#include "Mesh.h"
#include "TGA.h"

// C++
#include <iostream>
#include <memory>

using namespace Image;

void line(int x0, int y0, int x1, int y1, TGA &image, Color color)
{
  bool steep = false;
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
    float t = (x - x0) / (float) (x1 - x0);
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

int main(int argc, char *argv[])
{
  auto mesh  = Mesh::read_Wavefront("obj/african_head/african_head.obj");
  short int width = 800;
  short int height = 800;
  auto white = Color(255,255,255);

  auto image = std::make_shared<TGA>(width, height, TGA::RGB);

  for (unsigned long i = 0; i < mesh->faces_num(); i++)
  {
    auto face = mesh->getFace(i);
    for (int j = 0; j < 3; j++)
    {
      auto v0 = mesh->getVertex(face[j]);
      auto v1 = mesh->getVertex(face[(j + 1) % 3]);
      int x0 = (v0[0] + 1.) * width / 2.;
      int y0 = (v0[1] + 1.) * height / 2.;
      int x1 = (v1[0] + 1.) * width / 2.;
      int y1 = (v1[1] + 1.) * height / 2.;

      line(x0, y0, x1, y1, *image, white);
    }
  }

  image->flipVertically(); // i want to have the origin at the left bottom corner of the image
  image->write("output.tga");

	return 0;
}
