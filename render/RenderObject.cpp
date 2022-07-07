#include "RenderObject.h"

Rendering::RenderObject::RenderObject(PolygonMesh::Mesh &mesh, struct Material material, const Shading::Program &program)
:   mesh(mesh),
    material(material),
    program(program)
{
    if (!this->mesh.isAllocated())
        this->mesh.allocateBufferData();
    if (!this->mesh.isLoaded())
        this->mesh.load();
}

void Rendering::RenderObject::draw() const
{
    program.use();
    program.set("material.diffuse", material.diffuse);
    program.set("material.specular", material.specular);
    program.set("material.shininess", material.shininess);
    program.set("model", modelMatrix);

    glm::mat4 normal = modelMatrix;
    normal = glm::inverse(normal);
    normal = glm::transpose(normal);

    program.set("normalmodel", normal);
    mesh.draw();
}