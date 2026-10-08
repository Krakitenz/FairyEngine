#include "core/Camera.h"
#include <algorithm>
#include <cmath>

namespace fairy::core {

	Camera::Camera(float viewportWidth, float viewportHeight)
		: m_x(viewportWidth * 0.5f)
		, m_y(viewportHeight * 0.5f)
		, m_viewportWidth(viewportWidth)
		, m_viewportHeight(viewportHeight)
	{
	}

	void Camera::setWorldBounds(float worldWidth, float worldHeight) {
		m_worldWidth = worldWidth;
		m_worldHeight = worldHeight;
		m_x = clampX(m_x);
		m_y = clampY(m_y);
	}

	void Camera::setPosition(float x, float y) {
		m_x = clampX(x);
		m_y = clampY(y);
	}

	void Camera::follow(float targetX, float targetY, float dt) {
		const float factor = 1.0f - std::exp(-m_smoothing * std::max(dt, 0.0f));
		m_x += (clampX(targetX) - m_x) * factor;
		m_y += (clampY(targetY) - m_y) * factor;
		m_x = clampX(m_x);
		m_y = clampY(m_y);
	}

	float Camera::clampX(float value) const {
		const float halfWidth = m_viewportWidth * 0.5f;
		if (m_worldWidth <= m_viewportWidth) {
			return m_worldWidth * 0.5f;
		}
		return std::clamp(value, halfWidth, m_worldWidth - halfWidth);
	}

	float Camera::clampY(float value) const {
		const float halfHeight = m_viewportHeight * 0.5f;
		if (m_worldHeight <= m_viewportHeight) {
			return m_worldHeight * 0.5f;
		}
		return std::clamp(value, halfHeight, m_worldHeight - halfHeight);
	}

}
