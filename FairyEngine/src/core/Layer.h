#pragma once
#include "core/Camera.h"
#include "core/Sprite.h"

namespace fairy::core {

    // Слой — спрайт с параллаксом и прозрачностью
    struct Layer {
        Sprite sprite;
        float parallax;   // 0.1 = далеко (едва движется), 1.0 = норма, 1.5 = близко
        float opacity;    // 0.0 = прозрачный, 1.0 = непрозрачный

        Layer(const Texture* tex, float x, float y, float w, float h,
            float parallax_ = 1.0f, float opacity_ = 1.0f)
            : sprite(tex, x, y, w, h)
            , parallax(parallax_)
            , opacity(opacity_)
        {
        }

        void draw(Shader& shader, const Camera& camera) const {
            const float screenX = sprite.x() - camera.x() * parallax
                + camera.viewportWidth() * 0.5f;
            const float screenY = sprite.y() - camera.y() * parallax
                + camera.viewportHeight() * 0.5f;

            Sprite screenSprite(sprite.texture(), screenX, screenY,
                sprite.width(), sprite.height());
            shader.setFloat("uOpacity", opacity);
            screenSprite.draw(shader);
        }
    };

} // namespace fairy::core