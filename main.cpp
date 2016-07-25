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
#include "Draw.h"
#include "Utils.h"

// C++
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

using namespace Image;
using namespace Draw;
using namespace Utils;

int main(int argc, char *argv[])
{
  auto mesh  = Mesh::read_Wavefront("obj/african_head/african_head.obj");
  if(!mesh) return 1;

  short int width = 1000;
  short int height = 1000;
  Vector3f light_dir(0,0,-1);

  auto image   = std::make_shared<TGA>(width, height, TGA::RGB);
  auto zBuffer = std::make_shared<Utils::zBuffer>(width, height);

  BlockTimer timer("Draw");

#pragma omp parallel for
  for (unsigned long i = 0; i < mesh->faces_num(); i++)
  {
    auto face = mesh->getFace(i);

    Vector3f world_coords[3];
    for (int j: {0,1,2})
    {
      auto v = mesh->getVertex(face[j]);
      world_coords[j] = v;
    }

    auto normal = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
    normal.normalize();

    float intensity = normal * light_dir;

    if (intensity > 0)
    {
      if(intensity > 1) intensity = 1;

      triangle(world_coords, zBuffer, Color(intensity * 255, intensity * 255, intensity * 255, 255), *image);
    }
  }

  image->flipVertically(); // i want to have the origin at the left bottom corner of the image
  image->write("output.tga");


  zBuffer->write("zbuffer.tga");

	return 0;
}
