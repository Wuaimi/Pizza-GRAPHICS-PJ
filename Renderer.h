#ifndef RENDERER_H
#define RENDERER_H

#include <glad/glad.h>
#include <vector>
#include <string>
#include "Camera.h"
#include <glm/glm.hpp>
#include "TextRenderer.h" // Include the new header

class Renderer {
public:
    Renderer(TextRenderer& textRendererRef);
    ~Renderer();

    bool init(unsigned int screenWidth, unsigned int screenHeight);
    void draw(const Camera& camera, unsigned int screenWidth, unsigned int screenHeight);
    void switchTexture();
    void cleanup();
    void toggleHelp();
    void updateScreenSize(unsigned int width, unsigned int height);

private:
    // 3D Map Resources
    GLuint shaderProgram;
    GLuint mapVAO, mapVBO, mapEBO;
    std::vector<GLuint> textureIDs;
    size_t currentTextureIndex;
    std::vector<std::string> textureFiles;

    // Help state
    bool showHelp;
    std::string helpTextContent; // Store the actual text content

    // Reference to the Text Renderer instance
    TextRenderer& textRenderer;

    // Screen dimensions
    unsigned int currentScreenWidth;
    unsigned int currentScreenHeight;

    // Initialization Helpers
    bool loadShaders();
    bool setupMapBuffers();
    bool loadMapTextures();
    void loadSpecificTexture(size_t index); // Renamed loadTexture helper

    // Shader Helpers (Specific to Renderer's 3D shader)
    GLuint compileShader(GLenum type, const char* source);
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);

    // Drawing Helpers
    void drawMap(const glm::mat4& view, const glm::mat4& projection);
    void drawHelpText();
};

#endif // RENDERER_H