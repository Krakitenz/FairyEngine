#pragma once
#include <glad/gl.h>
#include "core/Texture.h"
#include "core/Shader.h"

namespace fairy::core {

    class Sprite {
    public:
        Sprite(const Texture* texture, float x, float y, float w, float h);

        void draw(Shader& shader) const;

        void setPosition(float x, float y) { m_x = x; m_y = y; }
        void setSize(float w, float h) { m_w = w; m_h = h; }

        float x()      const { return m_x; }
        float y()      const { return m_y; }
        float width()  const { return m_w; }
        float height() const { return m_h; }

        const Texture* texture() const { return m_texture; }

    private:
        const Texture* m_texture;
        float m_x, m_y, m_w, m_h;

        static GLuint s_vao;
        static void ensureQuad();
    };

}