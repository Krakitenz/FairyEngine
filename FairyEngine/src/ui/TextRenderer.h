#pragma once
#include "ui/Font.h"
#include "core/Shader.h"
#include <string>

namespace fairy::ui {

	class TextRenderer {
	public:
		TextRenderer(int width, int height);
		~TextRenderer();

		void begin();
		void draw(const Font& font, const std::string& text,
			float x, float y, float r, float g, float b, float a = 1.0f);

	private:
		int m_width;
		int m_height;
		fairy::core::Shader m_shader;
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
	};

}
