#pragma once

namespace fairy::config {

	struct WindowSettings {
		int width = 1920;
		int height = 1080;
		const char* title = "Fairy Engine";
	};

	struct CameraSettings {
		float smoothing = 8.0f;
		float targetHeight = 0.5f;
	};

	struct PlayerSettings {
		float startX = 900.0f;
		float startY = 120.0f;
	};

	struct FogSettings {
		int firstFrame = 1;
		int frameCount = 114;
		int digits = 5;
		float frameRate = 30.0f;
		float opacity = 0.35f;
	};

	struct LayerSettings {
		float parallax = 1.0f;
		float opacity = 1.0f;
	};

	struct PostProcessSettings {
		float dofStrength = 0.0f;
		float saturation = 0.85f;
		float brightness = 1.02f;
		float contrast = 1.05f;
		float tintR = 0.98f;
		float tintG = 0.97f;
		float tintB = 1.05f;
		float vignette = 1.4f;
		float focusHeightOffset = 150.0f;
	};

	inline constexpr WindowSettings window{};
	inline constexpr CameraSettings camera{};
	inline constexpr PlayerSettings player{};
	inline constexpr FogSettings fog{};
	inline constexpr LayerSettings background{};
	inline constexpr PostProcessSettings postProcess{};

}
