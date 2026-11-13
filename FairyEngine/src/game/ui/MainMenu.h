#pragma once
#include "ui/Button.h"
#include "ui/Font.h"
#include <memory>

namespace fairy::game::ui {

	enum class MainMenuAction {
		None,
		NewGame,
		ContinueGame,
		OpenSettings,
		Exit
	};

	class MainMenu {
	public:
		MainMenu(int width, int height, const char* fontPath);

		MainMenuAction update(float mouseX, float mouseY, bool mousePressed);
		void draw(float mouseX, float mouseY);
		float volume() const { return m_volume; }

	private:
		enum class Screen {
			Main,
			Settings
		};

		int m_width;
		int m_height;
		Screen m_screen = Screen::Main;
		float m_volume = 1.0f;
		fairy::ui::Font m_font;
		fairy::ui::TextRenderer m_text;
		fairy::ui::UIQuadRenderer m_quads;
		fairy::ui::Button m_newGameButton;
		fairy::ui::Button m_continueButton;
		fairy::ui::Button m_settingsButton;
		fairy::ui::Button m_exitButton;
		fairy::ui::Button m_volumeDownButton;
		fairy::ui::Button m_volumeUpButton;
		fairy::ui::Button m_backButton;
	};

}
