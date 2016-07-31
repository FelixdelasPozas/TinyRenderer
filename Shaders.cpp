/*
 File: Shaders.cpp
 Created on: 30 jul. 2016
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
#include <Shaders.h>
#include <Images.h>
#include <Mesh.h>

using namespace Images;

//--------------------------------------------------------------------
Vector3f GouraudShader::vertex(int iface, int nthvert)
{
  auto vertexId = m_mesh->getFaceVertices(iface)[nthvert];                       // get vertex id
  varying_intensity[nthvert] = std::max(0.f, m_mesh->getNormal(vertexId)*Light); // get diffuse lighting intensity
  auto gl_Vertex = m_mesh->getVertex(vertexId).augment();                        // read the vertex from .obj file

  return (transform*gl_Vertex).project();                                        // transform it to screen coordinates
}

//--------------------------------------------------------------------
bool GouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  float intensity = varying_intensity * baricentric; // interpolate intensity for the current pixel
  color = Color(255,255,255) * intensity;            // well duh
  return false;                                      // no, we do not discard this pixel
}

//--------------------------------------------------------------------
bool CellShader::fragment(Vector3f baricentric, Images::Color &color)
{
  float intensity = varying_intensity * baricentric; // interpolate intensity for the current pixel

  const float interval = 1. / shades;
  intensity = static_cast<int>(intensity/interval) * interval;

  color = baseColor;
  color = color * intensity;

  return false;
}

//--------------------------------------------------------------------
Vector3f TexturedGouraudShader::vertex(int iface, int nthvert)
{
  auto vertexId = m_mesh->getFaceVertices(iface)[nthvert];                       // get vertex id

  varying_uv_index[nthvert]  = m_mesh->getuvIds(iface)[nthvert];                 // get uv indexes
  varying_intensity[nthvert] = std::max(0.f, m_mesh->getNormal(vertexId)*Light); // get diffuse lighting intensity
  auto gl_Vertex = m_mesh->getVertex(vertexId).augment();                        // read the vertex from .obj file

  return (transform*gl_Vertex).project();                                        // transform it to screen coordinates
}

//--------------------------------------------------------------------
bool TexturedGouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  Vector2f vectoruv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) vectoruv += m_mesh->getuv(varying_uv_index[i])*baricentric[i];

  float intensity = varying_intensity * baricentric;                // interpolate intensity for the current pixel
  color = m_mesh->getDiffuse(vectoruv[0], vectoruv[1]) * intensity; // well duh
  return false;                                                     // no, we do not discard this pixel
}
