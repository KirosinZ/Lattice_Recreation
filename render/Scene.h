#ifndef RENDER_SCENE_H
#define RENDER_SCENE_H

#include "..\render\RenderObject.h"
#include "..\camera\Camera.h"
#include <GLFW/glfw3.h>

namespace Rendering {

    struct Scene {
        std::vector<Rendering::RenderObject> objects;
        std::vector<int> objectVisibilityMask;
        FirstPersonFlatCamera::Camera camera;
        glm::mat4 projectionMatrix;

        void mouseCallback(GLFWwindow *window, double xPosition, double yPosition);
        void keyboardCallback(GLFWwindow *window, float deltaTime);

        void addObject(const Rendering::RenderObject &object);
        void toggleObjectVisibility(int index);

        void frame() const;

    private:
        bool m_firstMouse = true;
        float m_lastX = 0.0f;
        float m_lastY = 0.0f;

        std::vector<bool> m_wireframeToggles;
    };

}

#endif //RENDER_SCENE_H
