#ifndef MESH_MESH_H
#define MESH_MESH_H

#include <string>
#include <gl/glew.h>


#include <glm/vec3.hpp>

#include "../multipointer/MultiPointer.h"
#include "SparsePoint.h"

namespace PolygonMesh {

    class Mesh {
        using uint = unsigned int;
        using uchar = unsigned char;

        using CoordinateType = glm::vec3;
        using TextureCoordinateType = glm::vec3;
        using NormalType = glm::vec3;

        using IndexType = uint;

    public:
        Mesh() = default;

        explicit Mesh(const std::string &filename);
        ~Mesh();

        Mesh(const Mesh &copy);
        Mesh(Mesh &&move) noexcept;

        Mesh& operator=(const Mesh &copy);
        Mesh& operator=(Mesh &&move) noexcept;

        [[maybe_unused]]
        void writeIntoObj(std::string filename) const;

        void allocateBufferData();
        void load();

        void draw() const;

        [[nodiscard]]
        std::vector<SparsePoint> coords();
        [[nodiscard]]
        std::vector<CoordinateType> constCoords() const;

        [[nodiscard]]
        bool isAllocated() const;
        [[nodiscard]]
        bool isLoaded() const;

        [[nodiscard]]
        glm::vec3 lowerBound() const;
        [[nodiscard]]
        glm::vec3 upperBound() const;

    private:
        struct Vertex
        {
            CoordinateType coordinates;
            TextureCoordinateType textureCoordinates;
            NormalType normal;
        };

        struct AuxiliaryData
        {
            IndexType pointIndex = 0;
            IndexType textureCoordinatesIndex = 0;
            IndexType normalIndex = 0;
        };

        std::vector<Vertex> m_renderData;
        std::vector<AuxiliaryData> m_auxiliaryData;
        std::vector<IndexType> m_indices;
        std::vector<std::pair<uchar, IndexType>> m_geometry;

        uint m_numVertices = 0;
        uint m_numTextureCoordinates = 0;
        uint m_numNormals = 0;

        GLuint VAO = 0, VBO = 0, EBO = 0;
        bool m_allocated = false;
        bool m_loaded = false;

        glm::vec3 m_lowerBound = glm::vec3(0.0f);
        glm::vec3 m_upperBound = glm::vec3(0.0f);

        void readObj(std::ifstream &file);
        void writeObj(std::ofstream &file) const;
    };

}

#endif //MESH_MESH_H
