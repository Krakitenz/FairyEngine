#include "world/LevelManager.h"
#include <cctype>
#include <fstream>
#include <stdexcept>

namespace fairy::world {

	namespace {
		int toInt(const std::string& value, const std::string& key) {
			try {
				return std::stoi(value);
			}
			catch (...) {
				throw std::runtime_error("Invalid integer for location key: " + key);
			}
		}

		float toFloat(const std::string& value, const std::string& key) {
			try {
				return std::stof(value);
			}
			catch (...) {
				throw std::runtime_error("Invalid number for location key: " + key);
			}
		}
	}

	Location LevelManager::load(const std::string& path) const {
		std::ifstream file(path);
		if (!file) {
			throw std::runtime_error("Cannot open location: " + path);
		}

		Location location;
		location.id = path;

		std::string line;
		int lineNumber = 0;
		while (std::getline(file, line)) {
			++lineNumber;
			line = trim(line);
			if (line.empty() || line.front() == '#') {
				continue;
			}

			const std::size_t separator = line.find('=');
			if (separator == std::string::npos) {
				throw std::runtime_error("Invalid location line "
					+ std::to_string(lineNumber) + ": " + line);
			}

			const std::string key = trim(line.substr(0, separator));
			const std::string value = trim(line.substr(separator + 1));
			apply(location, key, value);
		}

		return location;
	}

	std::string LevelManager::trim(const std::string& value) {
		std::size_t first = 0;
		while (first < value.size()
			&& std::isspace(static_cast<unsigned char>(value[first]))) {
			++first;
		}

		std::size_t last = value.size();
		while (last > first
			&& std::isspace(static_cast<unsigned char>(value[last - 1]))) {
			--last;
		}
		return value.substr(first, last - first);
	}

	void LevelManager::apply(Location& location, const std::string& key,
		const std::string& value) {
		if (key == "id") location.id = value;
		else if (key == "background.texture") location.backgroundTexture = value;
		else if (key == "background.parallax") location.backgroundParallax = toFloat(value, key);
		else if (key == "background.opacity") location.backgroundOpacity = toFloat(value, key);
		else if (key == "foreground.texture") location.foregroundTexture = value;
		else if (key == "foreground.parallax") location.foregroundParallax = toFloat(value, key);
		else if (key == "foreground.opacity") location.foregroundOpacity = toFloat(value, key);
		else if (key == "fog.prefix") location.fogPrefix = value;
		else if (key == "fog.firstFrame") location.fogFirstFrame = toInt(value, key);
		else if (key == "fog.frameCount") location.fogFrameCount = toInt(value, key);
		else if (key == "fog.digits") location.fogDigits = toInt(value, key);
		else if (key == "fog.frameRate") location.fogFrameRate = toFloat(value, key);
		else if (key == "fog.opacity") location.fogOpacity = toFloat(value, key);
		else if (key == "player.startX") location.playerStartX = toFloat(value, key);
		else if (key == "player.startY") location.playerStartY = toFloat(value, key);
		else if (key == "transition.nextLocation") location.nextLocation = value;
		else if (key == "transition.trigger") location.transitionTrigger = toFloat(value, key);
		else if (key == "transition.duration") location.transitionDuration = toFloat(value, key);
		else if (key == "door.width") location.doorWidth = toFloat(value, key);
		else if (key == "door.height") location.doorHeight = toFloat(value, key);
		else if (key == "door.interactionDistance") location.doorInteractionDistance = toFloat(value, key);
		else throw std::runtime_error("Unknown location key: " + key);
	}

}
