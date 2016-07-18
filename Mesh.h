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

// C++
#include <vector>
#include <string>
#include <memory>

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

    /** \brief Returns the face with the given index in the faces list.
     * \param[in] idx index.
     *
     */
    std::vector<unsigned long> getFace(unsigned long idx)
    { return m_faces[idx]; }

  private:
    using vertex = Vector3f;
    using face   = std::vector<unsigned long>;

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
     * \param[in] v1 index of the first triangle vertex.
     * \param[in] v2 index of the second triangle vertex.
     * \param[in] v3 index of the third triangle vertex.
     *
     */
    void addFace(const unsigned long v1, const unsigned long v2, const unsigned long v3);


    /** \brief Adds a face to the mesh.
     * \param[in] f face.
     *
     */
    void addFace(const face &f);

    /** \brief Mesh class constructor.
     *
     */
    explicit Mesh();

    std::vector<vertex> m_vertices; /** mesh vertex vector. */
    std::vector<face>   m_faces;    /** mesh faces vector.  */
};

#endif // MESH_H_
