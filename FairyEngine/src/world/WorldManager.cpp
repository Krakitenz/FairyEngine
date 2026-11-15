#include "world/WorldManager.h"
#include <stdexcept>

namespace fairy::world {

	void WorldManager::registerLocation(const std::string& id,
		const std::string& path) {
		if (id.empty()) {
			throw std::invalid_argument("Location id cannot be empty");
		}
		if (path.empty()) {
			throw std::invalid_argument("Location path cannot be empty");
		}
		if (!m_paths.emplace(id, path).second) {
			throw std::invalid_argument("Location is already registered: " + id);
		}
	}

	void WorldManager::load(const std::string& id) {
		const auto it = m_paths.find(id);
		if (it == m_paths.end()) {
			throw std::runtime_error("Location is not registered: " + id);
		}

		Location loaded = m_levelManager.load(it->second);
		loaded.id = id;
		m_current = std::move(loaded);
		m_currentId = id;
		m_hasCurrent = true;
	}

	const Location& WorldManager::current() const {
		if (!m_hasCurrent) {
			throw std::logic_error("No location is loaded");
		}
		return m_current;
	}

	bool WorldManager::has(const std::string& id) const {
		return m_paths.find(id) != m_paths.end();
	}

}
