/*
 File: Mesh.cpp
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
#include "Images.h"

// C++
#include <cassert>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <memory>
#include <sstream>

using namespace Images;

//--------------------------------------------------------------------
Mesh::Mesh(const std::string &id)
: m_id      {id}
, m_material{nullptr}
{
}

//--------------------------------------------------------------------
void Mesh::addVertex(const double x, const double y, const double z)
{
  addVertex(Vector3f{x,y,z});
}

//--------------------------------------------------------------------
void Mesh::addVertex(const Vector3f &v)
{
  m_vertices.push_back(v);
}

//--------------------------------------------------------------------
void Mesh::addFace(const Face &f)
{
  m_faces.push_back(f);
}

//--------------------------------------------------------------------
void Mesh::adduv(const double u, const double v)
{
  adduv(Vector2f{u,v});
}

//--------------------------------------------------------------------
void Mesh::adduv(const Vector2f &t)
{
  m_uv.push_back(t);
}

//--------------------------------------------------------------------
void Mesh::addNormal(const double x, const double y, const double z)
{
  addNormal(Vector3f{x,y,z});
}

//--------------------------------------------------------------------
void Mesh::addNormal(const Vector3f &n)
{
  m_normals.push_back(n);
}

//--------------------------------------------------------------------
Images::Color Mesh::getDiffuse(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::DIFFUSE);
  return texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));
}

//--------------------------------------------------------------------
Vector3f Mesh::getNormalMap(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::NORMAL);
  auto texColor = texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));
  Vector4f vector{texColor.r, texColor.g, texColor.b, texColor.a};

  return vector.project().normalize();
}

//--------------------------------------------------------------------
float Mesh::getSpecular(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::SPECULAR);
  auto color = texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));

  return color.raw[0]/1.f;
}

//--------------------------------------------------------------------
Vector3f Mesh::getTangent(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::NORMALTS);
  auto color = texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));
  assert(color.bytespp >= 3);

  Vector3f result{color.raw[2], color.raw[1], color.raw[0]};

  // shift vector from [0,1] to [-1,1]
  return (((result/255.) * 2.0) - Vector3f{1,1,1}).normalize();
}

//--------------------------------------------------------------------
Images::Color Mesh::getGlow(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::GLOW);
  return texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));
}

//--------------------------------------------------------------------
Images::Color Mesh::getSSS(const float u, const float v)
{
  auto texture = m_material->getTexture(m_mtl, Material::TYPE::SSS);
  return texture->get(u*(texture->getWidth()-1), v*(texture->getHeight()-1));
}

//--------------------------------------------------------------------
std::shared_ptr<Material> parseMaterials(const std::string &filename)
{
  auto material = std::make_shared<Material>();

  std::cout << "parse materials in " << filename << std::endl;

  std::string path;
  auto pos = filename.find_last_of('/');
  if(pos != std::string::npos)
  {
    path = filename.substr(0, pos+1);
  }

  std::ifstream in;
  in.open(filename.c_str(), std::ifstream::in);
  std::string materialId;
  if (!in.fail())
  {
    std::string line;
    while (!in.eof())
    {
      std::getline(in, line);
      std::istringstream iss(line.c_str());

      if (!line.compare(0, 7, "newmtl "))
      {
        materialId = line.substr(7, line.length()-7);
        continue;
      }

      if(!line.compare(0,3, "Ka ") || !line.compare(0,3, "Kd ") || !line.compare(0,3, "Ks "))
      {
        char trash;
        std::string key = line.substr(0,2);
        iss >> trash >> trash;

        Vector3f value;
        for(int i = 0; i < 3;++i)
        {
          iss >> value[i];
        }

        material->addProperty(materialId, key, value);
        continue;
      }

      if(!line.compare(0,7, "map_Ks ") || !line.compare(0,7, "map_kS "))
      {
        auto texture = line.substr(7, line.length()-7);
        if(texture.length() > 0)
        {
          auto filename = path + texture;

          material->addTexture(filename, Images::TGA::read(filename));
          material->addMaterialTexture(materialId, Material::TYPE::SPECULAR, filename);
        }
        continue;
      }

      if(!line.compare(0,7, "map_Kd ") || !line.compare(0,7, "map_kD "))
      {
        auto texture = line.substr(7, line.length()-7);
        if(texture.length() > 0)
        {
          auto filename = path + texture;

          material->addTexture(filename, Images::TGA::read(filename));
          material->addMaterialTexture(materialId, Material::TYPE::DIFFUSE, filename);
        }
        continue;
      }

      if(!line.compare(0,9, "map_Bump "))
      {
        auto texture = line.substr(9, line.length()-9);
        if(texture.length() > 0)
        {
          auto filename = path + texture;

          material->addTexture(filename, Images::TGA::read(filename));
          material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, filename);
        }
        continue;
      }

      if(!line.compare(0,5, "bump "))
      {
        auto texture = line.substr(5, line.length()-5);
        if(texture.length() > 0)
        {
          auto filename = path + texture;

          material->addTexture(filename, Images::TGA::read(filename));
          material->addMaterialTexture(materialId, Material::TYPE::NORMALTS, filename);
        }
        continue;
      }
    }
  }
  else
  {
    std::cout << "Couldn't read material file: " << filename << std::endl;
  }
  std::cout << std::flush;

  return material;
}

//--------------------------------------------------------------------
std::shared_ptr<Wavefront> Wavefront::read(const std::string& filename)
{
  auto object = std::shared_ptr<Wavefront>{new Wavefront(filename)};
  std::string mtllib;
  int id = 0;
  long int vNum = 0;
  long int uvNum = 0;
  long int nNum = 0;

  std::ifstream in;
  in.open(filename.c_str(), std::ifstream::in);

  if (!in.fail())
  {
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(std::to_string(id++));

    std::string line;
    while (!in.eof())
    {
      std::getline(in, line);
      std::istringstream iss(line.c_str());
      char trash;

      if (!line.compare(0, 2, "o "))
      {
        if(mesh != nullptr && mesh->vertex_num() != 0)
        {
          vNum += mesh->m_vertices.size();
          uvNum += mesh->m_uv.size();
          nNum += mesh->m_normals.size();

          object->m_meshes.push_back(mesh);
        }

        std::string idstring;
        iss >> trash >> idstring;

        mesh = std::make_shared<Mesh>(idstring);
        continue;
      }

      if(!line.compare(0, 7, "usemtl "))
      {
        auto material = line.substr(7, line.length()-7);

        mesh->setMaterialId(material);
        continue;
      }

      if(!line.compare(0, 7, "mtllib "))
      {
        mtllib = line.substr(7, line.length()-7);

        continue;
      }

      if (!line.compare(0, 2, "v "))
      {
        iss >> trash;
        Vector3f v;
        for (int i = 0; i < 3; i++)
        {
          iss >> v[i];
        }

        mesh->addVertex(v);
        continue;
      }

      if (!line.compare(0, 2, "f "))
      {
        Mesh::Face f;
        long int vIdx, tIdx, nIdx;
        iss >> trash;
        while (iss >> vIdx >> trash >> tIdx >> trash >> nIdx)
        {
          // in wavefront obj all indices start at 1, not zero
          f._vertex.push_back(std::abs(vIdx) - vNum - 1);
          f._uv.push_back(std::abs(tIdx) - uvNum - 1);
          f._normal.push_back(std::abs(nIdx) - nNum - 1);
        }

        mesh->addFace(f);
        continue;
      }

      if(!line.compare(0, 3, "vt "))
      {
        iss >> trash >> trash;
        Vector2f t;
        for (int i = 0; i < 2; i++)
        {
          iss >> t[i];
        }

        mesh->adduv(t);
        continue;
      }

      if (!line.compare(0, 3, "vn "))
      {
        iss >> trash >> trash;
        Vector3f v;
        for (int i = 0; i < 3; i++)
        {
          iss >> v[i];
        }

        mesh->addNormal(v);
        continue;
      }
    }

    if(mesh != nullptr && mesh->vertex_num() != 0)
    {
      object->m_meshes.push_back(mesh);
    }
  }
  else
  {
    std::cout << "Can't open " << filename << std::endl << std::flush;
    return nullptr;
  }

  std::cout << "processed: " << filename << std::endl;
  if(object->m_meshes.size() != 0)
  {
    if(mtllib != std::string())
    {
      auto pos = filename.find_last_of('/');
      if(pos != std::string::npos)
      {
        mtllib = filename.substr(0, pos+1) + mtllib;
      }

      auto material = parseMaterials(mtllib);

      object->setMaterial(material);
    }
    else
    {
      std::cout << "No materials file specified." << std::endl;
    }


    std::cout << "meshes: " << object->m_meshes.size() << std::endl;
    for(auto mesh: object->m_meshes)
    {
      std::cout << "mesh id: " << mesh->id() << " material: " << mesh->materialId() << " textures: " << (mesh->hasSpecular() ? "S" : "") << (mesh->hasNormalMap() ? "N" : "") << (mesh->hasTangent() ? "T" : "");
      std::cout << " vertices: " << mesh->m_vertices.size() << " faces: " << mesh->m_faces.size();
      std::cout << " uv: " << mesh->m_uv.size() << " normals: " << mesh->m_normals.size() << std::endl;
    }
  }
  else
  {
    std::cout << "No meshes!!!" << std::endl;
  }
  std::cout << std::flush;

  return object;
}

//--------------------------------------------------------------------
bool Wavefront::write(const std::string &filename)
{
  std::ofstream out;
  out.open(filename.c_str(), std::ofstream::out);

  if (out.fail()) return false;

  std::cout << "write: " << filename << std::endl;

  for(auto mesh: m_meshes)
  {
    out << "# mesh " << mesh->id() << std::endl;
    out << "o " << mesh->id() << std::endl << std::endl;

    out << "# vertices list" << std::endl;
    for(auto v: mesh->m_vertices)
    {
      out << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
    }
    out << std::endl;

    out << "# vt list" << std::endl;
    for(auto t: mesh->m_uv)
    {
      out << "vt  " << t[0] << " " << t[1] << " " << t[2] << std::endl;
    }
    out << std::endl;

    out << "# normals list" << std::endl;
    for(auto n: mesh->m_normals)
    {
      out << "vn  " << n[0] << " " << n[1] << " " << n[2] << std::endl;
    }
    out << std::endl;

    out << "# faces list" << std::endl;
    for(auto f: mesh->m_faces)
    {
      out << "f ";
      for(unsigned int i = 0; i < f._vertex.size(); ++i)
      {
        out << f._vertex[i] << "/" << f._uv[i] << " " << f._normal[i] << (i == f._vertex.size()-1 ? "" : " ");
      }
      out << std::endl;
    }
    out << std::endl;

    if(mesh->materialId() != std::string())
    {
      out << "# material" << std::endl;
      out << "usemtl " << mesh->materialId() << std::endl;
    }

    out << std::endl;

    std::cout << "mesh id: " << mesh->id() << " material: " << (mesh->materialId() == std::string() ? "none":mesh->materialId());
    std::cout << " vertices: " << mesh->m_vertices.size() << " faces: " << mesh->m_faces.size() << std::endl;
  }

  std::cout << std::flush;

  out.flush();
  out.close();

  return true;
}

//--------------------------------------------------------------------
void Wavefront::setMaterial(std::shared_ptr<Material> material)
{
  m_material = material;

  for(auto mesh: m_meshes)
  {
    mesh->setMaterial(material);
  }
}

//--------------------------------------------------------------------
void Material::addTexture(const std::string &filename, const std::shared_ptr<Images::Image> texture)
{
  texture->flipVertically();
  m_textures[filename] = texture;
}

//--------------------------------------------------------------------
void Material::addProperty(const std::string& materialId, const std::string& key, const Vector3f& value)
{
  m_properties[materialId][key] = value;
}

void Material::addMaterialTexture(const std::string &materialId, const TYPE type, const std::string &textureId)
{
  m_materials[materialId][static_cast<int>(type)] = textureId;
}

//--------------------------------------------------------------------
std::shared_ptr<Images::Image> Material::getTexture(const std::string& id, const TYPE type) const
{
  assert(m_materials.find(id) != m_materials.end());

  auto map = m_materials.at(id);

  assert(map.find(static_cast<int>(type)) != map.end());

  return m_textures.at(map.at(static_cast<int>(type)));
}

//--------------------------------------------------------------------
const Vector3f Material::getProperty(const std::string& id, const std::string& key) const
{
  assert(m_properties.find(id) != m_properties.end());

  auto map = m_properties.at(id);

  assert(map.find(key) != map.end());

  return map.at(key);
}

//--------------------------------------------------------------------
bool Material::hasTexture(const std::string &materialId, const TYPE type) const
{
  auto it = m_materials.find(materialId);

  if(it == m_materials.end()) return false;

  auto map = (*it).second;

  return (map.find(static_cast<int>(type)) != map.end());
}
