#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>
#include <glm/glm.hpp>

// --- Window Configuration ---
const unsigned int INITIAL_SCR_WIDTH = 800;
const unsigned int INITIAL_SCR_HEIGHT = 600;
const char* const WINDOW_TITLE = "3D Map Demo";

// --- Camera Configuration ---
const float INITIAL_YAW = 0.0f;
const float INITIAL_PITCH = 20.0f;
const float INITIAL_RADIUS = 3.0f;
const float INITIAL_FOV = 45.0f;
const glm::vec3 INITIAL_PIVOT_POINT = glm::vec3(0.0f, 0.0f, 0.0f);
const float MIN_PITCH = -0.1f;
const float MAX_PITCH = 89.0f;
const float MIN_RADIUS = 1.0f;
const float MAX_RADIUS = 10.0f;
const float MIN_PIVOT_Y = -0.5f; // Limit for Q/E panning
const float MAX_PIVOT_Y = 1.0f;  // Limit for Q/E panning
const float MOUSE_SENSITIVITY = 0.2f;
const float SCROLL_SENSITIVITY = 0.1f;
const float KEY_MOVE_SPEED = 0.1f;
const float KEY_ROTATE_SPEED = 10.0f;
const float SHIFT_PAN_SPEED_MULTIPLIER = 0.01f; // Multiplier for panning speed with shift

// --- Texture Configuration ---
const std::vector<std::string> TEXTURE_FILES = {
    "textures/map1.png",
    "textures/map2.png",
    "textures/map_A.png",
    "textures/map_B.png",
    "textures/map_C.png"
};

// --- Shader Sources ---
const char* const VERTEX_SHADER_SOURCE = R"glsl(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoord;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)glsl";

const char* const FRAGMENT_SHADER_SOURCE = R"glsl(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D texture1;
void main() {
    FragColor = texture(texture1, TexCoord);
}
)glsl";


#endif // CONFIG_H