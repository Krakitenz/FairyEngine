#pragma once
#include "core/GameObject.h"
#include <string>

namespace fairy::world {

	class Door : public core::GameObject {
	public:
		Door(float x, float y, float width, float height,
			float interactionDistance, std::string nextLocation);

		void update(float dt) override;
		bool tryInteract(float playerX, float playerY, bool clicked) const;

		float x() const { return m_x; }
		float y() const { return m_y; }
		float width() const { return m_width; }
		float height() const { return m_height; }
		const std::string& nextLocation() const { return m_nextLocation; }

	private:
		float m_x;
		float m_y;
		float m_width;
		float m_height;
		float m_interactionDistance;
		std::string m_nextLocation;
	};

}
