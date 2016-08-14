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
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <thread>

using namespace Images;
using namespace GL_Impl;
using namespace Utils;

Matrix4f ModelView;
Matrix4f ViewPort;
Matrix4f Projection;
Vector3f Light;

constexpr auto PI   = 3.14159265358979323846;
constexpr auto PI_2 = 1.57079632679489661923;
constexpr auto PI_4 = 0.78539816339744830962;

std::vector<std::shared_ptr<Mesh>> loadMeshes()
{
  std::vector<std::shared_ptr<Mesh>> meshes;

  auto obj = Wavefront::read("obj/TF2-Scout/Scout.obj");
  assert(obj);
  for(auto mesh: obj->meshes())
  {
    meshes.push_back(mesh);
  }
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
//  auto obj = Wavefront::read("obj/african_head/african_head.obj");
//  assert(obj);
//  for(auto mesh: obj->meshes())
//  {
//    mesh->setDiffuseTexture(diffuseTex);
//    mesh->setNormalMap(normalMapTex);
//    mesh->setSpecular(specular);
//    mesh->setTangent(tangent);
//
//    meshes.push_back(mesh);
//  }
//
//  diffuseTex = TGA::read("obj/african_head/african_head_eye_inner_diffuse.tga");
//  assert(diffuseTex);
//  diffuseTex->flipVertically();
//
//  normalMapTex = TGA::read("obj/african_head/african_head_eye_inner_nm.tga");
//  assert(normalMapTex);
//  normalMapTex->flipVertically();
//
//  specular = TGA::read("obj/african_head/african_head_eye_inner_spec.tga");
//  assert(specular);
//  specular->flipVertically();
//
//  tangent = TGA::read("obj/african_head/african_head_eye_inner_nm_tangent.tga");
//  assert(tangent);
//  tangent->flipVertically();
//
//  obj = Wavefront::read("obj/african_head/african_head_eye_inner.obj");
//  assert(obj);
//  for(auto mesh: obj->meshes())
//  {
//    mesh->setDiffuseTexture(diffuseTex);
//    mesh->setNormalMap(normalMapTex);
//    mesh->setSpecular(specular);
//    mesh->setTangent(tangent);
//
//    meshes.push_back(mesh);
//  }

  // DIABLO 3
//  auto diffuseTex = TGA::read("obj/diablo3_pose/diablo3_pose_diffuse.tga");
//  assert(diffuseTex);
//  diffuseTex->flipVertically();
//
//  auto normalMapTex = TGA::read("obj/diablo3_pose/diablo3_pose_nm.tga");
//  assert(normalMapTex);
//  normalMapTex->flipVertically();
//
//  auto specular = TGA::read("obj/diablo3_pose/diablo3_pose_spec.tga");
//  assert(specular);
//  specular->flipVertically();
//
//  auto tangent = TGA::read("obj/diablo3_pose/diablo3_pose_nm_tangent.tga");
//  assert(tangent);
//  tangent->flipVertically();
//
//  auto glow = TGA::read("obj/diablo3_pose/diablo3_pose_glow.tga");
//  assert(glow);
//  glow->flipVertically();
//
//  auto obj = Wavefront::read("obj/diablo3_pose/diablo3_pose.obj");
//  assert(obj);
//  for(auto mesh: obj->meshes())
//  {
//    mesh->setDiffuseTexture(diffuseTex);
//    mesh->setNormalMap(normalMapTex);
//    mesh->setSpecular(specular);
//    mesh->setTangent(tangent);
//    mesh->setGlowTexture(glow);
//
//    meshes.push_back(mesh);
//  }

  // FLOOR
//  diffuseTex = TGA::read("obj/floor_diffuse.tga");
//  assert(diffuseTex);
//  diffuseTex->flipVertically();
//
//  tangent = TGA::read("obj/floor_nm_tangent.tga");
//  assert(tangent);
//  tangent->flipVertically();
//
//  obj = Wavefront::read("obj/floor.obj");
//  assert(obj);
//  for(auto mesh: obj->meshes())
//  {
//    mesh->setDiffuseTexture(diffuseTex);
//    mesh->setTangent(tangent);
//
//    meshes.push_back(mesh);
//  }

  return meshes;
}

//--------------------------------------------------------------------
int main(int argc, char *argv[])
{
  auto threadsNum  = std::thread::hardware_concurrency()-4;
  short int width  = 2000;
  short int height = 2000;
  Vector3f eye   {5,5,10};
  Vector3f center{0.,2.5,0.};
  Vector3f up    {0,1,0};

  std::cout << "Using " << threadsNum << " threads." << std::flush;

  auto lightVector = Vector3f{-5.,10.,1.};
  Light = lightVector;
  viewport(width/8, height/8, width*3/4, height*3/4);
  projection(-1.f/(eye-center).norm());
  lookAt(eye, center, up);

  auto image   = std::make_shared<TGA>(width, height, Image::RGB);
  auto zBuffer = std::make_shared<Utils::zBuffer>(width, height);

  BlockTimer timer("Render");

  auto meshes = loadMeshes();

  // z-buffer generation pass
  std::cout << "start z-buffer pass" << std::endl << std::flush;
  for(auto current: meshes)
  {
    std::cout << "process " << current->id() << std::endl << std::flush;
    #pragma omp parallel for schedule(dynamic,1) num_threads(threadsNum)
    for (unsigned long i = 0; i < current->faces_num(); i++)
    {
      EmptyShader shader;
      shader.uniform_mesh = current;

      Vector3f screen_coords[3];
      for (int j = 0; j < 3; j++)
      {
        screen_coords[j] = shader.vertex(i, j);
      }

      triangle(screen_coords, shader, *zBuffer, *image);
    }
  }

  zBuffer->write("1-zBufferPass");

  // Screen space ambient occlusion pass
  std::cout << "start ambient pass" << std::endl << std::flush;
  auto ambientImage = std::make_shared<TGA>(width, height, Image::GRAYSCALE);
  auto zPtr = zBuffer->getBuffer(); // only reads, we can bypass mutex to execute faster.

  #pragma omp parallel for schedule(dynamic,1) num_threads(threadsNum)
  for (int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      if (zPtr[y * zBuffer->getWidth() + x] == -std::numeric_limits<float>::max()) continue;

      float total = 0;
      float angle = 0;
      for (int i = 0; i < 8; ++i, angle += PI_4)
      {
        total += PI_2 - max_elevation_angle(*zBuffer, Vector2f{x,y}, Vector2f{std::cos(angle), std::sin(angle)});
      }
      total /= (PI_2) * 8;

      const int value = std::min(255., std::max(0., total / (1 / 255.)));
      ambientImage->set(x, y, Images::Color(value, value, value));
    }
  }

  ambientImage->flipVertically();
  ambientImage->write("2-ambient");
//  auto ambientImage = Images::TGA::read("2-ambient.tga");
  ambientImage->flipVertically();
  zBuffer->clear();

  auto dBuffer = std::make_shared<Utils::zBuffer>(width, height);

  // light depth pass
  projection(-1.f/lightVector.norm());
  lookAt(lightVector, center, up);

  std::cout << "start light depth pass" << std::endl << std::flush;
  for(auto current: meshes)
  {
    std::cout << "process " << current->id() << std::endl << std::flush;
    #pragma omp parallel for schedule(dynamic,1) num_threads(threadsNum)
    for (unsigned long i = 0; i < current->faces_num(); i++)
    {
      EmptyShader shader;
      shader.uniform_mesh = current;

      Vector3f screen_coords[3];
      for (int j = 0; j < 3; j++)
      {
        screen_coords[j] = shader.vertex(i, j);
      }

      triangle(screen_coords, shader, *dBuffer, *image);
    }
  }

  dBuffer->write("3-depthPass");

  // final rendering pass
  auto ShadowTransform = ViewPort*Projection*ModelView;
  projection(-1.f/(eye-center).norm());
  lookAt(eye, center, up);

  // int pass = 0;
  std::cout << "start render pass" << std::endl << std::flush;
  for(auto current: meshes)
  {
    std::cout << "process " << current->id() << std::endl << std::flush;
    #pragma omp parallel for schedule(dynamic,1) num_threads(threadsNum)
    for (unsigned long i = 0; i < current->faces_num(); i++)
    {
      FinalShader shader;
      shader.uniform_transform_S = ShadowTransform;
      shader.uniform_ambient_image = ambientImage;
      shader.uniform_depthBuffer = dBuffer;
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
  image->write("4-output");

	return 0;
}
