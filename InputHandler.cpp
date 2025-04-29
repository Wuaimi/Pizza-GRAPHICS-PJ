#include "InputHandler.h"
#include "Camera.h"
#include "Renderer.h"
#include "Window.h"   // Include full definition for Window::toggleFullscreen etc.
#include "Config.h"
#include <glad/glad.h> // Include glad before glfw
#include <GLFW/glfw3.h> // Need full definitions here
#include <iostream>
#include <glm/glm.hpp>

InputHandler::InputHandler(Camera& cam, Renderer& rend, Window& win) :
    camera(cam),
    renderer(rend),
    windowRef(win),
    mousePressed(false)
{
}

void InputHandler::registerCallbacks(GLFWwindow* glfwWindow) {
    // Store 'this' pointer to access InputHandler instance in static callbacks
    glfwSetWindowUserPointer(glfwWindow, this);

    // Set all callbacks handled by this class
    glfwSetKeyCallback(glfwWindow, key_callback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);
    glfwSetCursorPosCallback(glfwWindow, cursor_position_callback);
    glfwSetScrollCallback(glfwWindow, scroll_callback);

    // Framebuffer callback remains set by Window, but it uses the user pointer
    // that *this* class has now set to find the InputHandler instance.
}

void InputHandler::notifyScreenResize(unsigned int width, unsigned int height) {
    camera.updateScreenSize(width, height);
    renderer.updateScreenSize(width, height); // Notify renderer too
}

void InputHandler::processKeyInput(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Camera Rotation (Arrows)
        if (key == GLFW_KEY_UP)    camera.processKeyboardRotation(0.0f, 1.0f);
        if (key == GLFW_KEY_DOWN)  camera.processKeyboardRotation(0.0f, -1.0f);
        if (key == GLFW_KEY_LEFT)  camera.processKeyboardRotation(-1.0f, 0.0f);
        if (key == GLFW_KEY_RIGHT) camera.processKeyboardRotation(1.0f, 0.0f);

        // Pivot Panning (WASDQE)
        if (key == GLFW_KEY_W) camera.processKeyboardPan(glm::vec3(0.0f, 0.0f, -1.0f));
        if (key == GLFW_KEY_S) camera.processKeyboardPan(glm::vec3(0.0f, 0.0f, 1.0f));
        if (key == GLFW_KEY_A) camera.processKeyboardPan(glm::vec3(-1.0f, 0.0f, 0.0f));
        if (key == GLFW_KEY_D) camera.processKeyboardPan(glm::vec3(1.0f, 0.0f, 0.0f));
        if (key == GLFW_KEY_Q) camera.processKeyboardPan(glm::vec3(0.0f, -1.0f, 0.0f));
        if (key == GLFW_KEY_E) camera.processKeyboardPan(glm::vec3(0.0f, 1.0f, 0.0f));

        // Fullscreen (F11 Press)
        if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
            Window::toggleFullscreen(glfwWindow); // Call static method
        }

        // Close (Escape Press)
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(glfwWindow, true);
        }
    }

    // --- Press-Only Actions ---
    if (action == GLFW_PRESS) {
        // Reset Camera (Tab)
        if (key == GLFW_KEY_TAB) {
            int width, height;
            glfwGetFramebufferSize(glfwWindow, &width, &height);
            camera.reset(width, height);
            std::cout << "Camera Reset" << std::endl;
        }

        // Cycle Texture (Space)
        if (key == GLFW_KEY_SPACE) {
            renderer.switchTexture();
        }

        // Toggle Help (H)
        if (key == GLFW_KEY_H) {
            renderer.toggleHelp();
        }
    }
}

void InputHandler::processMouseButton(GLFWwindow* glfwWindow, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            mousePressed = true;
            camera.firstMouse = true;
        }
        else if (action == GLFW_RELEASE) {
            mousePressed = false;
        }
    }
}

void InputHandler::processCursorPos(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (!mousePressed) return;
    bool shiftPressed = (glfwGetKey(glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(glfwWindow, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS);
    camera.processMouseMovement(static_cast<float>(xpos), static_cast<float>(ypos), shiftPressed);
}

void InputHandler::processScroll(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// --- Static Callback Wrappers ---
void InputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) handler->processKeyInput(window, key, scancode, action, mods);
}

void InputHandler::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) handler->processMouseButton(window, button, action, mods);
}

void InputHandler::cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) handler->processCursorPos(window, xpos, ypos);
}

void InputHandler::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));
    if (handler) handler->processScroll(window, xoffset, yoffset);
}