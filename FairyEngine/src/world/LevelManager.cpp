#include "world/LevelManager.h"
#include <cctype>
#include <cmath>
#include <fstream>
#include <limits>
#include <stdexcept>

namespace fairy::world {

	namespace {
		int toInt(const std::string& value, const std::string& key) {
			std::size_t consumed = 0;
			int result = 0;
			try {
				result = std::stoi(value, &consumed);
			}
			catch (const std::exception&) {
				throw std::runtime_error("Invalid integer for location key '" + key
					+ "': " + value);
			}
			if (consumed != value.size()) {
				throw std::runtime_error("Invalid integer for location key '" + key
					+ "': " + value);
			}
			return result;
		}

		float toFloat(const std::string& value, const std::string& key) {
			std::size_t consumed = 0;
			float result = 0.0f;
			try {
				result = std::stof(value, &consumed);
			}
			catch (const std::exception&) {
				throw std::runtime_error("Invalid number for location key '" + key
					+ "': " + value);
			}
			if (consumed != value.size() || !std::isfinite(result)) {
				throw std::runtime_error("Invalid number for location key '" + key
					+ "': " + value);
			}
			return result;
		}
	}

	Location LevelManager::load(const std::string& path) const {
		std::ifstream file(path);
		if (!file) {
			throw std::runtime_error("Cannot open location: " + path);
		}

		Location location;

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
				throw std::runtime_error(path + ":" + std::to_string(lineNumber)
					+ ": expected key=value, got '" + line + "'");
			}

			const std::string key = trim(line.substr(0, separator));
			const std::string value = trim(line.substr(separator + 1));
			try {
				apply(location, key, value);
			}
			catch (const std::exception& error) {
				throw std::runtime_error(path + ":" + std::to_string(lineNumber)
					+ ": " + error.what());
			}
		}
		if (file.bad()) {
			throw std::runtime_error("Failed while reading location: " + path);
		}

		if (location.id.empty()) {
			throw std::runtime_error(path + ": missing required key 'id'");
		}
		if (location.backgroundTexture.empty()) {
			throw std::runtime_error(path + ": background.texture cannot be empty");
		}
		if (location.backgroundParallax < 0.0f || location.foregroundParallax < 0.0f) {
			throw std::runtime_error(path + ": layer parallax cannot be negative");
		}
		if (location.backgroundOpacity < 0.0f || location.backgroundOpacity > 1.0f
			|| location.foregroundOpacity < 0.0f || location.foregroundOpacity > 1.0f
			|| location.fogOpacity < 0.0f || location.fogOpacity > 1.0f) {
			throw std::runtime_error(path + ": layer and fog opacity must be between 0 and 1");
		}
		if (location.fogFirstFrame < 0 || location.fogFrameCount < 0
			|| location.fogDigits <= 0 || location.fogDigits > 12
			|| location.fogFrameRate < 0.0f
			|| (location.fogFrameCount > 1 && location.fogFrameRate <= 0.0f)) {
			throw std::runtime_error(path + ": invalid fog animation settings");
		}
		if (location.fogFrameCount > 0
			&& location.fogFirstFrame > std::numeric_limits<int>::max()
				- location.fogFrameCount + 1) {
			throw std::runtime_error(path + ": fog frame range is too large");
		}
		if (location.transitionTrigger < 0.0f || location.transitionTrigger > 1.0f
			|| location.transitionDuration < 0.0f) {
			throw std::runtime_error(path + ": invalid transition settings");
		}
		if (location.doorWidth <= 0.0f || location.doorHeight <= 0.0f
			|| location.doorInteractionDistance < 0.0f) {
			throw std::runtime_error(path + ": invalid door settings");
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
