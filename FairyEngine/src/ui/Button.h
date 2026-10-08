#pragma once
#include "ui/TextRenderer.h"
#include "ui/UIQuadRenderer.h"
#include <string>

namespace fairy::ui {

	class Button {
	public:
		Button(std::string label, float x, float y, float width, float height);

		bool contains(float mouseX, float mouseY) const;
		bool clicked(float mouseX, float mouseY, bool mousePressed) const;
		void draw(UIQuadRenderer& quads, TextRenderer& text,
			const Font& font, float mouseX, float mouseY) const;

		const std::string& label() const { return m_label; }

	private:
		std::string m_label;
		float m_x;
		float m_y;
		float m_width;
		float m_height;
	};

}
