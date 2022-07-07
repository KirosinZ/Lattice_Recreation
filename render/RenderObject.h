#ifndef RENDER_RENDER_OBJECT_H
#define RENDER_RENDER_OBJECT_H

#include "..\mesh\Mesh.h"
#include "..\program\Program.h"
#include <glm/mat4x4.hpp>

namespace Rendering {

    struct RenderObject {
        struct Material
        {
            glm::vec3 diffuse = glm::vec3(0.65f);
            glm::vec3 specular = glm::vec3(0.35f);

            float shininess = 64.0f;
        };

        PolygonMesh::Mesh& mesh;
        Shading::Program program;
        Material material;
        glm::mat4 modelMatrix = glm::mat4(1.0f);

        RenderObject(PolygonMesh::Mesh& mesh, Material material, const Shading::Program& program);

        void draw() const;
    };

}


#endif //RENDER_RENDER_OBJECT_H
