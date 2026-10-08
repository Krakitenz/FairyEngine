#include "game/ui/MainMenu.h"

namespace fairy::game::ui {

	MainMenu::MainMenu(int width, int height, const char* fontPath)
		: m_width(width)
		, m_height(height)
		, m_font(fontPath, 42.0f)
		, m_text(width, height)
		, m_quads(width, height)
		, m_startButton("START GAME", width * 0.5f - 180.0f,
			height * 0.5f - 20.0f, 360.0f, 72.0f)
		, m_exitButton("EXIT", width * 0.5f - 180.0f,
			height * 0.5f - 120.0f, 360.0f, 72.0f)
	{
	}

	MainMenuAction MainMenu::update(float mouseX, float mouseY,
		bool mousePressed) {
		if (m_startButton.clicked(mouseX, mouseY, mousePressed)) {
			return MainMenuAction::StartGame;
		}
		if (m_exitButton.clicked(mouseX, mouseY, mousePressed)) {
			return MainMenuAction::Exit;
		}
		return MainMenuAction::None;
	}

	void MainMenu::draw(float mouseX, float mouseY) {
		m_quads.draw(0.0f, 0.0f, static_cast<float>(m_width),
			static_cast<float>(m_height),
			0.03f, 0.04f, 0.10f, 0.72f);
		m_text.begin();
		m_text.draw(m_font, "FAIRY ENGINE", 420.0f, 560.0f,
			1.0f, 0.82f, 0.38f);
		m_startButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
		m_exitButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
	}

}
