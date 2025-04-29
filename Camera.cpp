#include "Camera.h"
#include "Config.h"
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm> // for std::clamp or use glm::clamp

Camera::Camera(unsigned int screenWidth, unsigned int screenHeight) {
    reset(screenWidth, screenHeight);
}

void Camera::reset(unsigned int screenWidth, unsigned int screenHeight) {
    pivotPoint = INITIAL_PIVOT_POINT;
    yaw = INITIAL_YAW;
    pitch = INITIAL_PITCH;
    radius = INITIAL_RADIUS;
    fov = INITIAL_FOV;
    lastX = static_cast<float>(screenWidth) / 2.0f;
    lastY = static_cast<float>(screenHeight) / 2.0f;
    firstMouse = true;
}

void Camera::processMouseMovement(float xpos, float ypos, bool shiftPressed) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    if (shiftPressed) {
        float radYaw = glm::radians(yaw);
        float radPitch = glm::radians(pitch);
        // Simplified panning - might not be perfect camera relative
        glm::vec3 front = glm::normalize(glm::vec3(
            cos(radYaw) * cos(radPitch),
            sin(radPitch),
            sin(radYaw) * cos(radPitch)
        ));
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(front, worldUp));

        pivotPoint += right * xoffset * SHIFT_PAN_SPEED_MULTIPLIER;
        // Approximate vertical panning
        pivotPoint.y -= yoffset * SHIFT_PAN_SPEED_MULTIPLIER;

    }
    else {
        yaw += xoffset;
        pitch += yoffset;
    }

    // Constrain pitch and pivot y
    pitch = glm::clamp(pitch, MIN_PITCH, MAX_PITCH);
    pivotPoint.y = glm::clamp(pivotPoint.y, MIN_PIVOT_Y, MAX_PIVOT_Y);
}

void Camera::processMouseScroll(float yoffset) {
    radius -= yoffset * SCROLL_SENSITIVITY;
    radius = glm::clamp(radius, MIN_RADIUS, MAX_RADIUS);
}

void Camera::processKeyboardRotation(float yawOffset, float pitchOffset) {
    yaw += yawOffset * KEY_ROTATE_SPEED;
    pitch += pitchOffset * KEY_ROTATE_SPEED;
    pitch = glm::clamp(pitch, MIN_PITCH, MAX_PITCH);
}

void Camera::processKeyboardPan(const glm::vec3& offsetDirection) {
    // Calculate the potential new position based on direction and speed
    glm::vec3 movement = offsetDirection * KEY_MOVE_SPEED;
    glm::vec3 potentialPivotPoint = pivotPoint + movement;

    // Apply the movement for X and Z axes directly
    pivotPoint.x = potentialPivotPoint.x;
    pivotPoint.z = potentialPivotPoint.z;

    // Apply the movement for the Y axis BUT clamp it immediately afterwards
    pivotPoint.y = potentialPivotPoint.y;
    pivotPoint.y = glm::clamp(pivotPoint.y, MIN_PIVOT_Y, MAX_PIVOT_Y);
}


glm::mat4 Camera::getViewMatrix() const {
    float radYaw = glm::radians(yaw);
    float radPitch = glm::radians(pitch);

    float camX = radius * cos(radPitch) * sin(radYaw);
    float camY = radius * sin(radPitch);
    float camZ = radius * cos(radPitch) * cos(radYaw);

    glm::vec3 cameraPos = pivotPoint + glm::vec3(camX, camY, camZ);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    return glm::lookAt(cameraPos, pivotPoint, worldUp);
}

glm::mat4 Camera::getProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
}

void Camera::updateScreenSize(unsigned int width, unsigned int height) {
    // Resetting mouse coords on resize might be useful if not using firstMouse correctly
    lastX = static_cast<float>(width) / 2.0f;
    lastY = static_cast<float>(height) / 2.0f;
    firstMouse = true;
}