#include "core/Framebuffer.h"
#include <stdexcept>

namespace fairy::core {

    Framebuffer::Framebuffer(int width, int height)
    {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Framebuffer dimensions must be positive");
        }

        glGenFramebuffers(1, &m_fbo);
        glGenTextures(1, &m_color);
        glGenRenderbuffers(1, &m_depth);

        if (!m_fbo || !m_color || !m_depth) {
            release();
            throw std::runtime_error("Could not allocate framebuffer resources");
        }

        try {
            resize(width, height);
        }
        catch (...) {
            release();
            throw;
        }
    }

    Framebuffer::~Framebuffer() {
        release();
    }

    void Framebuffer::release() {
        if (m_depth) {
            glDeleteRenderbuffers(1, &m_depth);
            m_depth = 0;
        }
        if (m_color) {
            glDeleteTextures(1, &m_color);
            m_color = 0;
        }
        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
    }

    void Framebuffer::resize(int width, int height) {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Framebuffer dimensions must be positive");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glBindTexture(GL_TEXTURE_2D, m_color);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, m_color, 0);

        glBindRenderbuffer(GL_RENDERBUFFER, m_depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
            GL_RENDERBUFFER, m_depth);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            throw std::runtime_error("Framebuffer is not complete after resize");
        }

        m_width = width;
        m_height = height;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::bind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
    }

    void Framebuffer::unbind() const {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}