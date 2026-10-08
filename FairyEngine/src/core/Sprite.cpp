#include "core/Sprite.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace fairy::core {

    GLuint Sprite::s_vao = 0;

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

        GLuint vbo;
        glGenVertexArrays(1, &s_vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    Sprite::Sprite(const Texture* texture, float x, float y, float w, float h)
        : m_texture(texture), m_x(x), m_y(y), m_w(w), m_h(h) {
    }

    void Sprite::draw(Shader& shader) const {
        ensureQuad();

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(m_x, m_y, 0.0f));
        model = glm::scale(model, glm::vec3(m_w, m_h, 1.0f));
        shader.setMat4("uModel", glm::value_ptr(model));

        m_texture->bind(0);
        glBindVertexArray(s_vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

}