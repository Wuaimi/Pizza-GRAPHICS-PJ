#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

// Forward declarations
struct GLFWwindow; // Use forward declaration
class Camera;
class Renderer;
class Window;

class InputHandler {
public:
    InputHandler(Camera& cam, Renderer& rend, Window& win);

    void registerCallbacks(GLFWwindow* glfwWindow);

    // Methods called by static callbacks
    void processKeyInput(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
    void processMouseButton(GLFWwindow* glfwWindow, int button, int action, int mods);
    void processCursorPos(GLFWwindow* glfwWindow, double xpos, double ypos);
    void processScroll(GLFWwindow* glfwWindow, double xoffset, double yoffset);

    // Method called by Window's framebuffer callback
    void notifyScreenResize(unsigned int width, unsigned int height);

    // Static callback functions that GLFW will call
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

    // Getter needed for toggleFullscreen
    Window& getWindowRef() { return windowRef; }


private:
    Camera& camera;
    Renderer& renderer;
    Window& windowRef; // Store reference to Window for toggleFullscreen
    bool mousePressed;
};

#endif // INPUT_HANDLER_H