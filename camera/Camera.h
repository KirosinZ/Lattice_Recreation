#ifndef CAMERA_CAMERA_H
#define CAMERA_CAMERA_H

#include <glm/glm.hpp>
#include <GL/glew.h>

namespace FirstPersonFlatCamera {
    enum class CameraMovement
    {
        cNone = 0,
        cForward = 1,
        cBackward = 2,
        cLeft = 4,
        cRight = 8,
        cUp = 16,
        cDown = 32,
    };

    inline CameraMovement operator~ (CameraMovement a);
    inline CameraMovement operator| (CameraMovement a, CameraMovement b);
    inline CameraMovement operator& (CameraMovement a, CameraMovement b);
    inline CameraMovement operator^ (CameraMovement a, CameraMovement b);
    inline CameraMovement& operator|= (CameraMovement& a, CameraMovement b);
    inline CameraMovement& operator&= (CameraMovement& a, CameraMovement b);
    inline CameraMovement& operator^= (CameraMovement& a, CameraMovement b);

    class Camera
    {
    public:
        glm::vec3 position;
        glm::vec3 localFront;
        glm::vec3 localUp;
        glm::vec3 localRight;
        glm::vec3 globalUp;

        float yaw = 0.0f;
        float pitch = 0.0f;

        float moveSpeed = 2.5f;
        float mouseSensitivity = 0.1f;
        float zoom = 45.0f;

        Camera();

        [[maybe_unused]]
        Camera(
                glm::vec3 pos,
                glm::vec3 up,
                float yaw,
                float pitch);

        [[nodiscard]]
        glm::mat4 view() const;

        void processKeyboard(CameraMovement direction, float deltaTime);

        void processMouse(float xOffset, float yOffset, GLboolean constrainPitch = GL_TRUE);

    private:

        void updateVectors();
    };








    // IMPL
    inline CameraMovement operator~ (CameraMovement a)
    {
        return (CameraMovement)~(int)a;
    }

    inline CameraMovement operator| (CameraMovement a, CameraMovement b)
    {
        return (CameraMovement)((int)a | (int)b);
    }

    inline CameraMovement operator& (CameraMovement a, CameraMovement b)
    {
        return (CameraMovement)((int)a & (int)b);
    }

    inline CameraMovement operator^ (CameraMovement a, CameraMovement b)
    {
        return (CameraMovement)((int)a ^ (int)b);
    }

    inline CameraMovement& operator|= (CameraMovement& a, CameraMovement b)
    {
        return (CameraMovement&)((int&)a |= (int)b);
    }

    inline CameraMovement& operator&= (CameraMovement& a, CameraMovement b)
    {
        return (CameraMovement&)((int&)a &= (int)b);
    }

    inline CameraMovement& operator^= (CameraMovement& a, CameraMovement b)
    {
        return (CameraMovement&)((int&)a ^= (int)b);
    }
}



#endif //CAMERA_CAMERA_H
