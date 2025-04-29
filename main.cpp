#include "Window.h"
#include "Renderer.h"
#include "Camera.h"
#include "InputHandler.h"
#include "TextRenderer.h"
#include "Config.h"
#include <iostream>

int main() {
    // 1. Initialize Window
    Window window;
    if (!window.init(INITIAL_SCR_WIDTH, INITIAL_SCR_HEIGHT, WINDOW_TITLE)) {
        std::cerr << "Failed to initialize Window" << std::endl;
        return -1;
    }

    // 2. Initialize Text Renderer (after GL context exists)
    TextRenderer textRenderer;
    // *** ADJUST FONT PATH AND NAME AS NEEDED ***
    if (!textRenderer.init("fonts/arial.ttf")) {
        std::cerr << "Failed to initialize Text Renderer" << std::endl;
        window.cleanup();
        return -1;
    }

    // 3. Initialize Renderer (pass TextRenderer)
    Renderer renderer(textRenderer);
    if (!renderer.init(window.getWidth(), window.getHeight())) {
        std::cerr << "Failed to initialize Renderer" << std::endl;
        textRenderer.cleanup();
        window.cleanup();
        return -1;
    }

    // 4. Initialize Camera
    Camera camera(window.getWidth(), window.getHeight());

    // 5. Initialize Input Handler (links everything)
    InputHandler inputHandler(camera, renderer, window);
    window.setInputHandler(&inputHandler); // Crucial: Registers callbacks


    // 6. Main Loop
    while (!window.shouldClose()) {
        // Poll events first to update input states handled by callbacks
        window.pollEvents();

        // --- Rendering ---
        renderer.draw(camera, window.getWidth(), window.getHeight());

        // --- Swap Buffers ---
        window.swapBuffers();
    }

    // 7. Cleanup (in reverse order of creation, roughly)
    textRenderer.cleanup();
    renderer.cleanup();
    window.cleanup();

    return 0;
}