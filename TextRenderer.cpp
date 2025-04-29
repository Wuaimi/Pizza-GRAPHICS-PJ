#include "TextRenderer.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

// Define STB implementation in *one* .cpp file
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

// Shader sources for text rendering
const char* const TEXT_VERTEX_SHADER_SOURCE = R"glsl(
#version 330 core
layout (location = 0) in vec4 vertex; // vec2 pos, vec2 tex
out vec2 TexCoords;
uniform mat4 projection;
void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)glsl";

const char* const TEXT_FRAGMENT_SHADER_SOURCE = R"glsl(
#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D textTexture; // Our glyph atlas
uniform vec3 textColor;
void main() {
    float alpha = texture(textTexture, TexCoords).r;
    color = vec4(textColor, alpha);
}
)glsl";


TextRenderer::TextRenderer() :
    shaderProgram(0), VAO(0), VBO(0), glyphTextureID(0), projectionMatrix(1.0f)
{
}

TextRenderer::~TextRenderer() {} // Cleanup called explicitly

bool TextRenderer::init(const std::string& fontPath, float fontSize, int atlasWidth, int atlasHeight) {
    if (!loadShaders()) return false;

    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "ERROR::TEXTRENDERER::Failed to open font file: " << fontPath << std::endl;
        return false;
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(size);
    if (!file.read(reinterpret_cast<char*>(fontBuffer.data()), size)) {
        std::cerr << "ERROR::TEXTRENDERER::Failed to read font file: " << fontPath << std::endl;
        return false;
    }
    file.close();
    std::cout << "Font file loaded: " << fontPath << " (" << size << " bytes)" << std::endl;


    std::vector<unsigned char> atlasBitmap(atlasWidth * atlasHeight);
    int bakeResult = stbtt_BakeFontBitmap(fontBuffer.data(), 0, fontSize,
        atlasBitmap.data(), atlasWidth, atlasHeight,
        32, 96, bakedChars);

    if (bakeResult <= 0) {
        std::cerr << "ERROR::TEXTRENDERER::Failed to bake font bitmap. Result code: " << bakeResult << std::endl;
        return false;
    }
    std::cout << "Font bitmap baked. Max height reached: " << bakeResult << " pixels." << std::endl;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
    glGenTextures(1, &glyphTextureID);
    glBindTexture(GL_TEXTURE_2D, glyphTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, atlasBitmap.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // Restore default alignment
    std::cout << "Glyph texture atlas created (ID: " << glyphTextureID << ")" << std::endl;

    if (!setupBuffers()) return false;

    return true;
}

void TextRenderer::renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color) {
    if (glyphTextureID == 0) {
        // std::cerr << "WARN::TEXTRENDERER::Glyph texture not loaded, cannot render text." << std::endl;
        return;
    }

    glEnable(GL_BLEND); // Ensure blending is enabled
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.x, color.y, color.z);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, glyphTextureID);
    glUniform1i(glGetUniformLocation(shaderProgram, "textTexture"), 0);

    glBindVertexArray(VAO);

    float originalX = x;

    for (std::string::const_iterator c = text.begin(); c != text.end(); ++c) {
        if (*c == '\n') {
            // Estimate line height (can be improved)
            float lineHeight = (bakedChars['X' - 32].y1 - bakedChars['X' - 32].y0) * scale * 1.5f;
            y -= lineHeight; // Move down
            x = originalX; // Reset x
            continue;
        }
        if (*c < 32 || *c >= 128) { // Handle only ASCII 32-127
            // Advance using space character width
            stbtt_aligned_quad dummy_q;
            stbtt_GetBakedQuad(bakedChars, 512, 512, ' ' - 32, &x, &y, &dummy_q, 1);
            continue;
        }

        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(bakedChars, 512, 512, *c - 32, &x, &y, &q, 1); // 1=opengl & y grows up

        // Adjust quad position by scale - stbtt positions are baseline relative
        // We apply scale directly to vertices for simplicity here.
        float vertices[6][4] = {
            // Pos X,        Pos Y,          Tex X, Tex Y (Flipped T)
            { q.x0 * scale, q.y0 * scale,   q.s0, q.t1 }, // Bottom-Left Vertex uses Top T-Coord
            { q.x1 * scale, q.y0 * scale,   q.s1, q.t1 }, // Bottom-Right Vertex uses Top T-Coord
            { q.x0 * scale, q.y1 * scale,   q.s0, q.t0 }, // Top-Left Vertex uses Bottom T-Coord

            { q.x0 * scale, q.y1 * scale,   q.s0, q.t0 }, // Top-Left Vertex uses Bottom T-Coord
            { q.x1 * scale, q.y0 * scale,   q.s1, q.t1 }, // Bottom-Right Vertex uses Top T-Coord
            { q.x1 * scale, q.y1 * scale,   q.s1, q.t0 }  // Top-Right Vertex uses Bottom T-Coord
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    // Optionally disable blend if other things need it disabled
    // glDisable(GL_BLEND);
}

void TextRenderer::cleanup() {
    glDeleteTextures(1, &glyphTextureID);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glyphTextureID = VAO = VBO = shaderProgram = 0;
}

void TextRenderer::setProjection(const glm::mat4& projection) {
    projectionMatrix = projection;
}

bool TextRenderer::loadShaders() {
    shaderProgram = createShaderProgram(TEXT_VERTEX_SHADER_SOURCE, TEXT_FRAGMENT_SHADER_SOURCE);
    if (shaderProgram == 0) {
        std::cerr << "ERROR::TEXTRENDERER::SHADER_PROGRAM_LINKING_FAILED" << std::endl;
        return false;
    }
    std::cout << "Text Shader Program Loaded." << std::endl;
    return true;
}

bool TextRenderer::setupBuffers() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "Text VAO/VBO configured." << std::endl;
    return true;
}

GLuint TextRenderer::compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED (TextRenderer)\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint TextRenderer::createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader); glDeleteShader(fragmentShader); return 0;
    }
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED (TextRenderer)\n" << infoLog << std::endl;
        glDeleteProgram(program); program = 0;
    }
    glDeleteShader(vertexShader); glDeleteShader(fragmentShader);
    return program;
}