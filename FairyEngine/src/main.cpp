#include "core/Window.h"
#include "core/Shader.h"
#include "core/Texture.h"
#include <glad/gl.h>
#include <cstdio>

using namespace fairy::core;

const char* VERT = R"(
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aUV;
out vec2 vUV;
void main() {
    vUV = aUV;
    gl_Position = vec4(aPos, 0.0, 1.0);
}
)";

const char* FRAG = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;
uniform sampler2D uTex;
void main() {
    FragColor = texture(uTex, vUV);
}
)";

int main()
{
    try {
        Window window(1280, 720, "Fairy Engine");
        Shader shader(VERT, FRAG);
        Texture bg("assets/background.png");

        // Полноэкранный квад: x, y, u, v
        float verts[] = {
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,

            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f,
            -1.0f,  1.0f,  0.0f, 1.0f,
        };

        GLuint vao, vbo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        while (!window.shouldClose())
        {
            window.clear(0.0f, 0.0f, 0.0f, 1.0f);

            shader.bind();
            bg.bind(0);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 6);

            window.swapBuffers();
            window.pollEvents();
        }
    }
    catch (const std::exception& e) {
        std::fprintf(stderr, "Fatal: %s\n", e.what());
        return -1;
    }
    return 0;
}