#include "core/Window.h"
#include <cstdio>
#include <stdexcept>

namespace fairy::core {

    static void glfwErrorCallback(int code, const char* desc) {
        std::fprintf(stderr, "[GLFW] %d: %s\n", code, desc);
    }

    Window::Window(int width, int height, const std::string& title)
        : m_width(width), m_height(height)
    {
        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            throw std::runtime_error("glfwInit failed");

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_handle) {
            glfwTerminate();
            throw std::runtime_error("glfwCreateWindow failed");
        }

        glfwMakeContextCurrent(m_handle);
        glfwSwapInterval(1);

        if (!gladLoadGL(glfwGetProcAddress)) {
            glfwDestroyWindow(m_handle);
            glfwTerminate();
            throw std::runtime_error("gladLoadGL failed");
        }

        glViewport(0, 0, width, height);
    }

    Window::~Window() {
        if (m_handle) glfwDestroyWindow(m_handle);
        glfwTerminate();
    }

    bool Window::shouldClose() const { return glfwWindowShouldClose(m_handle); }
    void Window::pollEvents() { glfwPollEvents(); }
    void Window::swapBuffers() { glfwSwapBuffers(m_handle); }

    void Window::clear(float r, float g, float b, float a) const {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

} // namespace fairy::core