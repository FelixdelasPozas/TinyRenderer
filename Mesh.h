/*
 File: Mesh.h
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

#ifndef MESH_H_
#define MESH_H_

// Project
#include "Algebra.h"
#include "Images.h"

// C++
#include <vector>
#include <string>
#include <memory>
#include <list>

/** \class Mesh
 * \brief Implements a mesh object.
 *
 */
class Mesh
{
  public:
    /** \brief Mesh class virtual destructor.
     *
     */
    virtual ~Mesh()
    {}

    /** \brief Reads a wavefront file from disk and returns a mesh or null if error.
     * \param[in] filename wavefront file name.
     *
     */
    static std::shared_ptr<Mesh> read_Wavefront(const std::string &filename);

    /** \brief Writes the mesh to a file to disk.
     * \param[in] filename wavefront file name.
     *
     */
    bool write_Wavefront(const std::string &filename);

    /** \brief Returns the number of vertex of the mesh.
     *
     */
    unsigned long vertex_num() const
    { return m_vertices.size(); }

    /** \brief Returns the number of faces of the mesh.
     *
     */
    unsigned long faces_num() const
    { return m_faces.size(); }

    /** \brief Returns the vertex with the given index in the vertex list.
     * \param[in] idx index.
     *
     */
    Vector3f getVertex(unsigned long idx)
    { return m_vertices[idx]; }

    /** \brief Returns the vertices list of the given face index in the faces list.
     * \param[in] idx face index.
     *
     */
    std::vector<unsigned long> getFaceVertices(unsigned long idx)
    { return m_faces[idx]._vertex; }

    /** \brief Returns the uv indexes list of the given face index in the faces list.
     * \param[in] idx face index.
     *
     */
    std::vector<unsigned long> getuvIds(unsigned long idx)
    { return m_faces[idx]._uv; }

    /** \brief Returns the texture coordinates for the given index.
     * \param[in] idx vertex index.
     *
     */
    Vector2f getuv(unsigned long idx)
    { return m_uv[idx]; }

    /** \brief Returns the normal vector of the given vertex index.
     * \param[in] idx vertex index.
     *
     */
    Vector3f getNormal(unsigned long idx)
    { return m_normals[idx]; }

    /** \brief Sets the diffuse texture of the model.
     * \param[in] texture texture image object.
     *
     */
    void setDiffuseTexture(std::shared_ptr<Images::Image> texture);

    /** \brief Returns the diffuse texture.
     *
     * NOTE: should be const returned but need not to to dump it.
     *
     */
    std::shared_ptr<Images::Image> diffuseTexture() const
    { return m_diffuse; }

    /** \brief Returns the diffuse texture color for the given coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Images::Color getDiffuse(const float u, const float v);

    /** \brief Returns the diffuse texture color for the given coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Images::Color getDiffuse(Vector2f uv)
    { return getDiffuse(uv[0], uv[1]); }

    /** \brief Sets the normal map texture of the model.
     * \param[in] texture texture image object.
     *
     */
    void setNormalMap(std::shared_ptr<Images::Image> texture)
    { m_normalMap = texture; }

    /** \brief Returns the normal map texture.
     *
     */
    std::shared_ptr<Images::Image> normalMapTexture() const
    { return m_normalMap; }

    /** \brief Returns the normal vector for the given coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Vector3f getNormalMap(const float u, const float v);

    /** \brief Returns the normal vector for the given coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Vector3f getNormalMap(Vector2f uv)
    { return getNormalMap(uv[0], uv[1]); }

    /** \brief Sets the specular texture of the model.
     * \param[in] texture texture image object.
     *
     */
    void setSpecular(std::shared_ptr<Images::Image> texture)
    { m_specular = texture; }

    /** \brief Returns the specular texture.
     *
     */
    std::shared_ptr<Images::Image> SpecularTexture() const
    { return m_specular; }

    /** \brief Returns the specular value for the given coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    float getSpecular(const float u, const float v);

    /** \brief Returns the specular value for the given coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    float getSpecular(Vector2f uv)
    { return getSpecular(uv[0], uv[1]); }

    /** \brief Sets the Darboux tangent space texture of the model.
     * \param[in] texture texture image object.
     *
     */
    void setTangent(std::shared_ptr<Images::Image> texture)
    { m_tangent = texture; }

    /** \brief Returns the Darboux tangent space texture of the model.
     *
     */
    std::shared_ptr<Images::Image> TangentTexture() const
    { return m_tangent; }

    /** \brief Returns the tangent space vector for the given coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Vector3f getTangent(const float u, const float v);

    /** \brief Returns the tangent space vector for the given coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Vector3f getTangent(Vector2f uv)
    { return getTangent(uv[0], uv[1]); }

    /** \brief Sets the glow texture of the model.
     * \param[in] texture texture image object.
     *
     */
    void addAdditive(std::shared_ptr<Images::Image> texture)
    { m_additives.push_back(texture); }

    /** \brief Returns the glow texture of the model.
     *
     */
    unsigned int additives() const
    { return m_additives.size(); }

    /** \brief Returns the glow value for the given coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Images::Color getAdditives(const float u, const float v);

    /** \brief Returns the glow value for the given coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Images::Color getAdditives(Vector2f uv)
    { return getAdditives(uv[0], uv[1]); }


  private:
    using vertex = Vector3f;
    using normal = Vector3f;
    using uv     = Vector2f;
    struct face
    {
      std::vector<unsigned long> _vertex;
      std::vector<unsigned long> _uv;
      std::vector<unsigned long> _normal;
    };

    /** \brief Adds a vertex to the mesh.
     * \param[in] x vertex x coordinate.
     * \param[in] y vertex y coordinate.
     * \param[in] z vertex z coordinate.
     *
     */
    void addVertex(const double x, const double y, const double z);

    /** \brief Adds a vertex to the mesh.
     * \param[in] v Vector3f.
     *
     */
    void addVertex(const vertex &v);

    /** \brief Adds a face to the mesh.
     * \param[in] f face.
     *
     */
    void addFace(const face &f);

    /** \brief Adds a texture coordinate to the mesh.
     * \param[in] u texture u coordinate.
     * \param[in] v texture v coordinate.
     *
     */
    void adduv(const double u, const double v);

    /** \brief Adds a texture coordinate to the mesh.
     * \param[in] t texture coordinates.
     *
     */
    void adduv(const uv &t);

    /** \brief Adds a normal to the mesh.
     * \param[in] x normal x coordinate.
     * \param[in] y normal y coordinate.
     * \param[in] z normal z coordinate.
     *
     */
    void addNormal(const double x, const double y, const double z);

    /** \brief Adds a normal to the mesh.
     * \param[in] n normal vector.
     *
     */
    void addNormal(const normal &n);

    /** \brief Mesh class constructor.
     *
     */
    explicit Mesh();

    std::vector<vertex> m_vertices;  /** mesh vertex vector.                 */
    std::vector<face>   m_faces;     /** mesh faces vector.                  */
    std::vector<uv>     m_uv;        /** texture coordinates of vertices.    */
    std::vector<normal> m_normals;   /** face normals.                       */

    std::shared_ptr<Images::Image> m_diffuse;   /** mesh diffuse texture.    */
    std::shared_ptr<Images::Image> m_normalMap; /** normalMap texture.       */
    std::shared_ptr<Images::Image> m_specular;  /** specular texture.        */
    std::shared_ptr<Images::Image> m_tangent;   /** tangent-space normal map.*/

    std::list<std::shared_ptr<Images::Image>> m_additives; /** additive to diffuse textures.  */
};

#endif // MESH_H_
