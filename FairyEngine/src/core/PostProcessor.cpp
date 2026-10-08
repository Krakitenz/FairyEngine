#include "core/PostProcessor.h"

namespace fairy::core {

	PostProcessor::PostProcessor(int width, int height,
		const config::PostProcessSettings& settings)
		: m_width(width)
		, m_height(height)
		, m_settings(settings)
		, m_shader("assets/shaders/post.vert", "assets/shaders/post.frag", true)
	{
		const float vertices[] = {
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f,  1.0f,  1.0f, 1.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
		};

		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	PostProcessor::~PostProcessor() {
		if (m_vbo) glDeleteBuffers(1, &m_vbo);
		if (m_vao) glDeleteVertexArrays(1, &m_vao);
	}

	void PostProcessor::render(GLuint sceneTexture, float focusX, float focusY,
		float fade) {
		glViewport(0, 0, m_width, m_height);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader.bind();
		m_shader.setVec2("uFocusPoint", focusX / m_width, focusY / m_height);
		m_shader.setVec2("uResolution", static_cast<float>(m_width),
			static_cast<float>(m_height));
		m_shader.setFloat("uDofStrength", m_settings.dofStrength);
		m_shader.setFloat("uSaturation", m_settings.saturation);
		m_shader.setFloat("uBrightness", m_settings.brightness);
		m_shader.setFloat("uContrast", m_settings.contrast);
		m_shader.setVec3("uTint", m_settings.tintR, m_settings.tintG,
			m_settings.tintB);
		m_shader.setFloat("uVignette", m_settings.vignette);
		m_shader.setFloat("uFade", fade);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneTexture);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

}
