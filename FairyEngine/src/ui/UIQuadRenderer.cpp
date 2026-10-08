#include "ui/UIQuadRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fairy::ui {

	UIQuadRenderer::UIQuadRenderer(int width, int height)
		: m_width(width)
		, m_height(height)
		, m_shader("assets/shaders/ui_quad.vert", "assets/shaders/ui_quad.frag", true)
	{
		const float vertices[] = {
			0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
			0.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f,
		};
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}

	UIQuadRenderer::~UIQuadRenderer() {
		if (m_vbo) glDeleteBuffers(1, &m_vbo);
		if (m_vao) glDeleteVertexArrays(1, &m_vao);
	}

	void UIQuadRenderer::draw(float x, float y, float width, float height,
		float r, float g, float b, float a) {
		m_shader.bind();
		const glm::mat4 projection = glm::ortho(
			0.0f, static_cast<float>(m_width),
			0.0f, static_cast<float>(m_height));
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
		model = glm::scale(model, glm::vec3(width, height, 1.0f));
		m_shader.setMat4("uProj", glm::value_ptr(projection));
		m_shader.setMat4("uModel", glm::value_ptr(model));
		m_shader.setVec4("uColor", r, g, b, a);
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

}
