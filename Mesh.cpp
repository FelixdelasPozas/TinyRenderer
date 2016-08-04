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

// C++
#include <cassert>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include <sstream>

//--------------------------------------------------------------------
Mesh::Mesh()
: m_diffuse  {nullptr}
, m_normalMap{nullptr}
, m_specular {nullptr}
, m_tangent  {nullptr}
{
}

//--------------------------------------------------------------------
std::shared_ptr<Mesh> Mesh::read_Wavefront(const std::string &filename)
{
  std::ifstream in;
  in.open(filename.c_str(), std::ifstream::in);

  if (in.fail()) return nullptr;

  auto mesh = new Mesh();

  std::string line;
  while (!in.eof())
  {
    std::getline(in, line);
    std::istringstream iss(line.c_str());
    char trash;
    if (!line.compare(0, 2, "v "))
    {
      iss >> trash;
      vertex v;
      for (int i = 0; i < 3; i++)
      {
        iss >> v[i];
      }

      mesh->addVertex(v);
    }
    else
    {
      if (!line.compare(0, 2, "f "))
      {
        face f;
        int vIdx, tIdx, nIdx;
        iss >> trash;
        while (iss >> vIdx >> trash >> tIdx >> trash >> nIdx)
        {
          // in wavefront obj all indices start at 1, not zero
          f._vertex.push_back(--vIdx);
          f._uv.push_back(--tIdx);
          f._normal.push_back(--nIdx);
        }

        mesh->addFace(f);
      }
      else
      {
        if(!line.compare(0, 4, "vt  "))
        {
          iss >> trash >> trash;
          uv t;
          for (int i = 0; i < 2; i++)
          {
            iss >> t[i];
          }

          mesh->adduv(t);
        }
        else
        {
          if(!line.compare(0, 4, "vn  "))
          {
            iss >> trash >> trash;
            normal v;
            for (int i = 0; i < 3; i++)
            {
              iss >> v[i];
            }

            mesh->addNormal(v);
          }
        }
      }
    }
  }

  std::cout << "read: " << filename << " vertices: " << mesh->m_vertices.size() << " faces: " << mesh->m_faces.size();
  std::cout << " uv: " << mesh->m_uv.size() << " normals: " << mesh->m_normals.size() << std::endl << std::flush;

  return std::shared_ptr<Mesh>{mesh};
}

//--------------------------------------------------------------------
bool Mesh::write_Wavefront(const std::string &filename)
{
  std::ofstream out;
  out.open(filename.c_str(), std::ofstream::out);

  if (out.fail()) return false;

  out << "# vertices list" << std::endl;
  for(auto v: m_vertices)
  {
    out << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
  }

  out << "# vt list" << std::endl;
  for(auto t: m_uv)
  {
    out << "vt  " << t[0] << " " << t[1] << " " << t[2] << std::endl;
  }

  out << "# normals list" << std::endl;
  for(auto n: m_normals)
  {
    out << "vn  " << n[0] << " " << n[1] << " " << n[2] << std::endl;
  }

  out << "# faces list" << std::endl;
  for(auto f: m_faces)
  {
    out << "f ";
    for(unsigned int i = 0; i < f._vertex.size(); ++i)
    {
      out << f._vertex[i] << "/" << f._uv[i] << " " << f._normal[i] << (i == f._vertex.size()-1 ? "" : " ");
    }
    out << std::endl;
  }

  out.flush();
  out.close();

  std::cout << "write: " << filename << " vertices: " << m_vertices.size() << " faces: " << m_faces.size() << std::endl << std::flush;

  return true;
}

//--------------------------------------------------------------------
void Mesh::addVertex(const double x, const double y, const double z)
{
  addVertex(vertex{x,y,z});
}

//--------------------------------------------------------------------
void Mesh::addVertex(const vertex &v)
{
  m_vertices.push_back(v);
}

//--------------------------------------------------------------------
void Mesh::addFace(const face &f)
{
  m_faces.push_back(f);
}

//--------------------------------------------------------------------
void Mesh::adduv(const double u, const double v)
{
  adduv(uv{u,v});
}

//--------------------------------------------------------------------
void Mesh::adduv(const uv &t)
{
  m_uv.push_back(t);
}

//--------------------------------------------------------------------
void Mesh::addNormal(const double x, const double y, const double z)
{
  addNormal(normal{x,y,z});
}

//--------------------------------------------------------------------
void Mesh::addNormal(const normal &n)
{
  m_normals.push_back(n);
}

//--------------------------------------------------------------------
void Mesh::setDiffuseTexture(std::shared_ptr<Images::Image> texture)
{
  m_diffuse = texture;
}

//--------------------------------------------------------------------
Images::Color Mesh::getDiffuse(const float u, const float v)
{
  return m_diffuse->get(u*m_diffuse->getWidth(), v*m_diffuse->getHeight());
}

//--------------------------------------------------------------------
Vector3f Mesh::getNormalMap(const float u, const float v)
{
  auto texColor = m_normalMap->get(u*m_normalMap->getWidth(), v*m_normalMap->getHeight());
  Vector4f vector{texColor.r, texColor.g, texColor.b, texColor.a};

  return vector.project().normalize();
}

//--------------------------------------------------------------------
float Mesh::getSpecular(const float u, const float v)
{
  auto color = m_specular->get(u*m_specular->getWidth(), v*m_specular->getHeight());

  return color.raw[0]/1.f;
}

//--------------------------------------------------------------------
Vector3f Mesh::getTangent(const float u, const float v)
{
  auto color = m_tangent->get(u*m_tangent->getWidth(), v*m_tangent->getHeight());

  Vector3f result;
  for (int i=0; i<3; i++)
  {
    result[i] = static_cast<float>(color.raw[i])/255.f*2.f - 1.f;
  }

  return result;
}
