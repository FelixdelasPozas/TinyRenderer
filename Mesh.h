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
#include <unordered_map>
#include <map>

class Mesh;

/** \class Material
 * \brief Holds material textures and properties.
 *
 */
class Material
{
  public:
    /** texture types. */
    enum class TYPE: char { DIFFUSE = 0, NORMAL, NORMALTS, SPECULAR, GLOW, SSS };

    /** \brief Adds a texture.
     * \param[in] filename file path or name to serve as identifier.
     * \param[in] texture texture image.
     *
     */
    void addTexture(const std::string &filename, const std::shared_ptr<Images::Image> texture);

    /** \brief Associates a material and texture type to a image texture.
     * \param[in] materialId material identifier.
     * \param[in] type texture type.
     * \param[in] textureId texture filename.
     */
    void addMaterialTexture(const std::string &materialId, const TYPE type, const std::string &textureId);

    /** \brief Adds a material property.
     * \param[in] materialId material identifier.
     * \param[in] key property key.
     * \param[in] value property value.
     *
     */
    void addProperty(const std::string &materialId, const std::string &key, const Vector3f &value);

    /** \brief Returns a material texture.
     * \param[in] materialId material identifier.
     * \param[in] type texture type.
     *
     */
    std::shared_ptr<Images::Image> getTexture(const std::string &materialId, const TYPE type) const;

    /** \brief Returns a material property value.
     * \param[in] materialId material identifier.
     * \param[in] key property key.
     *
     */
    const Vector3f getProperty(const std::string &materialId, const std::string &key) const;

    /** \brief Returns true if the given material has a texture of the given type.
     * \param[in] materialId material identifier.
     * \param[in] type texture type.
     *
     */
    bool hasTexture(const std::string &materialId, const TYPE type) const;

  private:
    std::map<std::string, std::shared_ptr<Images::Image>>                      m_textures;   /** texture-filename <-> image map   */
    std::unordered_map<std::string, std::unordered_map<std::string, Vector3f>> m_properties; /** materialId <-> key-value map.    */
    std::unordered_map<std::string, std::unordered_map<int, std::string>>      m_materials;  /** materialId <-> type-texture map. */
};

/** \class Wavefront
 * \brief Implements a simple wavefront obj container.
 *
 */
class Wavefront
{
  public:
    using Meshes = std::vector<std::shared_ptr<Mesh>>;

    /** \brief WaveFront class destructor.
     *
     */
    ~Wavefront()
    {};

    /** \brief Static method to read a wavefront obj file.
     * \param[in] filename file name.
     *
     */
    static std::shared_ptr<Wavefront> read(const std::string &filename);

    /** \brief Returns the meshes vector.
     *
     */
    Meshes meshes() const
    { return m_meshes; }

    /** \brief Writes the object to a file on disk.
     * \param[in] filename file name.
     *
     */
    bool write(const std::string &filename);

    /** \brief Adds a mesh.
     * \param[in] mesh mesh object.
     *
     */
    void addMesh(std::shared_ptr<Mesh> mesh)
    { m_meshes.push_back(mesh); }

    /** \brief Returns the id of the object.
     *
     */
    const std::string &id() const
    { return m_id; };

    /** \brief Sets the material information for the meshes.
     *
     */
    void setMaterial(std::shared_ptr<Material> material);

  private:
    /** \brief WaveFront class constructor.
     *
     */
    explicit Wavefront(const std::string &id)
    : m_id{id}
    {};

  private:
    std::shared_ptr<Material> m_material; /** meshe's material.                */
    Meshes                    m_meshes;   /** mesh vector.                     */
    const std::string        &m_id;       /** object id, usually the filename. */
};

/** \class Mesh
 * \brief Implements a mesh object.
 *
 */
class Mesh
{
  public:
    /** \brief Mesh class constructor.
     * \param[in] id mesh id.
     *
     */
    explicit Mesh(const std::string &id);

    /** \brief Mesh class virtual destructor.
     *
     */
    virtual ~Mesh()
    {}

    /** \brief Returns the id of the mesh.
     *
     */
    const std::string &id() const
    { return m_id; }

    /** \brief Returns the number of vertex of the mesh.
     *
     */
    unsigned long vertex_num() const
    { return m_vertices.size(); }

    /** \brief Returns the number of normals of the mesh.
     *
     */
    unsigned long normals_num() const
    { return m_normals.size(); }

    /** \brief Returns the number of uv coordinates of the mesh.
     *
     */
    unsigned long uv_num() const
    { return m_uv.size(); }

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
    std::vector<unsigned long> getFaceVertexIds(unsigned long idx)
    { return m_faces[idx]._vertex; }

    /** \brief Returns the vertex id of the given face and vertex position.
     * \param[in] idx face index.
     * \param[in] n vertex position.
     *
     */
    unsigned long getFaceVertexId(const unsigned long idx, const unsigned long n)
    { return getFaceVertexIds(idx)[n]; }

    /** \brief Returns the uv indexes list of the given face index in the faces list.
     * \param[in] idx face index.
     *
     */
    std::vector<unsigned long> getFaceUVIds(unsigned long idx)
    { return m_faces[idx]._uv; }

    /** \brief Returns the normal vector indexes of the given face index in the faces list.
     * \param[in] idx face index.
     *
     */
    std::vector<unsigned long> getFaceNormals(unsigned long idx)
    { return m_faces[idx]._normal; }

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

    /** \brief Returns the glow color for the given texture coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Images::Color getGlow(const float u, const float v);

    /** \brief Returns the glow color for the given texture coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Images::Color getGlow(Vector2f uv)
    { return getGlow(uv[0], uv[1]); }

    /** \brief Returns the glow color for the given texture coordinates.
     * \param[in] u u coordinate.
     * \param[in] v v coordinate.
     *
     */
    Images::Color getSSS(const float u, const float v);

    /** \brief Returns the glow color for the given texture coordinates.
     * \param[in] uv Vector2f coordinates
     *
     */
    Images::Color getSSS(Vector2f uv)
    { return getGlow(uv[0], uv[1]); }

    /** \brief Returns true if the model has specular texture and false otherwise.
     *
     */
    bool hasSpecular() const
    { return m_material->hasTexture(m_mtl, Material::TYPE::SPECULAR); }

    /** \brief Returns true if the model has Darboux normal texture and false otherwise.
     *
     */
    bool hasTangent() const
    { return m_material->hasTexture(m_mtl, Material::TYPE::NORMALTS); }

    /** \brief Returns true if the model has normals texture and false otherwise.
     *
     */
    bool hasNormalMap() const
    { return m_material->hasTexture(m_mtl, Material::TYPE::NORMAL); }

    /** \brief Returns true if the model has glow texture and false otherwise.
     *
     */
    bool hasGlow() const
    { return m_material->hasTexture(m_mtl, Material::TYPE::GLOW); }

    /** \brief Returns true if the model has subsurface scattering texture.
     *
     */
    bool hasSSS() const
    { return m_material->hasTexture(m_mtl, Material::TYPE::SSS); }

    /** \brief Sets the id of the associated material.
     * \param[in] mtl material id.
     *
     */
    void setMaterialId(const std::string &mtl)
    { m_mtl = mtl; }

    /** \brief Sets the material object.
     * \param[in] material
     */
    void setMaterial(std::shared_ptr<Material> material)
    { m_material = material; }

    /** brief Returns the material id.
     *
     */
    const std::string &materialId() const
    { return m_mtl; }

    /** \brief Returns the material of the mesh.
     *
     */
    std::shared_ptr<Material> material() const
    { return m_material; }

  private:
    struct Face
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
    void addVertex(const Vector3f &v);

    /** \brief Adds a face to the mesh.
     * \param[in] f face.
     *
     */
    void addFace(const Face &f);

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
    void adduv(const Vector2f &t);

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
    void addNormal(const Vector3f &n);

    const std::string         m_id;       /** mesh id                          */
    std::vector<Vector3f>     m_vertices; /** mesh vertex vector.              */
    std::vector<Face>         m_faces;    /** mesh faces vector.               */
    std::vector<Vector2f>     m_uv;       /** texture coordinates of vertices. */
    std::vector<Vector3f>     m_normals;  /** face normals.                    */
    std::string               m_mtl;      /** material id.                     */
    std::shared_ptr<Material> m_material; /** mesh material object.            */

    friend std::shared_ptr<Wavefront> Wavefront::read(const std::string &);
    friend bool Wavefront::write(const std::string &);
};

#endif // MESH_H_
