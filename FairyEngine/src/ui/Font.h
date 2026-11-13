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
		float textWidth(const std::string& text) const {
			float width = 0.0f;
			for (const unsigned char character : text) {
				if (character >= 32 && character <= 127) {
					width += m_glyphs[character - 32].xadvance;
				}
			}
			return width;
		}

	private:
		GLuint m_texture = 0;
		float m_pixelHeight;
		std::array<stbtt_bakedchar, 96> m_glyphs;
	};

}
