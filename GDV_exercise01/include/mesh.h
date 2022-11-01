#ifndef MESH_H
#define MESH_H

#include <cstdint>
#include <string>
#include <vector>

#include "aabb.h"
#include "point2d.h"
#include "point3d.h"

using Vertex = Point3D;
using TextureCoordinate = Point2D;

struct TriangleIndices {
    uint32_t v1, v2, v3;
};

/**
 * @brief 3D Triangle Mesh
 */
class Mesh {
public:
    Mesh() = default;

    /**
     * @brief loadOBJ loads an OBJ file containing triangles or quads
     * vertex normals and texture coordinates are ignored
     * all faces are merged into one object
     * @param filename
     */
    void loadOBJ(const std::string& filename);

    /// remove all vertices and faces
    void clear() { *this = {}; }

    /// get vertices for reading
    const std::vector<Vertex>& getVertices() const { return vertices; }
    /// get vertices for writing
    std::vector<Vertex>& getVertices() { return vertices; }
    /// get faces for reading
    const std::vector<TriangleIndices>& getFaces() const { return faces; }
    /// get faces for writing
    std::vector<TriangleIndices>& getFaces() { return faces; }
    /// get pre-computed bounding box
    const AABB& getBounds() const { return aabb; }
    /// re-compute bounding box
    void updateBounds();

    /// get normals for reading
    const std::vector<Vertex>& getNormals() const { return normals; }
    /// get normals for writing
    std::vector<Vertex>& getNormals() { return normals; }
    /// get texture coordinates for reading
    const std::vector<TextureCoordinate>& getTextureCoordinates() const { return texCoords; }
    /// get texture coordinates for writing
    std::vector<TextureCoordinate>& getTextureCoordinates() { return texCoords; }

    const std::vector<float>& getFaceAreas() const { return faceAreas; }

    const std::vector<std::pair<size_t, size_t>>& getSmoothGroups() const { return smoothGroups; }

private:
    /// the vertices of the mesh
    std::vector<Vertex> vertices;
    /// the triangle faces of the mesh
    std::vector<TriangleIndices> faces;
    /// axis alignedbounding box containing all vertices
    AABB aabb;

    /// the normal vectors per vertex
    std::vector<Vertex> normals;
    /// the texture coordinates per vertex
    std::vector<TextureCoordinate> texCoords;
    /// the area of each triangle
    std::vector<float> faceAreas;
    /// smooth groups
    std::vector<std::pair<size_t, size_t>> smoothGroups;
};


struct Triangle {
    const Vertex& v1, v2, v3;
    Triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3)
        : v1{v1}, v2{v2}, v3{v3} {}

    Triangle(const TriangleIndices& indices, const Mesh& mesh)
        : Triangle{mesh.getVertices().at(indices.v1),
                   mesh.getVertices().at(indices.v2),
                   mesh.getVertices().at(indices.v3)} {}
};

#endif // MESH_H
