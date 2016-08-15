/*
 File: Utils.cpp
 Created on: 19 jul. 2016
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
#include <Images.h>
#include <Utils.h>
#include <GL_Impl.h>
#include <Mesh.h>

// C++
#include <iostream>
#include <algorithm>

using namespace Images;
using namespace GL_Impl;

//--------------------------------------------------------------------
Utils::BlockTimer::BlockTimer(const std::string& id)
: m_id       {id}
, m_startTime{std::chrono::high_resolution_clock::now()}
{
}

//--------------------------------------------------------------------
Utils::BlockTimer::~BlockTimer()
{
  auto endTime = std::chrono::high_resolution_clock::now();
  auto count   = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_startTime).count();

  std::cout << "\"" << m_id << "\" block time: " << count << " milliseconds (" << count/1000 << " seconds)."<< std::endl << std::flush;
}

//--------------------------------------------------------------------
Utils::zBuffer::zBuffer(const short width, const short height)
: m_width {width}
, m_height{height}
, m_min   {std::numeric_limits<float>::max()}
, m_max   {-std::numeric_limits<float>::max()}
{
  assert(width > 0 && height > 0);

  m_data = new float[width*height];

  clear();
}

//--------------------------------------------------------------------
Utils::zBuffer::zBuffer(const zBuffer& buffer)
: m_width{buffer.m_width}
, m_height{buffer.m_height}
, m_min{buffer.m_min}
, m_max{buffer.m_max}
{
  m_data = new float[m_width*m_height];

  std::copy_n(buffer.m_data, m_width*m_height, m_data);
}

//--------------------------------------------------------------------
Utils::zBuffer::~zBuffer()
{
  delete [] m_data;
}

//--------------------------------------------------------------------
double Utils::zBuffer::get(const unsigned short x, const unsigned short y)
{
  assert(x < m_width && y < m_height);
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_data[y*m_width + x];
}

//--------------------------------------------------------------------
void Utils::zBuffer::set(const unsigned short x, const unsigned short y, double value)
{
  assert(x < m_width && y < m_height);
  std::lock_guard<std::mutex> lock(m_mutex);

  if(m_min > value) m_min = value;
  if(m_max < value) m_max = value;

  m_data[y*m_width + x] = value;
}

//--------------------------------------------------------------------
bool Utils::zBuffer::checkAndSet(const unsigned short x, const unsigned short y, float value)
{
  assert(x < m_width && y < m_height);
  std::lock_guard<std::mutex> lock(m_mutex);

  if(m_data[y*m_width + x] < value)
  {
    if(m_min > value) m_min = value;
    if(m_max < value) m_max = value;

    m_data[y*m_width + x] = value;
    return true;
  }

  return false;
}

//--------------------------------------------------------------------
unsigned short Utils::zBuffer::getWidth() const
{
  return m_width;
}

//--------------------------------------------------------------------
unsigned short Utils::zBuffer::getHeight() const
{
  return m_height;
}

//--------------------------------------------------------------------
float Utils::zBuffer::getMinimum() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_min;
}

//--------------------------------------------------------------------
float Utils::zBuffer::getMaximum() const
{
  std::lock_guard<std::mutex> lock(m_mutex);

  return m_max;
}

//--------------------------------------------------------------------
void Utils::zBuffer::write(const std::string& filename)
{
  auto image = std::make_shared<Images::TGA>(m_width, m_height, Image::GRAYSCALE);
  auto min   = std::fabs(m_min);
  auto delta = 256./(m_max + min);

  for(unsigned short x = 0; x < m_width; ++x)
  {
    for(unsigned short y = 0; y < m_height; ++y)
    {
      auto value = get(x,y) + min;
      if(value == -std::numeric_limits<double>::max()) continue;
      image->set(x, y, Color(value*delta, Image::GRAYSCALE));
    }
  }

  image->flipVertically();
  image->write(filename);
}

//--------------------------------------------------------------------
bool Utils::dumpTexture(std::shared_ptr<Mesh> mesh, const std::string &filename)
{
  auto texture = mesh->material()->getTexture(mesh->materialId(), Material::TYPE::DIFFUSE);
  auto width   = texture->getWidth();
  auto height  = texture->getHeight();
  auto white   = Color(255,255,255);

  #pragma omp parallel for
  for (unsigned long i = 0; i < mesh->faces_num(); i++)
  {
    auto uvs  = mesh->getFaceUVIds(i);
    Vector2f uv_coords[3];

    for (int j: {0,1,2})
    {
      uv_coords[j] = mesh->getuv(uvs[j]);
    }

    for (int j: {0,1,2})
    {
      auto uv1 = uv_coords[j];
      auto uv2 = uv_coords[(j+1)%3];

      line(uv1[0]*width, uv1[1]*height, uv2[0]*width, uv2[1]*height, *texture, white);
    }
  }

  texture->flipVertically();
  return texture->write(filename);
}

//--------------------------------------------------------------------
void Utils::zBuffer::clear()
{
  std::fill_n(m_data, m_width*m_height, -std::numeric_limits<float>::max());
}

//--------------------------------------------------------------------
std::shared_ptr<Wavefront> Utils::africanHead()
{
  auto object = std::make_shared<Wavefront>();
  auto material = std::make_shared<Material>();

  std::string materialId = "african_head";

  std::string id = "obj/african_head/african_head_diffuse.tga";
  auto diffuseTex = TGA::read(id);
  assert(diffuseTex);
  material->addTexture(id, diffuseTex);
  material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, id);

  id = "obj/african_head/african_head_nm.tga";
  auto normalMapTex = TGA::read(id);
  assert(normalMapTex);
  material->addTexture(id, normalMapTex);
  material->addMaterialTexture(materialId, Material::TYPE::NORMAL, id);

  id = "obj/african_head/african_head_spec.tga";
  auto specular = TGA::read(id);
  assert(specular);
  material->addTexture(id, specular);
  material->addMaterialTexture(materialId, Material::TYPE::SPECULAR, id);

  id = "obj/african_head/african_head_nm_tangent.tga";
  auto tangent = TGA::read(id);
  assert(tangent);
  material->addTexture(id, tangent);
  material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, id);

  auto temp = Wavefront::read("obj/african_head/african_head.obj");
  assert(temp);
  for(auto mesh: temp->meshes())
  {
    mesh->setMaterialId(materialId);
    object->addMesh(mesh);
  }

  materialId = "african_head_eye";

  id = "obj/african_head/african_head_eye_inner_diffuse.tga";
  diffuseTex = TGA::read(id);
  assert(diffuseTex);
  material->addTexture(id, diffuseTex);
  material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, id);

  id = "obj/african_head/african_head_eye_inner_nm.tga";
  normalMapTex = TGA::read(id);
  assert(normalMapTex);
  material->addTexture(id, normalMapTex);
  material->addMaterialTexture(materialId, Material::TYPE::NORMAL, id);

  id = "obj/african_head/african_head_eye_inner_spec.tga";
  specular = TGA::read(id);
  assert(specular);
  material->addTexture(id, specular);
  material->addMaterialTexture(materialId, Material::TYPE::SPECULAR, id);

  id = "obj/african_head/african_head_eye_inner_nm_tangent.tga";
  tangent = TGA::read(id);
  assert(tangent);
  material->addTexture(id, tangent);
  material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, id);

  temp = Wavefront::read("obj/african_head/african_head_eye_inner.obj");
  assert(temp);
  for (auto mesh : temp->meshes())
  {
    mesh->setMaterialId(materialId);
    object->addMesh(mesh);
  }

  materialId = "floor";

  id = "obj/floor_diffuse.tga";
  diffuseTex = TGA::read(id);
  assert(diffuseTex);
  material->addTexture(id, diffuseTex);
  material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, id);

  id = "obj/floor_nm_tangent.tga";
  tangent = TGA::read(id);
  assert(tangent);
  material->addTexture(id, tangent);
  material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, id);

  temp = Wavefront::read("obj/floor.obj");
  assert(temp);
  for (auto mesh : temp->meshes())
  {
    mesh->setMaterialId(materialId);
    object->addMesh(mesh);
  }


  object->setMaterial(material);

  return object;
}

//--------------------------------------------------------------------
std::shared_ptr<Wavefront> Utils::diablo()
{
  auto object = std::make_shared<Wavefront>();
  auto material = std::make_shared<Material>();

  std::string materialId = "diablo";

  std::string id = "obj/diablo3_pose/diablo3_pose_diffuse.tga";
  auto diffuseTex = TGA::read(id);
  assert(diffuseTex);
  material->addTexture(id, diffuseTex);
  material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, id);

  id = "obj/diablo3_pose/diablo3_pose_nm.tga";
  auto normalMapTex = TGA::read(id);
  assert(normalMapTex);
  material->addTexture(id, normalMapTex);
  material->addMaterialTexture(materialId, Material::TYPE::NORMAL, id);

  id = "obj/diablo3_pose/diablo3_pose_spec.tga";
  auto specular = TGA::read(id);
  assert(specular);
  material->addTexture(id, specular);
  material->addMaterialTexture(materialId, Material::TYPE::SPECULAR, id);

  id = "obj/diablo3_pose/diablo3_pose_nm_tangent.tga";
  auto tangent = TGA::read(id);
  assert(tangent);
  material->addTexture(id, tangent);
  material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, id);

  id = "obj/diablo3_pose/diablo3_pose_glow.tga";
  auto glow = TGA::read(id);
  assert(glow);
  material->addTexture(id, glow);
  material->addMaterialTexture(materialId, Material::TYPE::GLOW, id);

  auto obj = Wavefront::read("obj/diablo3_pose/diablo3_pose.obj");
  assert(obj);
  for (auto mesh : obj->meshes())
  {
    mesh->setMaterialId(materialId);
    object->addMesh(mesh);
  }

  object->setMaterial(material);

  return object;
}

//--------------------------------------------------------------------
std::shared_ptr<Wavefront> Utils::floor()
{
  auto object = std::make_shared<Wavefront>();
  auto material = std::make_shared<Material>();

  std::string materialId = "floor";

  std::string id = "obj/floor_diffuse.tga";
  auto diffuseTex = TGA::read(id);
  assert(diffuseTex);
  material->addTexture(id, diffuseTex);
  material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, id);

  id = "obj/floor_nm_tangent.tga";
  auto tangent = TGA::read(id);
  assert(tangent);
  material->addTexture(id, tangent);
  material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, id);

  auto obj = Wavefront::read("obj/floor.obj");
  assert(obj);
  for (auto mesh : obj->meshes())
  {
    mesh->setMaterialId(materialId);
    object->addMesh(mesh);
  }

  object->setMaterial(material);

  return object;
}
