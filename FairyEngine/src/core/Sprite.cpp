#include "core/Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

namespace fairy::core {

    GLuint Sprite::s_vao = 0;
    GLuint Sprite::s_vbo = 0;

    void Sprite::ensureQuad() {
        if (s_vao) return;

        float verts[] = {
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.0f, 1.0f,
        };

        glGenVertexArrays(1, &s_vao);
        glGenBuffers(1, &s_vbo);
        if (!s_vao || !s_vbo) {
            release();
            throw std::runtime_error("Could not allocate sprite geometry");
        }

        glBindVertexArray(s_vao);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
    }

    Sprite::Sprite(const Texture* texture, float x, float y, float w, float h)
        : m_texture(texture), m_x(x), m_y(y), m_w(w), m_h(h) {
        if (!m_texture) {
            throw std::invalid_argument("Sprite texture cannot be null");
        }
        if (w <= 0.0f || h <= 0.0f) {
            throw std::invalid_argument("Sprite dimensions must be positive");
        }
    }

    void Sprite::draw(Shader& shader) const {
        ensureQuad();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_x, m_y, 0.0f));
        model = glm::scale(model, glm::vec3(m_w, m_h, 1.0f));
        shader.setMat4("uModel", glm::value_ptr(model));

        m_texture->bind(0);
        glBindVertexArray(s_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    void Sprite::release() {
        if (s_vbo) {
            glDeleteBuffers(1, &s_vbo);
            s_vbo = 0;
        }
        if (s_vao) {
            glDeleteVertexArrays(1, &s_vao);
            s_vao = 0;
        }
    }

}