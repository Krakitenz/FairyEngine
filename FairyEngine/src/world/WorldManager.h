#pragma once
#include "world/LevelManager.h"
#include <string>
#include <unordered_map>

namespace fairy::world {

	class WorldManager {
	public:
		void registerLocation(const std::string& id, const std::string& path);
		void load(const std::string& id);

		const Location& current() const;
		const std::string& currentId() const { return m_currentId; }
		bool has(const std::string& id) const;

	private:
		LevelManager m_levelManager;
		std::unordered_map<std::string, std::string> m_paths;
		Location m_current;
		std::string m_currentId;
		bool m_hasCurrent = false;
	};

}
