/*
 File: Draw.cpp
 Created on: 18 jul. 2016
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
#include "GL_Impl.h"
#include "Utils.h"

// C++
#include <cmath>
#include <limits>
#include <intrin.h>

using namespace Images;
using namespace Utils;

//--------------------------------------------------------------------
void GL_Impl::projection(const float coeff)
{
  Projection.identity();
  Projection[3][2] = coeff;
}

//--------------------------------------------------------------------
void GL_Impl::lookAt(const Vector3f &eye, const Vector3f &center, const Vector3f &up)
{
  auto z = (eye - center).normalize();
  auto x = (up ^ z).normalize();
  auto y = (z ^ x).normalize();

  Matrix4f Minv, Tr;
  Minv.identity();
  Tr.identity();

  for (int i = 0; i < 3; i++)
  {
    Minv[0][i] = x[i];
    Minv[1][i] = y[i];
    Minv[2][i] = z[i];
      Tr[i][3] = -center[i];
  }

  ModelView = Minv * Tr;
}
//--------------------------------------------------------------------
void GL_Impl::viewport(int x, int y, int width, int height, int depth)
{
  ViewPort.identity();
  ViewPort[0][3] = x + width / 2.f;
  ViewPort[1][3] = y + height / 2.f;
  ViewPort[2][3] = depth / 2.f;

  ViewPort[0][0] = width / 2.f;
  ViewPort[1][1] = height / 2.f;
  ViewPort[2][2] = depth / 2.f;
}

//--------------------------------------------------------------------
void GL_Impl::line(int x0, int y0, int x1, int y1, Image &image, const Color &color)
{
  auto steep = false;
  if (std::abs(x0 - x1) < std::abs(y0 - y1))
  {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }

  if (x0 > x1)
  {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }

  for (int x = x0; x <= x1; x++)
  {
    auto t = (x - x0) / static_cast<float>(x1 - x0);
    int y = y0 * (1. - t) + y1 * t;

    if (steep)
    {
      image.set(y, x, color);
    }
    else
    {
      image.set(x, y, color);
    }
  }
}

//--------------------------------------------------------------------
Vector3f barycentric(Vector3f *pts, const Vector3f &P)
{
  Vector3f s[2];

  for (auto i: {0,1})
  {
    s[i][0] = pts[2][i] - pts[0][i];
    s[i][1] = pts[1][i] - pts[0][i];
    s[i][2] = pts[0][i] - P[i];
  }

  auto u = s[0] ^ s[1];

  // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
  if (std::abs(u[2]) <= 0)
  {
    return Vector3f{-1, 1, 1}; // in this case generate negative coordinates, it will be thrown away by the rasterizator
  }

  return Vector3f{1.f - (u[0] + u[1]) / u[2], u[1] / u[2], u[0] / u[2]};
}

//--------------------------------------------------------------------
void GL_Impl::triangle(Vector3f *sPts, Shader &shader, zBuffer &buffer, Images::Image &image)
{
  const auto width  = image.getWidth();
  const auto height = image.getHeight();

  Vector2i min{ std::numeric_limits<int>::max(),  std::numeric_limits<int>::max()};
  Vector2i max{-std::numeric_limits<int>::max(), -std::numeric_limits<int>::max()};
  Vector2i clamp{width-1, height-1};

  for (int i: {0,1,2})
  {
    for (int j: {0,1})
    {
      min[j] = std::max(0, std::min(min[j], static_cast<int>(sPts[i][j])));
      max[j] = std::min(clamp[j], std::max(max[j], static_cast<int>(sPts[i][j])));
    }
  }

  Vector3f P{0,0,0};

  for (P[0] = min[0]; P[0] <= max[0]; ++P[0])
  {
    for (P[1] = min[1]; P[1] <= max[1]; ++P[1])
    {
      auto bc_screen = barycentric(sPts, P);
      if(bc_screen[0] < 0 || bc_screen[1] < 0 || bc_screen[2] < 0) continue;

      P[2] = sPts[0][2]*bc_screen[0] + sPts[1][2]*bc_screen[1] + sPts[2][2]*bc_screen[2];
      if (!buffer.checkAndSet(P[0], P[1], P[2])) continue;

      Color color;
      bool discard = shader.fragment(bc_screen, color);
      if (!discard)
      {
        image.set(P[0], P[1], color);
      }
    }
  }
}

//--------------------------------------------------------------------
float GL_Impl::max_elevation_angle(zBuffer &buffer, Vector2f point, Vector2f direction)
{
  float max_angle = 0;
  float t = 0;

  const auto ptr       = buffer.getBuffer();
  const __m256 max     = _mm256_set1_ps(1000);
  const __m256 distmax = _mm256_set1_ps(1);
  const __m256 width   = _mm256_set1_ps(buffer.getWidth());
  const __m256 height  = _mm256_set1_ps(buffer.getHeight());

  bool finished = false;

  while(!finished)
  {
    __m256 vt = _mm256_set_ps(t, t+1, t+2, t+3, t+4, t+5, t+6, t+7);
    vt = _mm256_min_ps(max, vt);

    __m256 valid = _mm256_set1_ps(0xffff);
    __m256 px = _mm256_set1_ps(point[0]);
    __m256 py = _mm256_set1_ps(point[1]);
    __m256 cx = _mm256_set1_ps(direction[0]);
    __m256 cy = _mm256_set1_ps(direction[1]);

    cx = _mm256_mul_ps(cx, vt);
    cy = _mm256_mul_ps(cy, vt);
    cx = _mm256_add_ps(px, cx);
    cy = _mm256_add_ps(py, cy);

    auto compx = _mm256_cmp_ps(cx, width, _CMP_GE_OS);
    auto compy = _mm256_cmp_ps(cy, height, _CMP_GE_OS);

    for(int i = 0; i < 8; ++i)
    {
      if(compx[i] != 0) { valid[i] = 0x0; finished = true; }
      if(compy[i] != 0) { valid[i] = 0x0; finished = true; }
    }

    compx = _mm256_cmp_ps(cx, _mm256_setzero_ps(), _CMP_LT_OS);
    compy = _mm256_cmp_ps(cy, _mm256_setzero_ps(), _CMP_LT_OS);

    for(int i = 0; i < 8; ++i)
    {
      if(compx[i] != 0) { valid[i] = 0x0; finished = true; }
      if(compy[i] != 0) { valid[i] = 0x0; finished = true; }
    }

    auto distx = _mm256_sub_ps(px, cx);
    auto disty = _mm256_sub_ps(py, cy);

    distx = _mm256_mul_ps(distx, distx);
    disty = _mm256_mul_ps(disty, disty);
    disty = _mm256_add_ps(distx, disty);
    distx = _mm256_sqrt_ps(disty);

    compx = _mm256_cmp_ps(distx, distmax, _CMP_GE_OS);

    for(int i = 0; i < 8; ++i)
    {
      if(!valid[i]) continue;
      if(compx[i] == 0) { valid[i] = 0x0; }
    }

    cx = _mm256_floor_ps(cx);
    cy = _mm256_floor_ps(cy);

    auto ip = _mm256_mul_ps(py, width);
    ip = _mm256_add_ps(px, ip);

    auto ic = _mm256_mul_ps(cy, width);
    ic = _mm256_add_ps(cx, ic);

    for(int i = 0; i < 8; ++i)
    {
      if(valid[i] == 0) ic[i] = 0;
    }

    px = _mm256_set1_ps(ptr[static_cast<long>(ip[0])]);

    cx = _mm256_set_ps(ptr[static_cast<long>(ic[7])],ptr[static_cast<long>(ic[6])],ptr[static_cast<long>(ic[5])],ptr[static_cast<long>(ic[4])],
                       ptr[static_cast<long>(ic[3])],ptr[static_cast<long>(ic[2])],ptr[static_cast<long>(ic[1])],ptr[static_cast<long>(ic[0])]);

    cx = _mm256_sub_ps(cx, px);

    cx = _mm256_div_ps(cx, distx);

    for(int i = 0; i < 8; ++i)
    {

      if(valid[i] == 0x0) continue;

      max_angle = std::max(max_angle, ::atanf(cx[i]));
    }

    t += 8;
    if(t >= 1000) break;
  }

  return max_angle;
}
