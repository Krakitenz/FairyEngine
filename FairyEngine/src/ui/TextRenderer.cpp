#include "ui/TextRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

namespace fairy::ui {

	TextRenderer::TextRenderer(int width, int height)
		: m_width(width)
		, m_height(height)
		, m_shader("assets/shaders/ui_text.vert", "assets/shaders/ui_text.frag", true)
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
			4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
			4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);
	}

	TextRenderer::~TextRenderer() {
		if (m_vbo) glDeleteBuffers(1, &m_vbo);
		if (m_vao) glDeleteVertexArrays(1, &m_vao);
	}

	void TextRenderer::begin() {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_shader.bind();
		const glm::mat4 projection = glm::ortho(
			0.0f, static_cast<float>(m_width),
			0.0f, static_cast<float>(m_height));
		m_shader.setMat4("uProj", glm::value_ptr(projection));
	}

	void TextRenderer::draw(const Font& font, const std::string& text,
		float x, float y, float r, float g, float b, float a) {
		float cursorX = x;
		float cursorY = y;
		const stbtt_bakedchar* glyphs = font.glyphs();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, font.texture());
		glBindVertexArray(m_vao);
		m_shader.setVec4("uColor", r, g, b, a);

		for (const unsigned char character : text) {
			if (character < 32 || character > 127) continue;
			stbtt_aligned_quad quad;
			stbtt_GetBakedQuad(glyphs, 512, 512,
				character - 32, &cursorX, &cursorY, &quad, 1);

			const float vertices[] = {
				quad.x0, m_height - quad.y1, quad.s0, quad.t1,
				quad.x1, m_height - quad.y1, quad.s1, quad.t1,
				quad.x1, m_height - quad.y0, quad.s1, quad.t0,
				quad.x0, m_height - quad.y1, quad.s0, quad.t1,
				quad.x1, m_height - quad.y0, quad.s1, quad.t0,
				quad.x0, m_height - quad.y0, quad.s0, quad.t0,
			};
			glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
		glBindVertexArray(0);
	}

}
