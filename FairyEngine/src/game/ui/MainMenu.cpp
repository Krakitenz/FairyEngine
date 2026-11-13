#include "game/ui/MainMenu.h"
#include <cstdio>

namespace fairy::game::ui {

	MainMenu::MainMenu(int width, int height, const char* fontPath)
		: m_width(width)
		, m_height(height)
		, m_font(fontPath, 42.0f)
		, m_text(width, height)
		, m_quads(width, height)
		, m_newGameButton("NEW GAME", width * 0.5f - 180.0f,
			height * 0.5f + 70.0f, 360.0f, 72.0f)
		, m_continueButton("CONTINUE", width * 0.5f - 180.0f,
			height * 0.5f - 20.0f, 360.0f, 72.0f)
		, m_settingsButton("SETTINGS", width * 0.5f - 180.0f,
			height * 0.5f - 110.0f, 360.0f, 72.0f)
		, m_exitButton("EXIT", width * 0.5f - 180.0f,
			height * 0.5f - 200.0f, 360.0f, 72.0f)
		, m_volumeDownButton("-", width * 0.5f - 180.0f,
			height * 0.5f - 20.0f, 72.0f, 72.0f)
		, m_volumeUpButton("+", width * 0.5f + 108.0f,
			height * 0.5f - 20.0f, 72.0f, 72.0f)
		, m_backButton("BACK", width * 0.5f - 180.0f,
			height * 0.5f - 140.0f, 360.0f, 72.0f)
	{
	}

	MainMenuAction MainMenu::update(float mouseX, float mouseY,
		bool mousePressed) {
		if (m_screen == Screen::Main) {
			if (m_newGameButton.clicked(mouseX, mouseY, mousePressed)) {
				return MainMenuAction::NewGame;
			}
			if (m_continueButton.clicked(mouseX, mouseY, mousePressed)) {
				return MainMenuAction::ContinueGame;
			}
			if (m_settingsButton.clicked(mouseX, mouseY, mousePressed)) {
				m_screen = Screen::Settings;
				return MainMenuAction::OpenSettings;
			}
			if (m_exitButton.clicked(mouseX, mouseY, mousePressed)) {
				return MainMenuAction::Exit;
			}
		}
		else {
			if (m_volumeDownButton.clicked(mouseX, mouseY, mousePressed)) {
				m_volume -= 0.1f;
			}
			if (m_volumeUpButton.clicked(mouseX, mouseY, mousePressed)) {
				m_volume += 0.1f;
			}
			if (m_backButton.clicked(mouseX, mouseY, mousePressed)) {
				m_screen = Screen::Main;
			}
			if (m_volume < 0.0f) {
				m_volume = 0.0f;
			}
			if (m_volume > 1.0f) {
				m_volume = 1.0f;
			}
		}
		return MainMenuAction::None;
	}

	void MainMenu::draw(float mouseX, float mouseY) {
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_quads.draw(0.0f, 0.0f, static_cast<float>(m_width),
			static_cast<float>(m_height),
			0.015f, 0.02f, 0.06f, 0.84f);

		const float panelWidth = 520.0f;
		const float panelHeight = 620.0f;
		const float panelX = (static_cast<float>(m_width) - panelWidth) * 0.5f;
		const float panelY = 220.0f;
		m_quads.draw(panelX + 14.0f, panelY - 14.0f,
			panelWidth, panelHeight, 0.0f, 0.0f, 0.0f, 0.38f);
		m_quads.draw(panelX, panelY, panelWidth, panelHeight,
			0.045f, 0.055f, 0.105f, 0.94f);
		m_quads.draw(panelX, panelY + panelHeight - 5.0f,
			panelWidth, 5.0f, 0.78f, 0.59f, 0.26f, 0.92f);
		m_quads.draw(panelX + 46.0f, panelY + panelHeight - 82.0f,
			panelWidth - 92.0f, 1.0f, 0.78f, 0.59f, 0.26f, 0.48f);

		m_text.begin();
		m_text.draw(m_font, "FAIRY ENGINE", panelX + 82.0f,
			panelY + panelHeight - 64.0f,
			1.0f, 0.82f, 0.38f);

		if (m_screen == Screen::Main) {
			m_text.draw(m_font, "A QUIET STORY IN THE MIST", panelX + 118.0f,
				panelY + panelHeight - 112.0f,
				0.68f, 0.70f, 0.78f, 0.92f);
			m_newGameButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
			m_continueButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
			m_settingsButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
			m_exitButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
		}
		else {
			m_text.draw(m_font, "SETTINGS", panelX + 135.0f,
				panelY + panelHeight - 112.0f,
				1.0f, 0.82f, 0.38f);
			m_text.draw(m_font, "MASTER VOLUME", panelX + 108.0f,
				panelY + 270.0f,
				1.0f, 0.94f, 0.78f);

			char volumeText[16];
			std::snprintf(volumeText, sizeof(volumeText), "%d%%",
				static_cast<int>(m_volume * 100.0f + 0.5f));
			m_text.draw(m_font, volumeText, panelX + 222.0f,
				panelY + 215.0f,
				1.0f, 0.94f, 0.78f);
			m_volumeDownButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
			m_volumeUpButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
			m_backButton.draw(m_quads, m_text, m_font, mouseX, mouseY);
		}
	}

}
