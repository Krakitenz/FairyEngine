#pragma once
#include "world/Location.h"
#include <string>

namespace fairy::world {

	class LevelManager {
	public:
		Location load(const std::string& path) const;

	private:
		static std::string trim(const std::string& value);
		static void apply(Location& location, const std::string& key,
			const std::string& value);
	};

}
