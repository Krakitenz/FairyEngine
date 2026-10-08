#pragma once
#include <string>

namespace fairy::world {

	struct Location {
		std::string id;
		std::string backgroundTexture = "assets/room2.png";
		float backgroundParallax = 1.0f;
		float backgroundOpacity = 1.0f;
		std::string foregroundTexture;
		float foregroundParallax = 1.0f;
		float foregroundOpacity = 1.0f;

		std::string fogPrefix = "fog";
		int fogFirstFrame = 1;
		int fogFrameCount = 114;
		int fogDigits = 5;
		float fogFrameRate = 30.0f;
		float fogOpacity = 0.35f;

		float playerStartX = 900.0f;
		float playerStartY = 120.0f;

		std::string nextLocation;
		float transitionTrigger = 0.9f;
		float transitionDuration = 0.8f;

		float doorWidth = 180.0f;
		float doorHeight = 260.0f;
		float doorInteractionDistance = 180.0f;
	};

}
