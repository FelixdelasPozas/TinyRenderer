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

auto minmax = [](float value) { return std::min(1.f, std::max(0.f, value)); };

//--------------------------------------------------------------------
Vector3f GouraudShader::vertex(int iface, int nthvert)
{
  auto vertexId = uniform_mesh->getFaceVertices(iface)[nthvert];                      // get vertex id
  auto intensity = Light.normalize() * uniform_mesh->getNormal(vertexId).normalize(); // get diffuse lighting intensity
  varying_intensity[nthvert] = minmax(intensity);
  auto vertex = uniform_mesh->getVertex(vertexId).augment();                          // read the vertex from .obj file

  return (uniform_transform*vertex).project();                                        // transform it to screen coordinates
}

//--------------------------------------------------------------------
bool GouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  float intensity = minmax(varying_intensity * baricentric); // interpolate intensity for the current pixel
  color = Color(255,255,255) * intensity;                    // well duh
  return false;                                              // no, we do not discard this pixel
}

//--------------------------------------------------------------------
bool CellShader::fragment(Vector3f baricentric, Images::Color &color)
{
  float intensity = varying_intensity * baricentric; // interpolate intensity for the current pixel

  const float interval = 1.f / varying_shades;
  intensity = static_cast<int>(intensity/interval) * interval;

  color = varying_baseColor;
  color = color * intensity;

  return false;
}

//--------------------------------------------------------------------
Vector3f TexturedGouraudShader::vertex(int iface, int nthvert)
{
  varying_uv_index[nthvert] = uniform_mesh->getuvIds(iface)[nthvert]; // get uv indexes

  return GouraudShader::vertex(iface, nthvert);
}

//--------------------------------------------------------------------
bool TexturedGouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  Vector2f vectoruv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) vectoruv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  float intensity = minmax(varying_intensity * baricentric); // interpolate intensity for the current pixel
  color = uniform_mesh->getDiffuse(vectoruv) * intensity;    // well duh
  return false;                                              // no, we do not discard this pixel
}

//--------------------------------------------------------------------
bool NormalMapping::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal = uniform_mesh->getNormalMap(uv);

  auto n = (uniform_transform_TI * normal.augment()).project().normalize();
  auto l = (uniform_transform * Light.augment()).project().normalize();
  float intensity = minmax(n*l);

  color = Color(255,255,255)*intensity; // well duh
  return false;
}

//--------------------------------------------------------------------
bool TexturedNormalMapping::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal = uniform_mesh->getNormalMap(uv);

  auto n = (uniform_transform_TI * normal.augment()).project().normalize();
  auto l = (uniform_transform * Light.augment()).project().normalize();
  float intensity = minmax(n*l);

  color = uniform_mesh->getDiffuse(uv)*intensity; // well duh
  return false;
}

//--------------------------------------------------------------------
Vector3f MultiShader::vertex(int iface, int nthvert)
{
  if(varying_vertexi == 3) varying_vertexi = 0;

  auto vertexId = uniform_mesh->getFaceVertices(iface)[nthvert];                      // get vertex id
  auto vertex = uniform_mesh->getVertex(vertexId).augment();                          // read the vertex from .obj file

  varying_svertex[varying_vertexi++] = (uniform_transform*vertex).project();          // transform it to screen coordinates

  for(auto shader: uniform_shaders) shader->vertex(iface, nthvert);

  return varying_svertex[varying_vertexi - 1];
}

//--------------------------------------------------------------------
bool MultiShader::fragment(Vector3f baricentric, Images::Color& color)
{
  assert(varying_vertexi == 3);

  Vector2f xy;
  for(int i = 0; i < 3; ++i)
  {
    xy[0] += varying_svertex[i][0] * baricentric[i];
    xy[1] += varying_svertex[i][1] * baricentric[i];
  }

  xy = xy / uniform_interval;

  // squares
  // varying_selector = ((static_cast<int>(xy[0]) % uniform_shaders.size()) + static_cast<int>(xy[1])) % uniform_shaders.size();

  // diagonal stripes
  varying_selector = static_cast<int>(xy[0] + xy[1]) % uniform_shaders.size();

  return uniform_shaders[varying_selector]->fragment(baricentric, color);
}

//--------------------------------------------------------------------
bool TexturedSpecularShader::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal    = uniform_mesh->getNormalMap(uv);
  auto n         = (uniform_transform_TI * normal.augment()).project().normalize();
  auto l         = (uniform_transform * Light.augment()).project().normalize();
  auto reflected = (n*(2.f*n*l) - l).normalize();
  float specular = std::pow(std::max(reflected[2], 0.0f), uniform_mesh->getSpecular(uv));
  float diffuse  = minmax(n*l);
  auto dColor    = uniform_mesh->getDiffuse(uv);
  color = dColor;

  for (int i=0; i < color.bytespp; i++)
  {
    int value = uniform_ambient_coeff*uniform_ambient_value + static_cast<int>(dColor.raw[i])*(uniform_diffuse_coeff*diffuse + uniform_specular_coeff*specular);
    color.raw[i] = std::min(value, 255);
  }

  return false;
}

//--------------------------------------------------------------------
Vector3f PhongShader::vertex(int iface, int nthvert)
{
  auto vertexId = uniform_mesh->getFaceVertices(iface)[nthvert];

  varying_uv_index[nthvert] = uniform_mesh->getuvIds(iface)[nthvert];
  varying_normals[nthvert]  = vertexId;

  auto vertex = uniform_mesh->getVertex(vertexId).augment();
  return (uniform_transform*vertex).project();
}

//--------------------------------------------------------------------
bool PhongShader::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv; // final uv coordinates
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  Vector3f normal;
  for(unsigned int i = 0; i < 3; ++i) normal += uniform_mesh->getNormal(varying_normals[i])*baricentric[i];
  normal.normalize();

  float diffuse = minmax(normal*Light);
  color = uniform_mesh->getDiffuse(uv)*diffuse;

  return false;
}

//--------------------------------------------------------------------
Vector3f DarbouxNormalShader::vertex(int iface, int nthvert)
{
  auto vertexId = uniform_mesh->getFaceVertices(iface)[nthvert];
  varying_uv_index[nthvert] = uniform_mesh->getuvIds(iface)[nthvert];
  varying_normals[nthvert]  = (uniform_transform_TI * uniform_mesh->getNormal(vertexId).augment()).project();
  varying_vertex[nthvert] = (uniform_transform * uniform_mesh->getVertex(vertexId).augment()).project();

  return varying_vertex[nthvert];
}

//--------------------------------------------------------------------
bool DarbouxNormalShader::fragment(Vector3f baricentric, Images::Color& color)
{
  auto nb = (varying_normals[0] * baricentric[0] + varying_normals[1] * baricentric[1] + varying_normals[2] * baricentric[2]).normalize();

  auto uv0 = uniform_mesh->getuv(varying_uv_index[0]);
  auto uv1 = uniform_mesh->getuv(varying_uv_index[1]);
  auto uv2 = uniform_mesh->getuv(varying_uv_index[2]);

  auto uv = uv0 * baricentric[0] + uv1 * baricentric[1] + uv2 * baricentric[2]; // final uv coordinates

  Matrix3f A;
  A[0] = varying_vertex[1] - varying_vertex[0];
  A[1] = varying_vertex[2] - varying_vertex[0];
  A[2] = nb;

  auto AI = A.inverse();

  auto i = AI * Vector3f{uv1[0] - uv0[0], uv2[0] - uv0[0], 0.f};
  auto j = AI * Vector3f{uv1[1] - uv0[1], uv2[1] - uv0[1], 0.f};

  Matrix3f B;
  B[0] = i.normalize();
  B[1] = j.normalize();
  B[2] = nb;
  B = B.transpose();

  auto l = Light;
  auto n = (B * uniform_mesh->getTangent(uv)).normalize();
  auto diff = minmax(n * l);

  color = uniform_mesh->getDiffuse(uv) * diff;

  return false;
}
