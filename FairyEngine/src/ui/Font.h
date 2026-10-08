#pragma once
#include <glad/gl.h>
#include <stb_truetype.h>
#include <array>
#include <string>

namespace fairy::ui {

	class Font {
	public:
		Font(const std::string& path, float pixelHeight);
		~Font();

		GLuint texture() const { return m_texture; }
		float pixelHeight() const { return m_pixelHeight; }
		const stbtt_bakedchar* glyphs() const { return m_glyphs.data(); }

	private:
		GLuint m_texture = 0;
		float m_pixelHeight;
		std::array<stbtt_bakedchar, 96> m_glyphs;
	};

}
