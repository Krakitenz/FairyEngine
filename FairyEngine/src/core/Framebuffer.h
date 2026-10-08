#pragma once
#include <glad/gl.h>

namespace fairy::core {

    class Framebuffer {
    public:
        Framebuffer(int width, int height);
        ~Framebuffer();

        void bind() const;
        void unbind() const;

        GLuint colorTexture() const { return m_color; }
        int width()  const { return m_width; }
        int height() const { return m_height; }

    private:
        GLuint m_fbo = 0;
        GLuint m_color = 0;
        GLuint m_depth = 0;
        int m_width, m_height;
    };

}