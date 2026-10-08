#pragma once
#include "ui/Button.h"
#include "ui/Font.h"
#include <memory>

namespace fairy::game::ui {

	enum class MainMenuAction {
		None,
		StartGame,
		Exit
	};

	class MainMenu {
	public:
		MainMenu(int width, int height, const char* fontPath);

		MainMenuAction update(float mouseX, float mouseY, bool mousePressed);
		void draw(float mouseX, float mouseY);

	private:
		int m_width;
		int m_height;
		fairy::ui::Font m_font;
		fairy::ui::TextRenderer m_text;
		fairy::ui::UIQuadRenderer m_quads;
		fairy::ui::Button m_startButton;
		fairy::ui::Button m_exitButton;
	};

}
