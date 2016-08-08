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
#include <Mesh.h>
#include <GL_Impl.h>
#include <Utils.h>
#include <Algebra.h>
#include <Shaders.h>

// C++
#include <array>
#include <iostream>
#include <limits>
#include <memory>
#include <string>

using namespace Images;
using namespace GL_Impl;
using namespace Utils;

Matrix4f ModelView;
Matrix4f ViewPort;
Matrix4f Projection;
Vector3f Light;

std::vector<std::shared_ptr<Mesh>> loadMeshes()
{
  std::vector<std::shared_ptr<Mesh>> meshes;

  // AFRICAN HEAD
//  auto diffuseTex = TGA::read("obj/african_head/african_head_diffuse.tga");
//  assert(diffuseTex);
//  diffuseTex->flipVertically();
//
//  auto normalMapTex = TGA::read("obj/african_head/african_head_nm.tga");
//  assert(normalMapTex);
//  normalMapTex->flipVertically();
//
//  auto specular = TGA::read("obj/african_head/african_head_spec.tga");
//  assert(specular);
//  specular->flipVertically();
//
//  auto tangent = TGA::read("obj/african_head/african_head_nm_tangent.tga");
//  assert(tangent);
//  tangent->flipVertically();
//
//  auto mesh = Mesh::read_Wavefront("obj/african_head/african_head.obj");
//  assert(mesh);
//  mesh->setDiffuseTexture(diffuseTex);
//  mesh->setNormalMap(normalMapTex);
//  mesh->setSpecular(specular);
//  mesh->setTangent(tangent);
//
//  meshes.push_back(mesh);

  // DIABLO 3
  auto diffuseTex = TGA::read("obj/diablo3_pose/diablo3_pose_diffuse.tga");
  assert(diffuseTex);
  diffuseTex->flipVertically();

  auto normalMapTex = TGA::read("obj/diablo3_pose/diablo3_pose_nm.tga");
  assert(normalMapTex);
  normalMapTex->flipVertically();

  auto specular = TGA::read("obj/diablo3_pose/diablo3_pose_spec.tga");
  assert(specular);
  specular->flipVertically();

  auto tangent = TGA::read("obj/diablo3_pose/diablo3_pose_nm_tangent.tga");
  assert(tangent);
  tangent->flipVertically();

  auto glow = TGA::read("obj/diablo3_pose/diablo3_pose_glow.tga");
  assert(glow);
  glow->flipVertically();

  auto mesh = Mesh::read_Wavefront("obj/diablo3_pose/diablo3_pose.obj");
  assert(mesh);
  mesh->setDiffuseTexture(diffuseTex);
  mesh->setNormalMap(normalMapTex);
  mesh->setSpecular(specular);
  mesh->setTangent(tangent);
  mesh->setGlowTexture(glow);

  meshes.push_back(mesh);

  // FLOOR
//  diffuseTex = TGA::read("obj/floor_diffuse.tga");
//  assert(diffuseTex);
//  diffuseTex->flipVertically();
//
//  tangent = TGA::read("obj/floor_nm_tangent.tga");
//  assert(tangent);
//  tangent->flipVertically();
//
//  mesh = Mesh::read_Wavefront("obj/floor.obj");
//  assert(mesh);
//  mesh->setDiffuseTexture(diffuseTex);
//  mesh->setTangent(tangent);
//
//  meshes.push_back(mesh);

  return meshes;
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
  short int width = 1000;
  short int height = 1000;
  Vector3f eye   {1,1,40};
  Vector3f center{0,0,0};
  Vector3f up    {0,1,0};

  auto lightVector = Vector3f{-5.,3.,5.}.normalize();
  Light = lightVector.normalize();
  viewport(width/8, height/8, width*3/4, height*3/4);

  auto image   = std::make_shared<TGA>(width, height, Image::RGB);
  auto zBuffer = std::make_shared<Utils::zBuffer>(width, height);
  auto dBuffer = std::make_shared<Utils::zBuffer>(width, height);

  BlockTimer timer("Draw");

  auto meshes = loadMeshes();

  // depth pass
  projection(-1.f/lightVector.norm());
  lookAt(lightVector, center, up);

  for(auto current: meshes)
  {
    #pragma omp parallel for
    for (unsigned long i = 0; i < current->faces_num(); i++)
    {
      DepthShader shader;
      shader.uniform_mesh = current;
      shader.depthBuffer = dBuffer;

      Vector3f screen_coords[3];
      for (int j = 0; j < 3; j++)
      {
        screen_coords[j] = shader.vertex(i, j);
      }

      triangle(screen_coords, shader, *dBuffer, *image);
    }
  }

  dBuffer->write("depthBuffer");
  image->clear();

  // second pass
  auto ShadowTransform = ViewPort*Projection*ModelView;
  projection(-1.f/(eye-center).norm());
  lookAt(eye, center, up);

  // int pass = 0;
  for(auto current: meshes)
  {
    #pragma omp parallel for
    for (unsigned long i = 0; i < current->faces_num(); i++)
    {
      HardShadowsShader shader;
      shader.uniform_transform_S = ShadowTransform;
      shader.depthBuffer = dBuffer;
      shader.uniform_mesh = current;
      shader.uniform_glow_coeff = 2.5;

      Vector3f screen_coords[3];
      for (int j = 0; j < 3; j++)
      {
        screen_coords[j] = shader.vertex(i, j);
      }

      triangle(screen_coords, shader, *zBuffer, *image);
    }

    // write the image after a mesh has been drawn.
    // image->write("pass_" + std::to_string(++pass));
    // dump texture decomposition in triangles.
    // dumpTexture(current, "pass_text_" + std::to_string(pass));
  }

  image->flipVertically(); // i want to have the origin at the left bottom corner of the image
  image->write("output");

  // dump zBuffer
  // zBuffer->write("zbuffer");

  // dump decomposition of the texture in triangles
  // dumpTexture(diffuseTex, mesh, "texture");

	return 0;
}
