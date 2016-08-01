/*
 File: Shaders.h
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

#ifndef SHADERS_H_
#define SHADERS_H_

// Project
#include <GL_Impl.h>
#include <Algebra.h>

/** \struct MultiShader
 * \brief Use of multiple shaders for image generation.
 *
 */
struct MultiShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f bar, Images::Color &color);

    Vector3f              varying_intensity; // written by vertex shader, read by fragment shader
    const Matrix4f        uniform_transform = ViewPort*Projection*ModelView;

    std::vector<Shader *> uniform_shaders;
    int                   uniform_interval = 0;
    int                   varying_selector = 0;
    Vector3f              varying_svertex[3];
    int                   varying_vertexi = 0;

    ~MultiShader()
    {
      for(auto shader: uniform_shaders) delete shader;
    };

    void addShader(Shader *shader)
    {
      shader->uniform_mesh = uniform_mesh;

      uniform_shaders.push_back(shader);
    }
};

/** \struct GouraudShader
 * \brief Implements Gouraud shading without textures
 *
 */
struct GouraudShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f bar, Images::Color &color);

    Vector3f              varying_intensity; // written by vertex shader, read by fragment shader
    const Matrix4f        uniform_transform = ViewPort*Projection*ModelView;
    const Matrix4f        uniform_transform_TI = (Projection*ModelView).transpose().inverse();
};

/** \struct GouraudShader
 * \brief Implements Gouraud shading without textures
 *
 */
struct CellShader
: public GouraudShader
{
    virtual bool fragment(Vector3f bar, Images::Color &color) override;

    int           varying_shades    = 5;                          // default number of varying_shades
    Images::Color varying_baseColor = Images::Color(255,255,255); // default color
};

/** \struct GouraudShader
 * \brief Implements Gouraud shading.
 *
 */
struct TexturedGouraudShader
: public GouraudShader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f bar, Images::Color &color);

    Vector3i varying_uv_index; // uv_indexes
};

struct NormalMapping
: public TexturedGouraudShader
{
    virtual bool fragment(Vector3f bar, Images::Color &color);
};

struct TexturedNormalMapping
: public TexturedGouraudShader
{
    virtual bool fragment(Vector3f bar, Images::Color &color);
};

struct TexturedSpecularShader
: public TexturedNormalMapping
{
    virtual bool fragment(Vector3f bar, Images::Color &color);

    int   uniform_ambient_value  = 120;
    int   uniform_ambient_coeff  = 0.2;
    float uniform_specular_coeff = 0.3;
    float uniform_diffuse_coeff  = 0.5;
};

struct PhongShader
: public GouraudShader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f bar, Images::Color &color);

    Vector3i varying_uv_index; // uv_indexes
    Vector3i varying_normals;  // normals.
};

#endif // SHADERS_H_
