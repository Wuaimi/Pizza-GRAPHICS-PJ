#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 pivotPoint;
    float yaw;
    float pitch;
    float radius;
    float fov;

    // Mouse state for movement calculation
    float lastX;
    float lastY;
    bool firstMouse;

    Camera(unsigned int screenWidth, unsigned int screenHeight);

    void reset(unsigned int screenWidth, unsigned int screenHeight);
    void processMouseMovement(float xpos, float ypos, bool shiftPressed);
    void processMouseScroll(float yoffset);
    void processKeyboardRotation(float yawOffset, float pitchOffset);
    void processKeyboardPan(const glm::vec3& offsetDirection);

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(float aspectRatio) const;

    void updateScreenSize(unsigned int width, unsigned int height);
};

#endif // CAMERA_H