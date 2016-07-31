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
    std::shared_ptr<Mesh> m_mesh;

    Matrix4f transform = ViewPort*Projection*ModelView;
};

/** \struct GouraudShader
 * \brief Implements Gouraud shading without textures
 *
 */
struct CellShader
: public GouraudShader
{
    virtual bool fragment(Vector3f bar, Images::Color &color) override;

    int           shades    = 5;                          // default number of shades
    Images::Color baseColor = Images::Color(255,255,255); // default color
};

/** \struct GouraudShader
 * \brief Implements Gouraud shading.
 *
 */
struct TexturedGouraudShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f bar, Images::Color &color);

    Vector3f              varying_intensity; // written by vertex shader, read by fragment shader
    Vector3i              varying_uv_index;
    std::shared_ptr<Mesh> m_mesh;

    Matrix4f transform = ViewPort*Projection*ModelView;
};

#endif // SHADERS_H_
