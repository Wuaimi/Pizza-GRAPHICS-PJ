#include "Window.h"
#include "InputHandler.h" // Include full definition now
#include "Config.h"
#include <iostream>

Window::Window() :
    glfwWindow(nullptr),
    primaryMonitor(nullptr),
    currentWidth(INITIAL_SCR_WIDTH),
    currentHeight(INITIAL_SCR_HEIGHT),
    isFullscreen(false),
    inputHandlerPtr(nullptr),
    windowedPosX(100), windowedPosY(100),
    windowedWidth(INITIAL_SCR_WIDTH), windowedHeight(INITIAL_SCR_HEIGHT)
{
}

Window::~Window() {
    cleanup();
}

bool Window::init(unsigned int width, unsigned int height, const std::string& title) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    currentWidth = width;
    currentHeight = height;
    windowedWidth = width;
    windowedHeight = height;

    primaryMonitor = glfwGetPrimaryMonitor();
    if (!primaryMonitor) {
        std::cerr << "Failed to get primary monitor" << std::endl;
    }

    glfwWindow = glfwCreateWindow(currentWidth, currentHeight, title.c_str(), NULL, NULL);
    if (!glfwWindow) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(glfwWindow);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return false;
    }

    glViewport(0, 0, currentWidth, currentHeight);

    // Store 'this' pointer initially for framebuffer callback setup
    // InputHandler will later overwrite this with its own pointer
    glfwSetWindowUserPointer(glfwWindow, this);
    glfwSetFramebufferSizeCallback(glfwWindow, framebuffer_size_callback);

    glfwGetWindowPos(glfwWindow, &windowedPosX, &windowedPosY);

    return true;
}

// This function is crucial for linking callbacks to InputHandler
void Window::setInputHandler(InputHandler* handler) {
    inputHandlerPtr = handler;
    if (inputHandlerPtr) {
        // InputHandler now takes over setting the user pointer and callbacks
        inputHandlerPtr->registerCallbacks(glfwWindow);
    }
    else {
        // Unregister callbacks if handler is removed
        glfwSetWindowUserPointer(glfwWindow, this); // Revert user pointer?
        glfwSetKeyCallback(glfwWindow, nullptr);
        glfwSetMouseButtonCallback(glfwWindow, nullptr);
        glfwSetCursorPosCallback(glfwWindow, nullptr);
        glfwSetScrollCallback(glfwWindow, nullptr);
    }
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(glfwWindow);
}

void Window::swapBuffers() {
    glfwSwapBuffers(glfwWindow);
}

void Window::pollEvents() {
    glfwPollEvents();
}

void Window::cleanup() {
    if (glfwWindow) {
        glfwDestroyWindow(glfwWindow);
        glfwWindow = nullptr;
    }
    glfwTerminate();
}

// --- Static Callback Implementations ---

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // IMPORTANT: Get the InputHandler instance (which should be stored in user ptr)
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(window));

    // Update viewport regardless
    glViewport(0, 0, width, height);

    if (handler) {
        // Notify InputHandler, which will notify Camera and Renderer
        handler->notifyScreenResize(width, height);
        std::cout << "Window resized (via InputHandler): " << width << "x" << height << std::endl;
    }
    else {
        // Fallback if InputHandler hasn't set the user pointer yet
        // (e.g., during initial setup)
        Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (windowInstance) {
            windowInstance->currentWidth = width;
            windowInstance->currentHeight = height;
            // Cannot easily notify Renderer/Camera here without the handler
            std::cout << "Window resized (Window instance only): " << width << "x" << height << std::endl;
        }
    }
}

void Window::toggleFullscreen(GLFWwindow* glfwWin) {
    // Need access to the Window instance variables (isFullScreen, monitor, etc.)
    // Assume InputHandler has set the user pointer correctly
    InputHandler* handler = static_cast<InputHandler*>(glfwGetWindowUserPointer(glfwWin));
    Window* windowInstance = nullptr;
    if (handler) {
        // Get the Window instance via the handler (assuming it holds a reference)
        windowInstance = &handler->getWindowRef(); // Need a getter in InputHandler
    }

    // Fallback: try getting Window instance directly (might be stale if handler set pointer)
    if (!windowInstance) {
        windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(glfwWin));
    }


    if (!windowInstance || !windowInstance->primaryMonitor) {
        std::cerr << "Error toggling fullscreen: Window instance or primary monitor not found." << std::endl;
        return;
    }

    windowInstance->isFullscreen = !windowInstance->isFullscreen;

    if (windowInstance->isFullscreen) {
        glfwGetWindowPos(glfwWin, &windowInstance->windowedPosX, &windowInstance->windowedPosY);
        // Use stored windowed dimensions, not current (might be fullscreen size)
        // glfwGetWindowSize(glfwWin, &windowInstance->windowedWidth, &windowInstance->windowedHeight);

        const GLFWvidmode* mode = glfwGetVideoMode(windowInstance->primaryMonitor);
        glfwSetWindowMonitor(glfwWin, windowInstance->primaryMonitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        std::cout << "Entering Fullscreen" << std::endl;
    }
    else {
        glfwSetWindowMonitor(glfwWin, nullptr,
            windowInstance->windowedPosX, windowInstance->windowedPosY,
            windowInstance->windowedWidth, windowInstance->windowedHeight, 0);
        std::cout << "Exiting Fullscreen" << std::endl;
    }
    // framebuffer_size_callback should be triggered automatically
}