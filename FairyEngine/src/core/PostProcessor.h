#pragma once
#include "core/Shader.h"
#include "config/GameConfig.h"

namespace fairy::core {

	class PostProcessor {
	public:
		PostProcessor(int width, int height,
			const config::PostProcessSettings& settings);
		~PostProcessor();

		void render(GLuint sceneTexture, float focusX, float focusY,
			float fade = 0.0f);

	private:
		int m_width;
		int m_height;
		const config::PostProcessSettings& m_settings;
		Shader m_shader;
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
	};

}
