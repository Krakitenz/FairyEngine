#include "world/LevelManager.h"
#include "world/WorldManager.h"
#include "core/Camera.h"
#include <stb_image.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace {

	using fairy::world::LevelManager;
	using fairy::world::Location;
	using fairy::world::WorldManager;

	class TestFailure final : public std::runtime_error {
	public:
		explicit TestFailure(const std::string& message)
			: std::runtime_error(message) {
		}
	};

	void require(bool condition, const std::string& message) {
		if (!condition) {
			throw TestFailure(message);
		}
	}

	void requireNear(float actual, float expected, float tolerance = 0.001f) {
		if (!std::isfinite(actual) || std::fabs(actual - expected) > tolerance) {
			throw TestFailure("Expected " + std::to_string(expected)
				+ ", got " + std::to_string(actual));
		}
	}

	template<typename Exception, typename Action>
	std::string expectThrows(Action&& action) {
		try {
			action();
		}
		catch (const Exception& error) {
			return error.what();
		}
		catch (const std::exception& error) {
			throw TestFailure("Unexpected exception type: " + std::string(error.what()));
		}

		throw TestFailure("Expected exception was not thrown");
	}

	class TemporaryDirectory {
	public:
		TemporaryDirectory() {
			const auto temporaryRoot = std::filesystem::temp_directory_path();
			for (std::size_t suffix = 0;; ++suffix) {
				m_path = temporaryRoot / ("FairyEngine.Tests." + std::to_string(suffix));
				std::error_code error;
				if (std::filesystem::create_directory(m_path, error)) {
					return;
				}
				if (error && error != std::errc::file_exists) {
					throw TestFailure("Could not create temporary directory '"
						+ m_path.string() + "': " + error.message());
				}
			}
		}

		~TemporaryDirectory() {
			std::error_code error;
			std::filesystem::remove_all(m_path, error);
		}

		std::filesystem::path write(const std::string& name,
			const std::string& contents) const {
			const std::filesystem::path path = m_path / name;
			std::ofstream file(path, std::ios::binary);
			if (!file) {
				throw TestFailure("Could not create temporary location: " + path.string());
			}
			file << contents;
			if (!file) {
				throw TestFailure("Could not write temporary location: " + path.string());
			}
			return path;
		}

	private:
		std::filesystem::path m_path;
	};

	void testCameraRejectsInvalidViewport() {
		expectThrows<std::invalid_argument>([] {
			fairy::core::Camera camera(0.0f, 100.0f);
		});
		expectThrows<std::invalid_argument>([] {
			fairy::core::Camera camera(100.0f, -1.0f);
		});
	}

	void testCameraRejectsInvalidWorldBounds() {
		fairy::core::Camera camera(100.0f, 80.0f);
		expectThrows<std::invalid_argument>([&] {
			camera.setWorldBounds(0.0f, 100.0f);
		});
		expectThrows<std::invalid_argument>([&] {
			camera.setWorldBounds(100.0f, -1.0f);
		});
	}

	void testCameraClampsPositionToWorldEdges() {
		fairy::core::Camera camera(100.0f, 80.0f);
		camera.setWorldBounds(500.0f, 400.0f);

		camera.setPosition(-100.0f, -100.0f);
		requireNear(camera.x(), 50.0f);
		requireNear(camera.y(), 40.0f);

		camera.setPosition(900.0f, 900.0f);
		requireNear(camera.x(), 450.0f);
		requireNear(camera.y(), 360.0f);
	}

	void testCameraCentersWhenWorldIsSmallerThanViewport() {
		fairy::core::Camera camera(100.0f, 80.0f);
		camera.setWorldBounds(60.0f, 40.0f);
		camera.setPosition(0.0f, 0.0f);
		requireNear(camera.x(), 30.0f);
		requireNear(camera.y(), 20.0f);
	}

	void testCameraReclampsAfterViewportResize() {
		fairy::core::Camera camera(100.0f, 80.0f);
		camera.setWorldBounds(1000.0f, 800.0f);
		camera.setPosition(950.0f, 760.0f);
		camera.setViewportSize(200.0f, 160.0f);

		requireNear(camera.viewportWidth(), 200.0f);
		requireNear(camera.viewportHeight(), 160.0f);
		requireNear(camera.x(), 900.0f);
		requireNear(camera.y(), 720.0f);
	}

	void testCameraFollowMovesSmoothlyAndIgnoresNegativeDelta() {
		fairy::core::Camera camera(100.0f, 100.0f);
		camera.setWorldBounds(1000.0f, 1000.0f);
		camera.setPosition(100.0f, 100.0f);
		camera.setSmoothing(4.0f);

		camera.follow(200.0f, 200.0f, 0.25f);
		const float expectedPosition = 100.0f + 100.0f * (1.0f - std::exp(-4.0f * 0.25f));
		requireNear(camera.x(), expectedPosition);
		requireNear(camera.y(), expectedPosition);

		const float beforeX = camera.x();
		const float beforeY = camera.y();
		camera.follow(300.0f, 300.0f, -1.0f);
		requireNear(camera.x(), beforeX);
		requireNear(camera.y(), beforeY);
	}

	void testCameraFollowNeverLeavesWorldBounds() {
		fairy::core::Camera camera(100.0f, 100.0f);
		camera.setWorldBounds(500.0f, 400.0f);
		camera.follow(10000.0f, 10000.0f, 10.0f);
		requireNear(camera.x(), 450.0f);
		requireNear(camera.y(), 350.0f);
	}

	Location loadValidLocation() {
		TemporaryDirectory temporaryDirectory;
		const auto path = temporaryDirectory.write("valid.location",
			"# scene fixture\n"
			"\n"
			"id = memory\n"
			"background.texture = assets/1.png\n"
			"background.parallax = 0.25\n"
			"background.opacity = 0.9\n"
			"foreground.texture = assets/2.png\n"
			"foreground.parallax = 1.25\n"
			"foreground.opacity = 0.75\n"
			"fog.prefix = mist\n"
			"fog.firstFrame = 3\n"
			"fog.frameCount = 8\n"
			"fog.digits = 4\n"
			"fog.frameRate = 24.0\n"
			"fog.opacity = 0.25\n"
			"player.startX = 512.5\n"
			"player.startY = 456.25\n"
			"transition.nextLocation = next\n"
			"transition.trigger = 0.85\n"
			"transition.duration = 0.5\n"
			"door.width = 90\n"
			"door.height = 140\n"
			"door.interactionDistance = 75\n");

		return LevelManager{}.load(path.string());
	}

	void testLocationParserReadsLayerSettings() {
		const Location location = loadValidLocation();
		require(location.id == "memory", "The location ID was not loaded");
		require(location.backgroundTexture == "assets/1.png",
			"The background texture was not loaded");
		require(location.backgroundParallax == 0.25f,
			"The background parallax was not loaded");
		require(location.backgroundOpacity == 0.9f,
			"The background opacity was not loaded");
		require(location.foregroundTexture == "assets/2.png",
			"The foreground texture was not loaded");
		require(location.foregroundParallax == 1.25f,
			"The foreground parallax was not loaded");
		require(location.foregroundOpacity == 0.75f,
			"The foreground opacity was not loaded");
	}

	void testLocationParserReadsFogAndPlayerSettings() {
		const Location location = loadValidLocation();
		require(location.fogPrefix == "mist" && location.fogFirstFrame == 3
			&& location.fogFrameCount == 8 && location.fogDigits == 4
			&& location.fogFrameRate == 24.0f && location.fogOpacity == 0.25f,
			"The fog animation settings were not loaded");
		require(location.playerStartX == 512.5f && location.playerStartY == 456.25f,
			"The player start position was not loaded");
	}

	void testLocationParserReadsTransitionAndDoorSettings() {
		const Location location = loadValidLocation();
		require(location.nextLocation == "next" && location.transitionTrigger == 0.85f
			&& location.transitionDuration == 0.5f,
			"The transition settings were not loaded");
		require(location.doorWidth == 90.0f && location.doorHeight == 140.0f
			&& location.doorInteractionDistance == 75.0f,
			"The door settings were not loaded");
	}

	void testLocationParserRejectsMissingFile() {
		TemporaryDirectory temporaryDirectory;
		const auto missingPath = temporaryDirectory.write("removed.location", "id = removed\n");
		std::filesystem::remove(missingPath);
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(missingPath.string());
		});
		require(message.find("Cannot open location:") != std::string::npos,
			"A missing location should report an open failure");
	}

	void testLocationParserReportsMalformedLines() {
		TemporaryDirectory temporaryDirectory;
		const auto path = temporaryDirectory.write("malformed.location",
			"id = malformed\n"
			"background.texture assets/1.png\n");

		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(path.string());
		});
		require(message.find("malformed.location:2:") != std::string::npos,
			"The parser error does not identify the source line");
	}

	void testLocationParserRejectsUnknownKeys() {
		TemporaryDirectory temporaryDirectory;
		const auto unknownKey = temporaryDirectory.write("unknown.location",
			"id = unknown\n"
			"scene.notASetting = value\n");
		const std::string unknownMessage = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(unknownKey.string());
		});
		require(unknownMessage.find("Unknown location key: scene.notASetting")
			!= std::string::npos, "Unknown location keys should be reported");
	}

	void testLocationParserRequiresAnId() {
		TemporaryDirectory temporaryDirectory;
		const auto missingId = temporaryDirectory.write("missing-id.location",
			"background.texture = assets/1.png\n");
		const std::string missingMessage = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(missingId.string());
		});
		require(missingMessage.find("missing required key 'id'") != std::string::npos,
			"A location without an ID should be rejected");
	}

	void testLocationParserRejectsTrailingCharactersInNumbers() {
		TemporaryDirectory temporaryDirectory;
		const auto trailingCharacters = temporaryDirectory.write("trailing-number.location",
			"id = invalid\n"
			"background.parallax = 0.25oops\n");
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(trailingCharacters.string());
		});
		require(message.find("Invalid number for location key 'background.parallax': 0.25oops")
			!= std::string::npos, "Invalid numeric input should identify the key and value");
	}

	void testLocationParserRejectsNonFiniteNumbers() {
		TemporaryDirectory temporaryDirectory;
		const auto nonFinite = temporaryDirectory.write("non-finite.location",
			"id = invalid\n"
			"fog.frameRate = nan\n");
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(nonFinite.string());
		});
		require(message.find("Invalid number for location key 'fog.frameRate': nan")
			!= std::string::npos, "Non-finite numeric input should identify the key and value");
	}

	void testLocationParserRejectsInvalidOpacity() {
		TemporaryDirectory temporaryDirectory;
		const auto invalidOpacity = temporaryDirectory.write("invalid-opacity.location",
			"id = invalid\n"
			"background.opacity = 1.01\n");
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(invalidOpacity.string());
		});
		require(message.find("opacity must be between 0 and 1") != std::string::npos,
			"Out-of-range opacity should report its validation rule");
	}

	void testLocationParserRejectsInvalidFogSettings() {
		TemporaryDirectory temporaryDirectory;
		const auto invalidFog = temporaryDirectory.write("invalid-fog.location",
			"id = invalid\n"
			"fog.frameCount = 2\n"
			"fog.frameRate = 0\n");
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(invalidFog.string());
		});
		require(message.find("invalid fog animation settings") != std::string::npos,
			"Invalid fog settings should report the failed validation");
	}

	void testLocationParserRejectsInvalidDoorDimensions() {
		TemporaryDirectory temporaryDirectory;
		const auto invalidDoor = temporaryDirectory.write("invalid-door.location",
			"id = invalid\n"
			"door.width = 0\n");
		const std::string message = expectThrows<std::runtime_error>([&] {
			LevelManager{}.load(invalidDoor.string());
		});
		require(message.find("invalid door settings") != std::string::npos,
			"Invalid door dimensions should report the failed validation");
	}

	void testWorldManagerSwitchesBetweenProjectLocations() {
		WorldManager world;
		world.registerLocation("intro", "assets/locations/intro.location");
		world.registerLocation("test2", "assets/locations/test2.location");

		world.load("intro");
		require(world.currentId() == "intro" && world.current().id == "intro",
			"The intro location was not activated");
		require(world.current().nextLocation == "test2",
			"The intro scene does not reference the target location");

		world.load(world.current().nextLocation);
		require(world.currentId() == "test2" && world.current().id == "test2",
			"The WorldManager did not switch to the registered target location");
	}

	void testWorldManagerPreservesCurrentLocationAfterFailedLoad() {
		TemporaryDirectory temporaryDirectory;
		const auto validLocation = temporaryDirectory.write("valid.location",
			"id = original\n"
			"background.texture = assets/1.png\n");
		const auto invalidLocation = temporaryDirectory.write("invalid.location",
			"id = invalid\n"
			"unknown.key = value\n");

		WorldManager world;
		world.registerLocation("original", validLocation.string());
		world.registerLocation("invalid", invalidLocation.string());
		world.load("original");

		const std::string message = expectThrows<std::runtime_error>([&] {
			world.load("invalid");
		});
		require(message.find("Unknown location key: unknown.key") != std::string::npos,
			"The failed location load should report its configuration error");
		require(world.currentId() == "original" && world.current().id == "original",
			"A failed location load replaced the active location");
	}

	void testWorldManagerReportsRegisteredLocations() {
		WorldManager world;
		require(!world.has("intro"), "An unregistered location should not be found");
		world.registerLocation("intro", "assets/locations/intro.location");
		require(world.has("intro"), "A registered location should be found");
	}

	void testWorldManagerRequiresALoadedLocation() {
		const WorldManager world;
		const std::string message = expectThrows<std::logic_error>([&] {
			world.current();
		});
		require(message.find("No location is loaded") != std::string::npos,
			"Accessing the current location should explain that none is loaded");
	}

	void testWorldManagerRejectsUnknownLocations() {
		WorldManager world;
		const std::string message = expectThrows<std::runtime_error>([&] {
			world.load("unregistered");
		});
		require(message.find("Location is not registered: unregistered")
			!= std::string::npos, "An unknown location should be identified");
	}

	void testWorldManagerRejectsInvalidRegistrations() {
		WorldManager world;
		expectThrows<std::invalid_argument>([&] {
			world.registerLocation("", "assets/locations/intro.location");
		});
		expectThrows<std::invalid_argument>([&] {
			world.registerLocation("intro", "");
		});

		world.registerLocation("intro", "assets/locations/intro.location");
		expectThrows<std::invalid_argument>([&] {
			world.registerLocation("intro", "assets/locations/test2.location");
		});
	}

	void testWorldManagerUsesRegisteredIdForLoadedLocation() {
		TemporaryDirectory temporaryDirectory;
		const auto locationPath = temporaryDirectory.write("chapter.location",
			"id = id-from-file\n"
			"background.texture = assets/1.png\n"
			"player.startX = 321\n");

		WorldManager world;
		world.registerLocation("chapter-one", locationPath.string());
		world.load("chapter-one");

		require(world.currentId() == "chapter-one"
			&& world.current().id == "chapter-one",
			"The registered location ID should override the file ID");
		require(world.current().backgroundTexture == "assets/1.png"
			&& world.current().playerStartX == 321.0f,
			"The loaded location contents were not preserved");
	}

	struct ImageDimensions {
		int width;
		int height;
	};

	ImageDimensions decodeImage(const std::string& path) {
		int width = 0;
		int height = 0;
		using Image = std::unique_ptr<stbi_uc, decltype(&stbi_image_free)>;
		Image image(stbi_load(path.c_str(), &width, &height, nullptr, STBI_rgb_alpha),
			stbi_image_free);
		if (!image) {
			const char* reason = stbi_failure_reason();
			throw TestFailure("Could not decode image '" + path + "': "
				+ (reason ? reason : "unknown image error"));
		}
		require(width > 0 && height > 0,
			"Image has invalid dimensions: " + path);
		return { width, height };
	}

	ImageDimensions validateImage(const std::string& path,
		std::map<std::string, ImageDimensions>& decodedImages) {
		const auto existing = decodedImages.find(path);
		if (existing != decodedImages.end()) {
			return existing->second;
		}
		require(std::filesystem::is_regular_file(path),
			"Referenced image is missing: " + path);
		const ImageDimensions dimensions = decodeImage(path);
		decodedImages.emplace(path, dimensions);
		return dimensions;
	}

	std::vector<std::pair<std::filesystem::path, Location>> loadProjectLocations() {
		const std::filesystem::path locationsDirectory = "assets/locations";
		require(std::filesystem::is_directory(locationsDirectory),
			"The project locations directory is missing");

		LevelManager levelManager;
		std::vector<std::pair<std::filesystem::path, Location>> locations;

		for (const auto& entry : std::filesystem::directory_iterator(locationsDirectory)) {
			if (!entry.is_regular_file() || entry.path().extension() != ".location") {
				continue;
			}

			locations.emplace_back(entry.path(), levelManager.load(entry.path().string()));
		}

		require(!locations.empty(), "No project location files were found");
		return locations;
	}

	void testProjectScenesAndReferencedImagesLoad() {
		const auto locations = loadProjectLocations();
		std::map<std::string, ImageDimensions> decodedImages;
		std::set<std::string> sceneIds;

		for (const auto& [path, location] : locations) {
			const std::string expectedId = path.stem().string();
			require(location.id == expectedId,
				"Scene ID does not match its file name: " + path.string());
			sceneIds.insert(location.id);

			validateImage(location.backgroundTexture, decodedImages);
			if (!location.foregroundTexture.empty()) {
				validateImage(location.foregroundTexture, decodedImages);
			}
		}

		require(sceneIds.find("intro") != sceneIds.end(),
			"The intro location was not found");
		require(sceneIds.find("test2") != sceneIds.end(),
			"The target location was not found");
	}

	void testFogFramesLoadWithConsistentDimensions() {
		const auto locations = loadProjectLocations();
		std::map<std::string, ImageDimensions> decodedImages;

		for (const auto& locationEntry : locations) {
			const Location& location = locationEntry.second;
			int fogWidth = 0;
			int fogHeight = 0;
			for (int offset = 0; offset < location.fogFrameCount; ++offset) {
				std::ostringstream framePath;
				framePath << "assets/fog/" << location.fogPrefix << '_'
					<< std::setfill('0') << std::setw(location.fogDigits)
					<< location.fogFirstFrame + offset << ".png";
				const ImageDimensions dimensions = validateImage(framePath.str(), decodedImages);
				if (offset == 0) {
					fogWidth = dimensions.width;
					fogHeight = dimensions.height;
				}
				else {
					require(dimensions.width == fogWidth && dimensions.height == fogHeight,
						"Fog animation frames do not have consistent dimensions: "
						+ framePath.str());
				}
			}
		}
	}

	void testShadersAndFontsExist() {
		const std::vector<std::string> resources = {
			"assets/shaders/scene.vert",
			"assets/shaders/scene.frag",
			"assets/shaders/post.vert",
			"assets/shaders/post.frag",
			"assets/shaders/ui_quad.vert",
			"assets/shaders/ui_quad.frag",
			"assets/shaders/ui_text.vert",
			"assets/shaders/ui_text.frag",
			"assets/PoetryVelvetRegularDemo-pgzzv.ttf",
			"assets/PoetryVelvetItalicDemo-DY99x.ttf"
		};

		for (const std::string& resource : resources) {
			require(std::filesystem::is_regular_file(resource),
				"Required shader or font is missing: " + resource);
			require(std::filesystem::file_size(resource) > 0,
				"Required shader or font is empty: " + resource);
		}
	}

}

int main() {
	const std::vector<std::pair<std::string, std::function<void()>>> tests = {
		{ "Camera rejects invalid viewports", testCameraRejectsInvalidViewport },
		{ "Camera rejects invalid world bounds", testCameraRejectsInvalidWorldBounds },
		{ "Camera clamps its position to world edges", testCameraClampsPositionToWorldEdges },
		{ "Camera centers worlds smaller than the viewport", testCameraCentersWhenWorldIsSmallerThanViewport },
		{ "Camera reclamps after viewport resize", testCameraReclampsAfterViewportResize },
		{ "Camera follows smoothly and ignores negative delta", testCameraFollowMovesSmoothlyAndIgnoresNegativeDelta },
		{ "Camera follow remains inside world bounds", testCameraFollowNeverLeavesWorldBounds },
		{ "location parser reads layer settings", testLocationParserReadsLayerSettings },
		{ "location parser reads fog and player settings", testLocationParserReadsFogAndPlayerSettings },
		{ "location parser reads transition and door settings", testLocationParserReadsTransitionAndDoorSettings },
		{ "location parser rejects missing files", testLocationParserRejectsMissingFile },
		{ "location parser reports malformed lines", testLocationParserReportsMalformedLines },
		{ "location parser rejects unknown keys", testLocationParserRejectsUnknownKeys },
		{ "location parser requires an ID", testLocationParserRequiresAnId },
		{ "location parser rejects trailing characters in numbers", testLocationParserRejectsTrailingCharactersInNumbers },
		{ "location parser rejects non-finite numbers", testLocationParserRejectsNonFiniteNumbers },
		{ "location parser rejects invalid opacity", testLocationParserRejectsInvalidOpacity },
		{ "location parser rejects invalid fog settings", testLocationParserRejectsInvalidFogSettings },
		{ "location parser rejects invalid door dimensions", testLocationParserRejectsInvalidDoorDimensions },
		{ "WorldManager switches between project locations", testWorldManagerSwitchesBetweenProjectLocations },
		{ "WorldManager preserves the active scene after a failed load", testWorldManagerPreservesCurrentLocationAfterFailedLoad },
		{ "WorldManager reports registered locations", testWorldManagerReportsRegisteredLocations },
		{ "WorldManager requires a loaded current location", testWorldManagerRequiresALoadedLocation },
		{ "WorldManager rejects unknown locations", testWorldManagerRejectsUnknownLocations },
		{ "WorldManager rejects invalid registrations", testWorldManagerRejectsInvalidRegistrations },
		{ "WorldManager uses the registered ID for a loaded location", testWorldManagerUsesRegisteredIdForLoadedLocation },
		{ "project scenes and referenced images load", testProjectScenesAndReferencedImagesLoad },
		{ "fog frames load with consistent dimensions", testFogFramesLoadWithConsistentDimensions },
		{ "shader and font resources exist", testShadersAndFontsExist }
	};

	std::size_t passed = 0;
	for (const auto& [name, test] : tests) {
		try {
			test();
			++passed;
			std::cout << "[PASS] " << name << '\n';
		}
		catch (const std::exception& error) {
			std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
		}
		catch (...) {
			std::cerr << "[FAIL] " << name << ": unknown error\n";
		}
	}

	std::cout << passed << '/' << tests.size() << " tests passed\n";
	return passed == tests.size() ? 0 : 1;
}
