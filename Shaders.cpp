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

auto minmax01 = [](float value) { return std::min(1.f, std::max(0.f, value)); };

//--------------------------------------------------------------------
Vector3f GouraudShader::vertex(int iface, int nthvert)
{
  const auto vId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  const auto nId = uniform_mesh->getFaceNormals(iface)[nthvert];
  const auto l   = (uniform_transform * Light.normalize().augment(0)).project(false).normalize();
  const auto n   = (uniform_transform_TI * (uniform_mesh->getNormal(nId).normalize()).augment(0)).project(false).normalize();

  varying_intensity[nthvert] = minmax01(n*l);
  const auto v = uniform_mesh->getVertex(vId).augment();

  return (uniform_transform * v).project();
}

//--------------------------------------------------------------------
bool GouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  const auto intensity = minmax01(varying_intensity * baricentric);
  color = Color(255,255,255) * intensity;

  return false;
}

//--------------------------------------------------------------------
bool CellShader::fragment(Vector3f baricentric, Images::Color &color)
{
  auto intensity = varying_intensity * baricentric;

  const auto interval = 1.f / varying_shades;
  intensity = static_cast<int>(intensity/interval) * interval;

  color = varying_baseColor * intensity;

  return false;
}

//--------------------------------------------------------------------
Vector3f TexturedGouraudShader::vertex(int iface, int nthvert)
{
  varying_uv_index[nthvert] = uniform_mesh->getFaceUVIds(iface)[nthvert];

  return GouraudShader::vertex(iface, nthvert);
}

//--------------------------------------------------------------------
bool TexturedGouraudShader::fragment(Vector3f baricentric, Images::Color &color)
{
  Vector2f uv;
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  const auto intensity = minmax01(varying_intensity * baricentric);
  color = uniform_mesh->getDiffuse(uv) * intensity;

  return false;
}

//--------------------------------------------------------------------
bool NormalMapping::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv;
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal = Vector3f{0,1,0};
  if(uniform_mesh->hasNormalMap())
  {
    normal = uniform_mesh->getNormalMap(uv);
  }

  const auto n = (uniform_transform_TI * normal.augment(0)).project(false).normalize();
  const auto l = (uniform_transform * Light.augment(0)).project(false).normalize();
  const auto intensity = minmax01(n*l);

  color = Color(255,255,255)*intensity;
  return false;
}

//--------------------------------------------------------------------
bool TexturedNormalMapping::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv;
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal = Vector3f{0,1,0};
  if(uniform_mesh->hasNormalMap())
  {
    normal = uniform_mesh->getNormalMap(uv);
  }

  const auto n = (uniform_transform_TI * normal.augment(0)).project(false).normalize();
  const auto l = (uniform_transform * Light.augment(0)).project(false).normalize();
  const auto intensity = minmax01(n*l);

  color = uniform_mesh->getDiffuse(uv)*intensity;
  return false;
}

//--------------------------------------------------------------------
Vector3f MultiShader::vertex(int iface, int nthvert)
{
  if(varying_vertexi == 3) varying_vertexi = 0;

  const auto vertexId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  const auto vertex = uniform_mesh->getVertex(vertexId).augment();

  varying_svertex[varying_vertexi++] = (uniform_transform*vertex).project();

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
  Vector2f uv;
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  auto normal = Vector3f{0,1,0};
  if(uniform_mesh->hasNormalMap())
  {
    normal = uniform_mesh->getNormalMap(uv);
  }

  const auto n       = (uniform_transform_TI * normal.augment(0)).project(false).normalize();
  const auto l       = (uniform_transform * Light.augment(0)).project(false).normalize();
  const auto diffuse = minmax01(n*l);
  color = uniform_mesh->getDiffuse(uv);

  float specular = 0;
  if(uniform_mesh->hasSpecular())
  {
    const auto reflected = (n*(n*l*2.f) - l).normalize();
    specular = std::pow(std::max(reflected[2], 0.0f), uniform_mesh->getSpecular(uv));
  }

  for (int i=0; i < color.bytespp; i++)
  {
    const int value = uniform_ambient_coeff*uniform_ambient_value + static_cast<int>(color.raw[i])*(uniform_diffuse_coeff*diffuse + uniform_specular_coeff*specular);
    color.raw[i] = std::min(value, 255);
  }

  return false;
}

//--------------------------------------------------------------------
Vector3f PhongShader::vertex(int iface, int nthvert)
{
  const auto vertexId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  varying_uv_index[nthvert] = uniform_mesh->getFaceUVIds(iface)[nthvert];
  varying_normals[nthvert]  = uniform_mesh->getFaceNormals(iface)[nthvert];
  const auto vertex = uniform_mesh->getVertex(vertexId).augment();

  return (uniform_transform*vertex).project();
}

//--------------------------------------------------------------------
bool PhongShader::fragment(Vector3f baricentric, Images::Color& color)
{
  Vector2f uv;
  for(unsigned int i = 0; i < 3; ++i) uv += uniform_mesh->getuv(varying_uv_index[i])*baricentric[i];

  Vector3f normal;
  for(unsigned int i = 0; i < 3; ++i) normal += uniform_mesh->getNormal(varying_normals[i])*baricentric[i];
  normal.normalize();

  auto n = (uniform_transform_TI * normal.augment(0)).project(false).normalize();
  auto l = (uniform_transform * Light.augment(0)).project(false).normalize();

  const auto diffuse = minmax01(n*l);
  color = uniform_mesh->getDiffuse(uv)*diffuse;

  return false;
}

//--------------------------------------------------------------------
Vector3f DarbouxNormalShader::vertex(int iface, int nthvert)
{
  const auto vertexId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  const auto normalId = uniform_mesh->getFaceNormals(iface)[nthvert];

  varying_uv_index[nthvert] = uniform_mesh->getFaceUVIds(iface)[nthvert];
  varying_normals.setColumn(nthvert, (uniform_transform_TI * uniform_mesh->getNormal(normalId).augment(0)).project(false));
  varying_vertex[nthvert] = (uniform_transform * uniform_mesh->getVertex(vertexId).augment()).project();

  return varying_vertex[nthvert];
}

//--------------------------------------------------------------------
bool DarbouxNormalShader::fragment(Vector3f baricentric, Images::Color& color)
{
  const auto nb = (varying_normals * baricentric).normalize();

  auto uv0 = uniform_mesh->getuv(varying_uv_index[0]);
  auto uv1 = uniform_mesh->getuv(varying_uv_index[1]);
  auto uv2 = uniform_mesh->getuv(varying_uv_index[2]);

  auto uv = (uv0 * baricentric[0]) + (uv1 * baricentric[1]) + (uv2 * baricentric[2]);

  Matrix3f A;
  A[0] = varying_vertex[1] - varying_vertex[0];
  A[1] = varying_vertex[2] - varying_vertex[0];
  A[2] = nb;

  auto AI = A.inverse();

  auto i = AI * Vector3f{uv1[0] - uv0[0], uv2[0] - uv0[0], 0.f};
  auto j = AI * Vector3f{uv1[1] - uv0[1], uv2[1] - uv0[1], 0.f};

  Matrix3f B;
  B.setColumn(0, i.normalize());
  B.setColumn(1, j.normalize());
  B.setColumn(2, nb);

  const auto l       = (uniform_transform * Light.augment(0)).project(false).normalize();
  const auto n       = (uniform_transform_TI * (B * uniform_mesh->getTangent(uv)).augment(0)).project(false).normalize();
  const auto diffuse = minmax01(n*l);
  auto specular      = 0.f;
  color              = uniform_mesh->getDiffuse(uv);

  if(color.bytespp == 4 && color.a == 0) return true;

  if(diffuse != 0 && uniform_mesh->hasSpecular())
  {
    const auto exp       = uniform_mesh->getSpecular(uv) + 5; // why this +5 fixes specular in diablo?? computations seems right.
    const auto reflected = (n*(n*l*2.f) - l).normalize();
    const auto base      = std::max(reflected[2], 0.f);
    specular = minmax01(std::pow(base, exp));
  }

  Matrix3f transformed;
  for(int i = 0; i < 3; ++i)
  {
    transformed[i] = (ViewPort * varying_vertex[i].augment()).project();
  }
  auto vertex = transformed.transpose() * baricentric;

  auto x = static_cast<unsigned short>(vertex[0]);
  auto y = static_cast<unsigned short>(vertex[1]);
  if(0 <= x && x < uniform_ambient_image->getWidth() && 0 <= y && y < uniform_ambient_image->getHeight())
  {
    varying_ambient_value = uniform_ambient_image->get(x,y).raw[0];
  }
  else
  {
    varying_ambient_value = 50;
  }

  auto light_coeff = uniform_diffuse_coeff*diffuse + uniform_specular_coeff*specular;
  auto ambient = uniform_ambient_coeff*varying_ambient_value;
  color = ((color * light_coeff) + ambient);

  if(uniform_mesh->hasGlow())
  {
    color += uniform_mesh->getGlow(uv) * uniform_glow_coeff;
  }

  return false;
}

//--------------------------------------------------------------------
Vector3f EmptyShader::vertex(int iface, int nthvert)
{
  const auto vertexId = uniform_mesh->getFaceVertexId(iface, nthvert);

  return (uniform_transform * uniform_mesh->getVertex(vertexId).augment()).project();
}

//--------------------------------------------------------------------
Vector3f HardShadowsShader::vertex(int iface, int nthvert)
{
  const auto vertexId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  varying_dVertex.setColumn(nthvert, (uniform_transform_S * uniform_mesh->getVertex(vertexId).augment()).project());

  return DarbouxNormalShader::vertex(iface, nthvert);
}

//--------------------------------------------------------------------
bool HardShadowsShader::fragment(Vector3f baricentric, Images::Color& color)
{
  DarbouxNormalShader::fragment(baricentric, color);

  auto vertex = varying_dVertex * baricentric;

  if(0 <= vertex[0] && vertex[0] < uniform_depthBuffer->getWidth()-1 && 0 <= vertex[1] && vertex[1] < uniform_depthBuffer->getHeight()-1)
  {
    if(uniform_depthBuffer->get(vertex[0], vertex[1]) > vertex[2]+43.34)
    {
      // multiply the non-ambient part of the color.
      auto ambient = uniform_ambient_coeff * varying_ambient_value;
      color = ((color - ambient) * 0.5) + ambient;
    }
  }

  return false;
}

//--------------------------------------------------------------------
Vector3f FinalShader::vertex(int iface, int nthvert)
{
  const auto vertexId = uniform_mesh->getFaceVertexIds(iface)[nthvert];
  const auto normalId = uniform_mesh->getFaceNormals(iface)[nthvert];
  const auto vertex   = uniform_mesh->getVertex(vertexId).augment();

  varying_uv_index[nthvert] = uniform_mesh->getFaceUVIds(iface)[nthvert];
  varying_vertex[nthvert]   = (uniform_transform * vertex).project();
  varying_normals.setColumn(nthvert, (uniform_transform_TI * uniform_mesh->getNormal(normalId).augment(0)).project(false).normalize());
  varying_dVertex.setColumn(nthvert, (uniform_transform_S * vertex).project());

  return varying_vertex[nthvert];
}

//--------------------------------------------------------------------
bool FinalShader::fragment(Vector3f baricentric, Images::Color& color)
{
  const auto nb = (varying_normals * baricentric).normalize();

  auto uv0 = uniform_mesh->getuv(varying_uv_index[0]);
  auto uv1 = uniform_mesh->getuv(varying_uv_index[1]);
  auto uv2 = uniform_mesh->getuv(varying_uv_index[2]);

  auto uv = (uv0 * baricentric[0]) + (uv1 * baricentric[1]) + (uv2 * baricentric[2]);

  if(uv[0] > 1 || uv[0] < 0 || uv[1] > 1 || uv[1] < 0) return true;

  const auto l = (uniform_transform * Light.augment(0)).project(false).normalize();
  auto n = nb;
  color = uniform_mesh->getDiffuse(uv);
  float diffuse = 1.0;
  float specular = 0.0;

  if(uniform_mesh->hasTangent())
  {
    Matrix3f A;
    A[0] = varying_vertex[1] - varying_vertex[0];
    A[1] = varying_vertex[2] - varying_vertex[0];
    A[2] = nb;

    auto AI = A.inverse();

    auto i = AI * Vector3f{uv1[0] - uv0[0], uv2[0] - uv0[0], 0.f};
    auto j = AI * Vector3f{uv1[1] - uv0[1], uv2[1] - uv0[1], 0.f};

    Matrix3f B;
    B.setColumn(0, i.normalize());
    B.setColumn(1, j.normalize());
    B.setColumn(2, nb);

    n = (uniform_transform_TI * (B * uniform_mesh->getTangent(uv)).augment(0)).project(false).normalize();
  }

  diffuse = minmax01(n*l);

  if(color.bytespp == 4 && color.a == 0) return true;

  if(diffuse > 0 && uniform_mesh->hasSpecular())
  {
    const auto exp       = uniform_mesh->getSpecular(uv) + 5; // why this +5 fixes specular in diablo?? computations seems right.
    const auto reflected = (n*(n*l*2.f) - l).normalize();
    const auto base      = std::max(reflected[2], 0.f);
    specular = minmax01(std::pow(base, exp));
  }

  auto vertex = (varying_vertex.transpose() * baricentric);
  auto x = static_cast<unsigned short>(vertex[0]);
  auto y = static_cast<unsigned short>(vertex[1]);
  if(0 <= x && x < uniform_ambient_image->getWidth() && 0 <= y && y < uniform_ambient_image->getHeight())
  {
    varying_ambient_value = uniform_ambient_image->get(x,y).raw[0];
  }
  else
  {
    varying_ambient_value = 15;
  }

  vertex = varying_dVertex * baricentric;
  x = static_cast<unsigned short>(vertex[0]);
  y = static_cast<unsigned short>(vertex[1]);

  float shadow_coeff = 1.0;
  if(0 <= x && x < uniform_depthBuffer->getWidth() && 0 <= y && y < uniform_depthBuffer->getHeight() && uniform_depthBuffer->get(x,y) > vertex[2]+43.34)
  {
    shadow_coeff = uniform_shadow_coeff;
  }

  auto light_coeff = uniform_diffuse_coeff*diffuse + uniform_specular_coeff*specular;
  auto ambient = uniform_ambient_coeff*varying_ambient_value;
  color = (color * light_coeff * shadow_coeff) + ambient;

  if(uniform_mesh->hasGlow())
  {
    color += uniform_mesh->getGlow(uv) * uniform_glow_coeff;
  }

  // ramp up the color a bit.
  color = color * 1.7;

  return false;
}
