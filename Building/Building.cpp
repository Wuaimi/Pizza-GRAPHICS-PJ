// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

// Screen settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// Camera
glm::vec3 cameraPos = glm::vec3(30.0f, 30.0f, 30.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool mousePressed = false;
float yaw = -135.0f;
float pitch = -30.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Building config
struct BuildingConfig {
    std::string type = "Commercial";
    int floors = 3;
    float floorHeight = 3.5f;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    float FAR = 5.0f;  // Floor Area Ratio
    float OSR = 0.3f;  // Open Space Ratio
};

BuildingConfig building;

void setupBuilding()
{
    building.type = "Commercial";
    building.floors = 3;
    building.floorHeight = 3.5f;
    building.FAR = 5.0f;
    building.OSR = 0.3f;
}

int main()
{
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Building Viewer", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    setupBuilding();

    // Cube vertices
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Shader setup
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    )";

    const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 color;
    void main()
    {
        FragColor = vec4(color, 1.0);
    }
    )";

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.85f, 0.85f, 0.85f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
        unsigned int colorLoc = glGetUniformLocation(shaderProgram, "color");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);

        // Draw ground
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(50.0f, 1.0f, 50.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.8f, 0.8f, 0.8f);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw building
// Draw building
        for (int i = 0; i < building.floors; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            // ตำแหน่งพื้นชั้นนี้
            float yOffset = (i * building.floorHeight) + (building.floorHeight / 2.0f);

            // วาดกล่องสีเทา (พื้นชั้น)
            model = glm::translate(model, building.position + glm::vec3(0.0f, yOffset, 0.0f));
            model = glm::scale(model, glm::vec3(4.0f, building.floorHeight, 4.0f));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3f(colorLoc, 0.7f, 0.7f, 0.7f); // สีเทาอ่อน
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // วาด "แผ่นดำ" (เป็นกล่องบางๆ อยู่บนพื้นชั้น)
            glm::mat4 outlineModel = glm::mat4(1.0f);

            // เพิ่มความสูงขึ้นนิดนึง
            float outlineYOffset = yOffset + (building.floorHeight / 2.0f) + 0.01f; // ดันขึ้นมานิดเดียวกันพื้น

            outlineModel = glm::translate(outlineModel, building.position + glm::vec3(0.0f, outlineYOffset, 0.0f));

            // ให้บางๆมาก เช่นสูงแค่ 0.02
            outlineModel = glm::scale(outlineModel, glm::vec3(4.0f, 0.02f, 4.05f));
            // ทำให้กว้างกว่านิดๆ (4.05 แทน 4.0) เพื่อยื่นออกมาหน่อย

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(outlineModel));
            glUniform3f(colorLoc, 0.0f, 0.0f, 0.0f); // สีดำสนิท
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        //model = glm::mat4(1.0f);
        //model = glm::translate(model, building.position + glm::vec3(0.0f, (building.floors * building.floorHeight) / 2.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(4.0f, building.floors * building.floorHeight, 4.0f));
        //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        //glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
        //glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw boundary (FAR, OSR)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        model = glm::mat4(1.0f);
        float areaScale = glm::sqrt(building.FAR); // simplify assumption
        model = glm::scale(model, glm::vec3(10.0f * areaScale, 1.0f, 10.0f * areaScale));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniform3f(colorLoc, 0.0f, 0.2f, 0.8f);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Building Info");

        const char* buildingTypes[] = { "Commercial", "Residential" };
        static int currentType = (building.type == "Residential") ? 1 : 0;

        // ตัวแปร input ชั่วคราว
        static int floorsInput = building.floors;
        static float floorHeightInput = building.floorHeight;
        static float farInput = building.FAR;
        static float osrInput = building.OSR;

        if (ImGui::Combo("Type", &currentType, buildingTypes, IM_ARRAYSIZE(buildingTypes))) {
            building.type = buildingTypes[currentType];
        }

        // edit ตัวแปร input
        ImGui::InputInt("Floors", &floorsInput);
        ImGui::InputFloat("Floor Height", &floorHeightInput);
        ImGui::InputFloat("FAR", &farInput);
        ImGui::InputFloat("OSR", &osrInput);

        // กดปุ่ม Apply เพื่อเซฟค่ากลับเข้า building จริงๆ
        if (ImGui::Button("Apply Changes")) {
            building.floors = floorsInput;
            building.floorHeight = floorHeightInput;
            building.type = buildingTypes[currentType];

            // ตั้งค่า FAR และ OSR ใหม่ตามประเภท
            if (building.type == "Commercial") {
                building.FAR = 5.0f;
                building.OSR = 0.3f;
            }
            else if (building.type == "Residential") {
                building.FAR = 2.5f;
                building.OSR = 0.5f;
            }

            // อัพเดตตัวแปร input ด้วย
            farInput = building.FAR;
            osrInput = building.OSR;
        }

        ImGui::Text("Total Height: %.2f meters", floorsInput * floorHeightInput);

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    // หลังจากนี้ (นอกลูป) จบการทำงานได้เลย
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
,

// input, mouse, resize callbacks เหมือนเดิมตามที่เคยส่งไว้
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 10.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!mousePressed)
        return;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

// glfw: whenever a mouse button is pressed/released, this callback is called
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            mousePressed = true;
            firstMouse = true;
        }
        else if (action == GLFW_RELEASE)
        {
            mousePressed = false;
        }
    }
}
