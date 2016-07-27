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
#include "Algebra.h"

// C++
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

using namespace Image;
using namespace Draw;
using namespace Utils;

const int depth = 255;

void viewport(int x, int y, int w, int h, Matrix4f &m)
{
  m.identity();
  m[0][3] = x + w / 2.f;
  m[1][3] = y + h / 2.f;
  m[2][3] = depth / 2.f;

  m[0][0] = w / 2.f;
  m[1][1] = h / 2.f;
  m[2][2] = depth / 2.f;
}

int main(int argc, char *argv[])
{
  auto mesh  = Mesh::read_Wavefront("obj/african_head/african_head.obj");
  if(!mesh) return 1;

  short int width = 1000;
  short int height = 1000;
  Vector3f light_dir{-1,-1,1};
  light_dir.normalize();
  Vector3f camera{0,0,3};

  Matrix4f Projection;
  Projection.identity();
  Projection[3][2] = -1.f/camera[2];

  Matrix4f ViewPort;
  viewport(width/8, height/8, width*3/4, height*3/4, ViewPort);

  auto image   = std::make_shared<TGA>(width, height, TGA::RGB);
  auto imagetx = TGA::read("obj/african_head/african_head_diffuse.tga");
  imagetx->flipVertically();
  assert(imagetx);
  auto zBuffer = std::make_shared<Utils::zBuffer>(width, height);

  BlockTimer timer("Draw");

  #pragma omp parallel for
  for (unsigned long i = 0; i < mesh->faces_num(); i++)
  {
    auto face = mesh->getFace(i);
    auto uvs  = mesh->getTexel(i);

    Vector3f world_coords[3];
    Vector3i screen_coords[3];
    Vector2f uv_coords[3];
    float    intensities[3];

    for (int j: {0,1,2})
    {
      world_coords[j]  = mesh->getVertex(face[j]);
      screen_coords[j] = (ViewPort * (Projection * world_coords[j].augment())).project();
      uv_coords[j]     = mesh->getuv(uvs[j]);
      intensities[j]   = mesh->getNormal(face[j]) * light_dir;
    }

    triangle(world_coords, screen_coords, intensities, zBuffer, *image, uv_coords, *imagetx);
  }

  image->flipVertically(); // i want to have the origin at the left bottom corner of the image
  image->write("output.tga");

  zBuffer->write("zbuffer.tga");

  /** dump decomposition of the texture in triangles */
  auto white = Color(255,255,255,255);
  width = imagetx->getWidth();
  height = imagetx->getHeight();
  #pragma omp parallel for
  for (unsigned long i = 0; i < mesh->faces_num(); i++)
  {
    auto uvs  = mesh->getTexel(i);
    Vector2f uv_coords[3];

    for (int j: {0,1,2})
    {
      uv_coords[j] = mesh->getuv(uvs[j]);
    }

    for (int j: {0,1,2})
    {
      auto uv1 = uv_coords[j];
      auto uv2 = uv_coords[(j+1)%3];

      line(uv1[0]*width, uv1[1]*height, uv2[0]*width, uv2[1]*height, *imagetx, white);
    }
  }

  imagetx->flipVertically();
  imagetx->write("texture.tga");

	return 0;
}
