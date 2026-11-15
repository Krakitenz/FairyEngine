#include "config/GameConfig.h"
#include "core/Animation.h"
#include "core/Camera.h"
#include "core/Layer.h"
#include "core/PostProcessor.h"
#include "core/SceneRenderer.h"
#include "core/Sprite.h"
#include "core/Texture.h"
#include "core/Window.h"
#include "world/Location.h"
#include "world/WorldManager.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

	using fairy::core::Animation;
	using fairy::core::Layer;
	using fairy::core::PostProcessor;
	using fairy::core::SceneRenderer;
	using fairy::core::Sprite;
	using fairy::core::Texture;
	using fairy::core::Window;
	using fairy::world::Location;
	using fairy::world::WorldManager;

	class SpriteGeometryCleanup {
	public:
		~SpriteGeometryCleanup() {
			Sprite::release();
		}
	};

	bool isProjectRoot(const std::filesystem::path& path) {
		return std::filesystem::exists(path / "assets" / "shaders" / "scene.vert");
	}

	std::filesystem::path searchUpwards(const std::filesystem::path& start) {
		std::filesystem::path candidate = start;
		while (!candidate.empty()) {
			if (isProjectRoot(candidate)) {
				return candidate;
			}
			const std::filesystem::path projectDirectory = candidate / "FairyEngine";
			if (isProjectRoot(projectDirectory)) {
				return projectDirectory;
			}

			const std::filesystem::path parent = candidate.parent_path();
			if (parent == candidate) {
				break;
			}
			candidate = parent;
		}
		return {};
	}

	std::filesystem::path findProjectRoot(const char* executablePath) {
		std::filesystem::path root = searchUpwards(std::filesystem::current_path());
		if (!root.empty()) {
			return root;
		}

		if (executablePath && *executablePath) {
			std::error_code error;
			const std::filesystem::path absolutePath =
				std::filesystem::absolute(executablePath, error);
			if (!error) {
				root = searchUpwards(absolutePath.parent_path());
				if (!root.empty()) {
					return root;
				}
			}
		}

		throw std::runtime_error("Could not find FairyEngine assets from the working or executable directory");
	}

	std::unique_ptr<Texture> loadTexture(const std::string& path, const char* role) {
		try {
			return std::make_unique<Texture>(path);
		}
		catch (const std::exception& error) {
			std::cerr << "[Resource] Could not load " << role << " '" << path
				<< "': " << error.what() << '\n';
			return nullptr;
		}
	}

	void fitLayerToViewport(Layer& layer, int width, int height) {
		const Texture* texture = layer.sprite.texture();
		const float scale = std::max(
			static_cast<float>(width) / static_cast<float>(texture->width()),
			static_cast<float>(height) / static_cast<float>(texture->height()));

		layer.sprite.setPosition(width * 0.5f, height * 0.5f);
		layer.sprite.setSize(texture->width() * scale, texture->height() * scale);
	}

	std::vector<std::string> makeFogFramePaths(const Location& location) {
		std::vector<std::string> paths;
		paths.reserve(static_cast<std::size_t>(location.fogFrameCount));

		for (int frame = location.fogFirstFrame;
			frame < location.fogFirstFrame + location.fogFrameCount; ++frame) {
			std::ostringstream path;
			path << "assets/fog/" << location.fogPrefix << '_'
				<< std::setfill('0') << std::setw(location.fogDigits) << frame << ".png";
			paths.push_back(path.str());
		}

		return paths;
	}

	Location loadInitialLocation(WorldManager& world) {
		try {
			world.load("intro");
			return world.current();
		}
		catch (const std::exception& error) {
			std::cerr << "[Resource] Could not load the intro scene: " << error.what()
				<< ". Using the default fallback scene.\n";
			return Location{};
		}
	}

	void runGame() {
		const fairy::config::WindowSettings& settings = fairy::config::window;
		Window window(settings.width, settings.height, settings.title);
		SpriteGeometryCleanup spriteGeometryCleanup;
		const int initialWidth = std::max(window.width(), 1);
		const int initialHeight = std::max(window.height(), 1);

		WorldManager world;
		world.registerLocation("intro", "assets/locations/intro.location");
		world.registerLocation("test2", "assets/locations/test2.location");
		const Location location = loadInitialLocation(world);

		std::vector<std::unique_ptr<Texture>> textures;
		std::vector<std::unique_ptr<Layer>> layerStorage;
		std::vector<Layer*> layers;

		auto appendLayer = [&](const std::string& path, float parallax,
			float opacity, const char* role) {
			std::unique_ptr<Texture> texture = loadTexture(path, role);
			if (!texture) {
				return false;
			}

			const Texture* texturePointer = texture.get();
			const float textureWidth = static_cast<float>(texture->width());
			const float textureHeight = static_cast<float>(texture->height());
			textures.push_back(std::move(texture));

			auto layer = std::make_unique<Layer>(texturePointer,
				initialWidth * 0.5f, initialHeight * 0.5f,
				textureWidth, textureHeight, parallax, opacity);
			fitLayerToViewport(*layer, initialWidth, initialHeight);
			layers.push_back(layer.get());
			layerStorage.push_back(std::move(layer));
			return true;
		};

		if (!appendLayer(location.backgroundTexture, location.backgroundParallax,
			location.backgroundOpacity, "background")) {
			if (location.backgroundTexture != "assets/room2.png") {
				appendLayer("assets/room2.png", location.backgroundParallax,
					location.backgroundOpacity, "fallback background");
			}
		}
		if (!location.foregroundTexture.empty()) {
			appendLayer(location.foregroundTexture, location.foregroundParallax,
				location.foregroundOpacity, "foreground");
		}
		if (layers.empty()) {
			std::cerr << "[Resource] No scene images could be loaded; showing the clear color.\n";
		}

		std::unique_ptr<Animation> fogAnimation;
		if (location.fogFrameCount > 0) {
			try {
				fogAnimation = std::make_unique<Animation>(
					makeFogFramePaths(location), location.fogFrameRate);
			}
			catch (const std::exception& error) {
				std::cerr << "[Resource] Fog animation disabled: " << error.what() << '\n';
			}
		}

		SceneRenderer sceneRenderer(initialWidth, initialHeight, layers,
			fogAnimation.get(), location.fogOpacity);
		PostProcessor postProcessor(initialWidth, initialHeight,
			fairy::config::postProcess);
		fairy::core::Camera camera(
			static_cast<float>(initialWidth), static_cast<float>(initialHeight));
		camera.setSmoothing(fairy::config::camera.smoothing);
		camera.setWorldBounds(static_cast<float>(initialWidth),
			static_cast<float>(initialHeight));

		int viewportWidth = initialWidth;
		int viewportHeight = initialHeight;
		auto previousFrame = std::chrono::steady_clock::now();
		while (!window.shouldClose()) {
			const auto now = std::chrono::steady_clock::now();
			const float dt = std::min(std::chrono::duration<float>(now - previousFrame).count(), 0.1f);
			previousFrame = now;

			window.pollEvents();
			const int width = window.width();
			const int height = window.height();
			if (width <= 0 || height <= 0) {
				continue;
			}

			if (width != viewportWidth || height != viewportHeight) {
				sceneRenderer.resize(width, height);
				postProcessor.resize(width, height);
				camera.setViewportSize(static_cast<float>(width),
					static_cast<float>(height));
				camera.setWorldBounds(static_cast<float>(width),
					static_cast<float>(height));
				for (Layer* layer : layers) {
					fitLayerToViewport(*layer, width, height);
				}
				viewportWidth = width;
				viewportHeight = height;
			}

			if (fogAnimation) {
				fogAnimation->update(dt);
			}
			sceneRenderer.render(camera);
			postProcessor.render(sceneRenderer.colorTexture(),
				width * 0.5f, height * 0.5f);
			window.swapBuffers();
		}
	}

}

int main(int argc, char** argv) {
	try {
		std::filesystem::current_path(findProjectRoot(argc > 0 ? argv[0] : nullptr));
		runGame();
	}
	catch (const std::exception& error) {
		std::cerr << "[Fatal] " << error.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}