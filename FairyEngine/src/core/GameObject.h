#pragma once

namespace fairy::core {

	class GameObject {
	public:
		virtual ~GameObject() = default;

		virtual void update(float dt) = 0;
	};

}
