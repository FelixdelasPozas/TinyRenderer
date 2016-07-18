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
        int itrash, idx;
        iss >> trash;
        while (iss >> idx >> trash >> itrash >> trash >> itrash)
        {
          idx--; // in wavefront obj all indices start at 1, not zero
          f.push_back(idx);
        }

        mesh->addFace(f);
      }
    }
  }

  std::cout << "read: " << filename << " vertices: " << mesh->m_vertices.size() << " faces: " << mesh->m_faces.size() << std::endl << std::flush;

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

  out << "# faces list" << std::endl;
  for(auto f: m_faces)
  {
    out << "f ";
    for(auto idx: f)
    {
      out << idx << " ";
    }
    out << std::endl;
  }

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
void Mesh::addFace(const unsigned long v1, const unsigned long v2, const unsigned long v3)
{
  assert(v1 < m_vertices.size() && v2 < m_vertices.size() && v3 < m_vertices.size());

  addFace(face{v1,v2,v3});
}

//--------------------------------------------------------------------
void Mesh::addFace(const face &f)
{
  m_faces.push_back(f);
}
