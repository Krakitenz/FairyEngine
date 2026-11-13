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
		quads.draw(m_x + 8.0f, m_y - 8.0f, m_width, m_height,
			0.0f, 0.0f, 0.0f, 0.28f);
		quads.draw(m_x - 2.0f, m_y - 2.0f, m_width + 4.0f, m_height + 4.0f,
			0.72f, 0.55f, 0.24f, hovered ? 0.95f : 0.62f);
		if (hovered) {
			quads.draw(m_x, m_y, m_width, m_height,
				0.22f, 0.19f, 0.28f, 0.98f);
		}
		else {
			quads.draw(m_x, m_y, m_width, m_height,
				0.07f, 0.08f, 0.14f, 0.96f);
		}

		text.begin();
		const float textWidth = font.textWidth(m_label);
		const float textX = m_x + (m_width - textWidth) * 0.5f;
		text.draw(font, m_label, textX, m_y + 24.0f,
			hovered ? 1.0f : 0.88f,
			hovered ? 0.88f : 0.86f,
			hovered ? 0.45f : 0.78f);
	}

}
