//
// Created by Kiril on 05.07.2022.
//

#ifndef MESH_SPARSEPOINT_H
#define MESH_SPARSEPOINT_H

#include <vector>
#include <glm/glm.hpp>

namespace PolygonMesh {

    class SparsePoint
    {
    public:
        [[nodiscard]]
        glm::vec3 get() const;
        void set(const glm::vec3 &vec);

        void add(glm::vec3 &ptr);
        void remove(glm::vec3 &ptr);
    private:
        std::vector<glm::vec3*> m_points;
    };

} // PolygonMesh

#endif //MESH_SPARSEPOINT_H
