#include "ui/Font.h"
#include <fstream>
#include <stdexcept>
#include <vector>

namespace fairy::ui {

	Font::Font(const std::string& path, float pixelHeight)
		: m_pixelHeight(pixelHeight)
	{
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if (!file) {
			throw std::runtime_error("Cannot open font: " + path);
		}

		const std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		std::vector<unsigned char> data(static_cast<size_t>(size));
		if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
			throw std::runtime_error("Cannot read font: " + path);
		}

		std::vector<unsigned char> bitmap(512 * 512);
		if (stbtt_BakeFontBitmap(data.data(), 0, pixelHeight,
			bitmap.data(), 512, 512, 32, 96, m_glyphs.data()) <= 0) {
			throw std::runtime_error("Cannot bake font: " + path);
		}

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0,
			GL_RED, GL_UNSIGNED_BYTE, bitmap.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Font::~Font() {
		if (m_texture) {
			glDeleteTextures(1, &m_texture);
		}
	}

}
