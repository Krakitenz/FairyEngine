#pragma once
#include "core/Shader.h"

namespace fairy::ui {

	class UIQuadRenderer {
	public:
		UIQuadRenderer(int width, int height);
		~UIQuadRenderer();

		void draw(float x, float y, float width, float height,
			float r, float g, float b, float a);

	private:
		int m_width;
		int m_height;
		core::Shader m_shader;
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
	};

}
