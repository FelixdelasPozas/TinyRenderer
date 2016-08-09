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
#include <Utils.h>

/** \struct MultiShader
 * \brief Use of multiple shaders for image generation.
 *
 */
struct MultiShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

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

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

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
    virtual bool fragment(Vector3f baricentric, Images::Color &color) override;

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

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    Vector3i varying_uv_index; // uv_indexes
};

/** \struct NormalMapping
 * \brief Implements normal mapping with normal textures.
 *
 */
struct NormalMapping
: public TexturedGouraudShader
{
    virtual bool fragment(Vector3f baricentric, Images::Color &color);
};

/** \struct TexturedNormalMapping
 * \brief Implements textured normal mapping with normal textures.
 *
 */
struct TexturedNormalMapping
: public TexturedGouraudShader
{
    virtual bool fragment(Vector3f baricentric, Images::Color &color);
};

/** \struct TexturedSpecularMapping
 * \brief Adds specular mapping computation to previous shader.
 *
 */
struct TexturedSpecularShader
: public TexturedNormalMapping
{
    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    int   uniform_ambient_value  = 120;
    int   uniform_ambient_coeff  = 0.2;
    float uniform_specular_coeff = 0.3;
    float uniform_diffuse_coeff  = 0.5;
};

/** \struct PhongShader
 * \brief Implements phong shading.
 *
 */
struct PhongShader
: public GouraudShader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    Vector3i varying_uv_index; // uv_indexes
    Vector3i varying_normals;  // normals indexes.
};

/** \struct DarbousNormalShader
 * \brief Implements normal mapping using tangent space textures.
 *
 */
struct DarbouxNormalShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    float          uniform_glow_coeff     = 1.0; // glow coefficient.
    float          uniform_specular_coeff = 0.4; // specular coefficient.
    float          uniform_diffuse_coeff  = 0.5; // diffuse coefficient.
    float          uniform_ambient_coeff  = 0.1; // ambient coefficient.

    Vector3i       varying_uv_index;       // uv_indexes
    Matrix3f       varying_normals;        // normals indexes.
    Matrix3f       varying_vertex;         // triangle in Projection*Modelview
    const Matrix4f uniform_transform    = ViewPort*Projection*ModelView;
    const Matrix4f uniform_transform_TI = (Projection*ModelView).transpose().inverse();
    int            varying_ambient_value;
    std::shared_ptr<Images::Image> uniform_ambient_image;
};

/** \struct EmptyShader
 * \brief Computes the transformed coordinates but does nothing else. Used to compute depth buffers.
 *
 */
struct EmptyShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color)
    { return true; }

    Matrix3f                        varying_vertex;
    Matrix4f                        uniform_transform = ViewPort*Projection*ModelView;
    std::shared_ptr<Utils::zBuffer> uniform_depthBuffer;
};

/** \struct HardShadowsShader
 * \brief Adds shadows computation to Darboux shader using a depth buffer computed from the light position.
 *
 */
struct HardShadowsShader
: public DarbouxNormalShader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    Matrix3f                        varying_dVertex;
    Matrix4f                        uniform_transform_S;
    std::shared_ptr<Utils::zBuffer> uniform_depthBuffer;
};

/** \struct FinalShader
 * \brief Computes the final image using all the shaders developed in the course.
 *
 */
struct FinalShader
: public GL_Impl::Shader
{
    virtual Vector3f vertex(int iface, int nthvert);

    virtual bool fragment(Vector3f baricentric, Images::Color &color);

    float          uniform_glow_coeff     = 1.0; // glow coefficient.
    float          uniform_specular_coeff = 0.4; // specular coefficient.
    float          uniform_diffuse_coeff  = 0.5; // diffuse coefficient.
    float          uniform_ambient_coeff  = 0.1; // ambient coefficient.

    Vector3i       varying_uv_index; // uv_indexes
    Matrix3f       varying_normals;  // normals indexes.
    Matrix3f       varying_vertex;   // triangle in Projection*Modelview
    Matrix3f       varying_dVertex;  // triangle in light projection.
    const Matrix4f uniform_transform    = ViewPort*Projection*ModelView;
    const Matrix4f uniform_transform_TI = (Projection*ModelView).transpose().inverse();
    Matrix4f       uniform_transform_S; // matrix of the light depth computation.
    int            varying_ambient_value;
    std::shared_ptr<Images::Image>  uniform_ambient_image;
    std::shared_ptr<Utils::zBuffer> uniform_depthBuffer;
};

#endif // SHADERS_H_
