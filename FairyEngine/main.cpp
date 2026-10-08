#include "core/Window.h"
#include "core/Texture.h"
#include "core/Player.h"
#include "core/Animation.h"
#include "core/Camera.h"
#include "core/Layer.h"
#include "core/SceneRenderer.h"
#include "core/PostProcessor.h"
#include "config/GameConfig.h"
#include "world/WorldManager.h"
#include "world/Door.h"
#include "game/ui/MainMenu.h"
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

using namespace fairy::core;
using namespace fairy::config;
using namespace fairy::world;
namespace config = fairy::config;

std::string framePath(const char* prefix, int index, int digits, const char* ext) {
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "assets/fog/%s_%0*d.%s",
        prefix, digits, index, ext);
    return std::string(buffer);
}

int main(int argc, char* argv[])
{
    try {
        const bool transitionTest = argc > 1
            && std::string(argv[1]) == "--test-transition";

        Window window(config::window.width, config::window.height,
            config::window.title);
        fairy::game::ui::MainMenu mainMenu(
            config::window.width,
            config::window.height,
            "assets/PoetryVelvetRegularDemo-pgzzv.ttf");
        bool gameStarted = false;

        WorldManager world;
        world.registerLocation("intro", "assets/locations/intro.location");
        world.registerLocation("test2", "assets/locations/test2.location");
        world.load("intro");

        Texture playerTexture("assets/dino_idle.png");
        Player player(&playerTexture, 0.0f, 0.0f);

        std::unique_ptr<Texture> roomTexture;
        std::unique_ptr<Texture> foregroundTexture;
        std::vector<std::unique_ptr<Texture>> fogTextures;
        std::unique_ptr<Animation> fogAnimation;
        std::unique_ptr<Layer> background;
        std::unique_ptr<Layer> foreground;
        std::vector<Layer*> layers;
        std::unique_ptr<SceneRenderer> sceneRenderer;
        std::unique_ptr<Door> door;
        Camera camera(static_cast<float>(config::window.width),
            static_cast<float>(config::window.height));

        auto loadLocation = [&]() {
            const Location& location = world.current();
            roomTexture = std::make_unique<Texture>(location.backgroundTexture);
            foregroundTexture.reset();

            fogTextures.clear();
            std::vector<const Texture*> fogFrames;
            for (int i = location.fogFirstFrame;
                i < location.fogFirstFrame + location.fogFrameCount; ++i) {
                auto texture = std::make_unique<Texture>(framePath(
                    location.fogPrefix.c_str(), i, location.fogDigits, "png"));
                fogFrames.push_back(texture.get());
                fogTextures.push_back(std::move(texture));
            }
            fogAnimation = std::make_unique<Animation>(fogFrames,
                location.fogFrameRate);

            const float worldWidth = static_cast<float>(roomTexture->width());
            const float worldHeight = static_cast<float>(roomTexture->height());
            player.setPosition(location.playerStartX, location.playerStartY);
            camera.setWorldBounds(worldWidth, worldHeight);
            camera.setPosition(location.playerStartX,
                config::camera.targetHeight * config::window.height);

            background = std::make_unique<Layer>(roomTexture.get(),
                worldWidth * 0.5f, worldHeight * 0.5f,
                worldWidth, worldHeight, location.backgroundParallax,
                location.backgroundOpacity);

            foreground.reset();
            layers.clear();
            layers.push_back(background.get());
            if (!location.foregroundTexture.empty()) {
                foregroundTexture = std::make_unique<Texture>(
                    location.foregroundTexture);
                foreground = std::make_unique<Layer>(foregroundTexture.get(),
                    worldWidth * 0.5f, worldHeight * 0.5f,
                    worldWidth, worldHeight, location.foregroundParallax,
                    location.foregroundOpacity);
                layers.push_back(foreground.get());
            }

            door = std::make_unique<Door>(
                worldWidth - location.doorWidth * 0.5f,
                location.playerStartY,
                location.doorWidth,
                location.doorHeight,
                location.doorInteractionDistance,
                location.nextLocation);
            sceneRenderer = std::make_unique<SceneRenderer>(
                config::window.width, config::window.height, layers,
                *fogAnimation, location.fogOpacity);
        };

        loadLocation();
        PostProcessor postProcessor(config::window.width, config::window.height,
            config::postProcess);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        enum class TransitionState { None, FadeOut, FadeIn };
        TransitionState transitionState = TransitionState::None;
        float transitionTimer = 0.0f;
        float locationTimer = 0.0f;
        std::string pendingLocation;
        float lastTime = static_cast<float>(glfwGetTime());
        while (!window.shouldClose()) {
            const float now = static_cast<float>(glfwGetTime());
            const float dt = now - lastTime;
            lastTime = now;

            double mouseX = 0.0;
            double mouseY = 0.0;
            glfwGetCursorPos(window.handle(), &mouseX, &mouseY);
            const float uiMouseX = static_cast<float>(mouseX);
            const float uiMouseY = config::window.height
                - static_cast<float>(mouseY);

            const Location& location = world.current();
            const bool clicked = glfwGetMouseButton(window.handle(),
                GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

            if (!gameStarted) {
                const auto action = mainMenu.update(
                    uiMouseX, uiMouseY, clicked);
                if (action == fairy::game::ui::MainMenuAction::StartGame) {
                    gameStarted = true;
                }
                else if (action == fairy::game::ui::MainMenuAction::Exit) {
                    glfwSetWindowShouldClose(window.handle(), GLFW_TRUE);
                }
            }

            if (gameStarted && transitionState == TransitionState::None) {
                locationTimer += dt;
                const bool left = glfwGetKey(window.handle(), GLFW_KEY_A) == GLFW_PRESS;
                const bool right = glfwGetKey(window.handle(), GLFW_KEY_D) == GLFW_PRESS;
                const bool jump = glfwGetKey(window.handle(), GLFW_KEY_SPACE) == GLFW_PRESS;
                player.update(dt, left, right, jump);
                camera.follow(player.x(),
                    config::camera.targetHeight * config::window.height, dt);
                door->update(dt);

                const bool automaticTestTransition = transitionTest
                    && locationTimer >= 10.0f;
                if ((door->tryInteract(player.x(), player.y(), clicked)
                    || automaticTestTransition)
                    && world.has(location.nextLocation)) {
                    pendingLocation = door->nextLocation();
                    transitionState = TransitionState::FadeOut;
                    transitionTimer = 0.0f;
                }
            }

            float fade = 0.0f;
            if (transitionState == TransitionState::FadeOut) {
                transitionTimer += dt;
                fade = transitionTimer / location.transitionDuration;
                if (transitionTimer >= location.transitionDuration) {
                    world.load(pendingLocation);
                    loadLocation();
                    locationTimer = 0.0f;
                    transitionState = TransitionState::FadeIn;
                    transitionTimer = world.current().transitionDuration;
                    fade = 1.0f;
                }
            }
            else if (transitionState == TransitionState::FadeIn) {
                transitionTimer -= dt;
                fade = transitionTimer / world.current().transitionDuration;
                if (transitionTimer <= 0.0f) {
                    transitionState = TransitionState::None;
                    fade = 0.0f;
                }
            }

            fogAnimation->update(dt);
            sceneRenderer->render(camera);

            const float focusX = player.x() - camera.x()
                + config::window.width * 0.5f;
            const float focusY = player.y() - camera.y()
                + config::window.height * 0.5f
                + config::postProcess.focusHeightOffset;
            postProcessor.render(sceneRenderer->colorTexture(), focusX, focusY,
                fade);

            if (!gameStarted) {
                mainMenu.draw(uiMouseX, uiMouseY);
            }

            window.swapBuffers();
            window.pollEvents();
        }
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return -1;
    }
    return 0;
}