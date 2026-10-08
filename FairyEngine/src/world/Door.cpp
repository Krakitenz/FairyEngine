#include "world/Door.h"
#include <cmath>
#include <utility>

namespace fairy::world {

	Door::Door(float x, float y, float width, float height,
		float interactionDistance, std::string nextLocation)
		: m_x(x)
		, m_y(y)
		, m_width(width)
		, m_height(height)
		, m_interactionDistance(interactionDistance)
		, m_nextLocation(std::move(nextLocation))
	{
	}

	void Door::update(float /*dt*/) {
	}

	bool Door::tryInteract(float playerX, float playerY, bool clicked) const {
		if (!clicked || m_nextLocation.empty()) {
			return false;
		}

		const bool insideHorizontalRange = std::abs(playerX - m_x)
			<= m_interactionDistance + m_width * 0.5f;
		const bool insideVerticalRange = std::abs(playerY - m_y)
			<= m_height * 0.5f;
		return insideHorizontalRange && insideVerticalRange;
	}

}
