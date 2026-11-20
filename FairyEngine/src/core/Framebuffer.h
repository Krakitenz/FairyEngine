#pragma once
#include <glad/gl.h>

namespace fairy::core {

    class Framebuffer {
    public:
        Framebuffer(int width, int height);
        ~Framebuffer();

        Framebuffer(const Framebuffer&) = delete;
        Framebuffer& operator=(const Framebuffer&) = delete;

        void bind() const;
        void unbind() const;
        void resize(int width, int height);

        GLuint colorTexture() const { return m_color; }
        int width()  const { return m_width; }
        int height() const { return m_height; }

    private:
        void release();

        GLuint m_fbo = 0;
        GLuint m_color = 0;
        GLuint m_depth = 0;
        int m_width = 0;
        int m_height = 0;
    };

}