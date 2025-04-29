#include "Renderer.h"
#include "Config.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "TextRenderer.h"

// Define STB_IMAGE_IMPLEMENTATION in exactly one .cpp file (this one is good)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const char* const BG_VERTEX_SHADER_SOURCE = R"glsl(
#version 330 core
layout (location = 0) in vec2 aPos; // Position only (in screen coordinates)

uniform mat4 projection; // Orthographic projection

void main() {
    gl_Position = projection * vec4(aPos.x, aPos.y, 0.0, 1.0);
}
)glsl";

const char* const BG_FRAGMENT_SHADER_SOURCE = R"glsl(
#version 330 core
out vec4 FragColor;

uniform vec4 backgroundColor; // RGBA color passed in

void main() {
    FragColor = backgroundColor;
}
)glsl";

Renderer::Renderer(TextRenderer& textRendererRef) :
    shaderProgram(0),
    mapVAO(0), mapVBO(0), mapEBO(0),
    currentTextureIndex(0),
    showHelp(false),
    textRenderer(textRendererRef),
    currentScreenWidth(0), currentScreenHeight(0)
{
    textureFiles = TEXTURE_FILES;
    textureIDs.resize(textureFiles.size());

    helpTextContent =
        "HELP (H: TOGGLE)\n"
        "---------------------\n"
        "LMB + DRAG: ROTATE | LMB + SHIFT + DRAG : PAN | SCROLL : ZOOM\n"
        "\n"
        "ARROWS : ROTATE | WASD : PAN PIVOT | Q / E : UP / DOWN\n"
        "\n"
        "TAB : RESET | SPACE : CYCLE MAP | F11 : FULLSCREEN | ESC : EXIT\n";
}

Renderer::~Renderer() {} // Cleanup called explicitly

void Renderer::updateScreenSize(unsigned int width, unsigned int height) {
    currentScreenWidth = width;
    currentScreenHeight = height;
    // Update TextRenderer's projection matrix for ortho rendering
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
    textRenderer.setProjection(projection);
}

bool Renderer::init(unsigned int screenWidth, unsigned int screenHeight) {
    currentScreenWidth = screenWidth;
    currentScreenHeight = screenHeight;

    if (!loadShaders()) return false;
    if (!setupMapBuffers()) return false;
    if (!loadMapTextures()) return false;

    updateScreenSize(screenWidth, screenHeight); // Set initial projection for text

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_BLEND); // Needed for text alpha blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    return true;
}

bool Renderer::loadShaders() {
    shaderProgram = createShaderProgram(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);
    if (shaderProgram == 0) {
        std::cerr << "ERROR::RENDERER::3D_SHADER_PROGRAM_LINKING_FAILED" << std::endl;
        return false;
    }
    std::cout << "3D Shader Program Loaded." << std::endl;
    return true;
}

bool Renderer::setupMapBuffers() {
    float mapVertices[] = {
        2.0f,  2.0f, 0.0f,  1.0f, 1.0f,
        2.0f, -2.0f, 0.0f,  1.0f, 0.0f,
       -2.0f, -2.0f, 0.0f,  0.0f, 0.0f,
       -2.0f,  2.0f, 0.0f,  0.0f, 1.0f
    };
    unsigned int mapIndices[] = { 0, 1, 3, 1, 2, 3 };

    glGenVertexArrays(1, &mapVAO);
    glGenBuffers(1, &mapVBO);
    glGenBuffers(1, &mapEBO);

    glBindVertexArray(mapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mapVertices), mapVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mapEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(mapIndices), mapIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    return true;
}

bool Renderer::loadMapTextures() {
    glGenTextures(textureIDs.size(), textureIDs.data());
    bool success = true;
    for (size_t i = 0; i < textureFiles.size(); ++i) {
        loadSpecificTexture(i);
        if (textureIDs[i] == 0) {
            success = false;
        }
    }
    if (!success) {
        std::cerr << "WARNING::RENDERER::MAP_TEXTURE_LOADING_FAILED for one or more textures." << std::endl;
    }
    return true;
}

void Renderer::loadSpecificTexture(size_t index) {
    if (index >= textureFiles.size()) return;
    const std::string& file = textureFiles[index];
    GLuint& texID = textureIDs[index];

    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Test if your textures need flipping
    unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = GL_RGB;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        std::cout << "Loaded Map Texture: " << file << std::endl;
    }
    else {
        std::cerr << "ERROR::RENDERER::Failed to load map texture: " << file << std::endl;
        glBindTexture(GL_TEXTURE_2D, 0);
        texID = 0; // Mark as invalid
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::draw(const Camera& camera, unsigned int screenWidth, unsigned int screenHeight) {
    if (screenWidth != currentScreenWidth || screenHeight != currentScreenHeight) {
        updateScreenSize(screenWidth, screenHeight);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw 3D Map
    float aspectRatio = (screenHeight == 0) ? 1.0f : (float)screenWidth / (float)screenHeight;
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
    drawMap(view, projection);

    // Draw Help Text
    if (showHelp) {
        drawHelpText();
    }
}

void Renderer::drawMap(const glm::mat4& view, const glm::mat4& projection) {
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND); // Usually disable blend for opaque map

    glUseProgram(shaderProgram);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glActiveTexture(GL_TEXTURE0);
    GLuint currentTex = (currentTextureIndex < textureIDs.size()) ? textureIDs[currentTextureIndex] : 0;
    glBindTexture(GL_TEXTURE_2D, currentTex != 0 ? currentTex : 0); // Bind 0 if invalid
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

    glBindVertexArray(mapVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::drawHelpText() {
    glDisable(GL_DEPTH_TEST); // Draw UI on top
    glEnable(GL_BLEND); // Ensure blend is on for text
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float xPos = 10.0f;
    // Set initial Y baseline near the TOP edge
    float yPos = 150;//static_cast<float>(currentScreenHeight) - 30.0f; // e.g., 30 pixels down from top

    float scale = 0.4f;
	glm::vec3 color = glm::vec3(1.0f, 0.3f, 0.0f); // Orange color for text

    textRenderer.renderText(helpTextContent, xPos, yPos, scale, color);

    glEnable(GL_DEPTH_TEST);
}

void Renderer::switchTexture() {
    size_t originalIndex = currentTextureIndex;
    int attempts = 0;
    if (textureIDs.empty()) return; // No textures to switch

    do {
        currentTextureIndex = (currentTextureIndex + 1) % textureIDs.size();
        attempts++;
        if (currentTextureIndex < textureIDs.size() && textureIDs[currentTextureIndex] != 0) {
            break; // Found valid texture
        }
    } while (currentTextureIndex != originalIndex && attempts <= textureIDs.size());

    if (currentTextureIndex < textureFiles.size() && textureIDs[currentTextureIndex] != 0) {
        std::cout << "Switched to Map: " << textureFiles[currentTextureIndex] << std::endl;
    }
    else {
        std::cerr << "Warning: No valid map textures found to switch to, or current index invalid." << std::endl;
        currentTextureIndex = originalIndex; // Revert if no valid found after checking all
    }
}

void Renderer::toggleHelp() {
    showHelp = !showHelp;
    std::cout << "Help display toggled: " << (showHelp ? "ON" : "OFF") << std::endl;
}

void Renderer::cleanup() {
    glDeleteVertexArrays(1, &mapVAO);
    glDeleteBuffers(1, &mapVBO);
    glDeleteBuffers(1, &mapEBO);
    glDeleteProgram(shaderProgram);
    if (!textureIDs.empty()) {
        glDeleteTextures(textureIDs.size(), textureIDs.data());
    }
    mapVAO = mapVBO = mapEBO = shaderProgram = 0;
    textureIDs.clear();
    showHelp = false;
}

GLuint Renderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success; char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED (Renderer)\n" << infoLog << std::endl;
        glDeleteShader(shader); return 0;
    } return shader;
}

GLuint Renderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vs == 0 || fs == 0) { glDeleteShader(vs); glDeleteShader(fs); return 0; }
    GLuint program = glCreateProgram();
    glAttachShader(program, vs); glAttachShader(program, fs); glLinkProgram(program);
    int success; char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED (Renderer)\n" << infoLog << std::endl;
        glDeleteProgram(program); program = 0;
    } glDeleteShader(vs); glDeleteShader(fs); return program;
}