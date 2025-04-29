#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <map>

// Include stb_truetype definitions here, but IMPLEMENTATION goes in .cpp
#include "stb_truetype.h"

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    bool init(const std::string& fontPath, float fontSize = 48.0f, int atlasWidth = 512, int atlasHeight = 512);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void cleanup();
    void setProjection(const glm::mat4& projection);

private:
    GLuint shaderProgram;
    GLuint VAO, VBO;
    GLuint glyphTextureID;

    stbtt_bakedchar bakedChars[96]; // ASCII 32..127

    glm::mat4 projectionMatrix;

    bool loadShaders();
    GLuint compileShader(GLenum type, const char* source);
    GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource);
    bool setupBuffers();

    float fontAscent;
    float fontDescent;
    float fontLineGap;
    float bakePixelHeight;
};

#endif // TEXT_RENDERER_H