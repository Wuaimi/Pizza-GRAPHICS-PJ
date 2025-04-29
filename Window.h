#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h> // GLAD FIRST
#include <GLFW/glfw3.h>
#include <string>

// Forward declare to avoid circular includes
class InputHandler;

class Window {
public:
    Window();
    ~Window();

    bool init(unsigned int width, unsigned int height, const std::string& title);
    void setInputHandler(InputHandler* handler);
    // void processInput(); // Input handled by InputHandler callbacks now

    bool shouldClose() const;
    void swapBuffers();
    void pollEvents();
    void cleanup();

    GLFWwindow* getGLFWwindow() const { return glfwWindow; }
    unsigned int getWidth() const { return currentWidth; }
    unsigned int getHeight() const { return currentHeight; }

    // Callbacks need access to instance via user pointer
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void toggleFullscreen(GLFWwindow* window);

private:
    GLFWwindow* glfwWindow;
    GLFWmonitor* primaryMonitor;
    unsigned int currentWidth;
    unsigned int currentHeight;
    bool isFullscreen;

    // Store initial non-fullscreen size/pos for restoring
    int windowedPosX, windowedPosY;
    int windowedWidth, windowedHeight;

    // Keep pointer to notify handler on resize
    InputHandler* inputHandlerPtr;
};

#endif // WINDOW_H