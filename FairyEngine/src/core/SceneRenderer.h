#pragma once
#include "core/Animation.h"
#include "core/Camera.h"
#include "core/Framebuffer.h"
#include "core/Layer.h"
#include "core/Shader.h"
#include <vector>

namespace fairy::core {

	class SceneRenderer {
	public:
		SceneRenderer(int width, int height, const std::vector<Layer*>& layers,
			Animation& fogAnimation, float fogOpacity);

		void render(const Camera& camera);

		GLuint colorTexture() const { return m_fbo.colorTexture(); }

	private:
		int m_width;
		int m_height;
		Shader m_shader;
		Framebuffer m_fbo;
		const std::vector<Layer*>& m_layers;
		Animation& m_fogAnimation;
		float m_fogOpacity;
	};

}
