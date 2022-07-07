#include "Scene.h"

void Rendering::Scene::addObject(const Rendering::RenderObject &object)
{
    objects.push_back(object);
    objectVisibilityMask.push_back(2);
    m_wireframeToggles.push_back(false);
    object.program.use();
    object.program.set("projection", projectionMatrix);
    object.program.set("dirlight.direction", glm::vec3(-1.0f));
    object.program.set("dirlight.ambient", 2.0f * glm::vec3(0.05f));
    object.program.set("dirlight.diffuse", 2.0f * glm::vec3(0.75f));
    object.program.set("dirlight.specular", 2.0f * glm::vec3(0.15f));
}

void Rendering::Scene::toggleObjectVisibility(int index)
{
    if(index >= objectVisibilityMask.size())
        return;

    objectVisibilityMask[index]--;
    if(objectVisibilityMask[index] == -1)
        objectVisibilityMask[index] = 2;
}

void Rendering::Scene::frame() const
{
    int index = 0;
    for(const auto& obj : objects) {
        const int visibility = objectVisibilityMask[index++];
        if(visibility == 0)
            continue;

        obj.program.use();
        obj.program.set("view", camera.view());
        obj.program.set("toView", camera.position);
        if(visibility == 1)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        obj.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Rendering::Scene::mouseCallback(GLFWwindow *window, double xPosition, double yPosition)
{
    if (m_firstMouse) {
        m_lastX = xPosition;
        m_lastY = yPosition;
        m_firstMouse = false;
    }

    float xoffset = xPosition - m_lastX;
    float yoffset = m_lastY - yPosition;
    m_lastX = xPosition;
    m_lastY = yPosition;

    camera.processMouse(xoffset, yoffset, GL_TRUE);
}

void Rendering::Scene::keyboardCallback(GLFWwindow *window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);


    for(int i = 0; i < objects.size(); i++) {
        if (glfwGetKey(window, GLFW_KEY_1 + i) != GLFW_PRESS)
            m_wireframeToggles[i] = false;
        if (glfwGetKey(window, GLFW_KEY_1 + i) == GLFW_PRESS && !m_wireframeToggles[i]) {
            m_wireframeToggles[i] = true;
            toggleObjectVisibility(i);
        }
    }

    using CM = FirstPersonFlatCamera::CameraMovement;

    CM cm = CM::cNone;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cm |= CM::cForward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cm |= CM::cLeft;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cm |= CM::cBackward;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cm |= CM::cRight;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cm |= CM::cUp;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        cm |= CM::cDown;

    camera.processKeyboard(cm, deltaTime);
}