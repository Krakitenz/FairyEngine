#include "ui/Button.h"

namespace fairy::ui {

	Button::Button(std::string label, float x, float y, float width, float height)
		: m_label(std::move(label))
		, m_x(x)
		, m_y(y)
		, m_width(width)
		, m_height(height)
	{
	}

	bool Button::contains(float mouseX, float mouseY) const {
		return mouseX >= m_x && mouseX <= m_x + m_width
			&& mouseY >= m_y && mouseY <= m_y + m_height;
	}

	bool Button::clicked(float mouseX, float mouseY, bool mousePressed) const {
		return mousePressed && contains(mouseX, mouseY);
	}

	void Button::draw(UIQuadRenderer& quads, TextRenderer& text,
		const Font& font, float mouseX, float mouseY) const {
		const bool hovered = contains(mouseX, mouseY);
		if (hovered) {
			quads.draw(m_x, m_y, m_width, m_height,
				0.75f, 0.58f, 0.22f, 0.95f);
		}
		else {
			quads.draw(m_x, m_y, m_width, m_height,
				0.12f, 0.10f, 0.18f, 0.88f);
		}

		text.begin();
		text.draw(font, m_label, m_x + 28.0f, m_y + 22.0f,
			1.0f, 0.94f, 0.78f);
	}

}
