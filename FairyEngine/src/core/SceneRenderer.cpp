#include "core/SceneRenderer.h"
#include "core/Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fairy::core {

	SceneRenderer::SceneRenderer(int width, int height,
		const std::vector<Layer*>& layers,
		Animation& fogAnimation, float fogOpacity)
		: m_width(width)
		, m_height(height)
		, m_shader("assets/shaders/scene.vert", "assets/shaders/scene.frag", true)
		, m_fbo(width, height)
		, m_layers(layers)
		, m_fogAnimation(fogAnimation)
		, m_fogOpacity(fogOpacity)
	{
	}

	void SceneRenderer::render(const Camera& camera) {
		m_fbo.bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_shader.bind();
		const glm::mat4 projection = glm::ortho(
			0.0f, static_cast<float>(m_width),
			0.0f, static_cast<float>(m_height),
			-1.0f, 1.0f);
		m_shader.setMat4("uProj", glm::value_ptr(projection));

		for (const Layer* layer : m_layers) {
			if (layer) {
				layer->draw(m_shader, camera);
			}
		}

		const Texture* fogTexture = m_fogAnimation.currentFrame();
		Sprite fog(fogTexture, m_width * 0.5f, m_height * 0.5f,
			static_cast<float>(m_width), static_cast<float>(m_height));
		m_shader.setFloat("uOpacity", m_fogOpacity);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		fog.draw(m_shader);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_fbo.unbind();
	}

}
