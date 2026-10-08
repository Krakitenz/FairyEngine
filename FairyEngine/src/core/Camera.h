#pragma once

namespace fairy::core {

	class Camera {
	public:
		Camera(float viewportWidth, float viewportHeight);

		void setWorldBounds(float worldWidth, float worldHeight);
		void setSmoothing(float smoothing) { m_smoothing = smoothing; }
		void setPosition(float x, float y);
		void follow(float targetX, float targetY, float dt);

		float x() const { return m_x; }
		float y() const { return m_y; }
		float viewportWidth() const { return m_viewportWidth; }
		float viewportHeight() const { return m_viewportHeight; }

	private:
		float clampX(float value) const;
		float clampY(float value) const;

		float m_x;
		float m_y;
		float m_viewportWidth;
		float m_viewportHeight;
		float m_worldWidth = 0.0f;
		float m_worldHeight = 0.0f;
		float m_smoothing = 8.0f;
	};

}
