#include "core/Window.h"
#include <cstdio>
#include <stdexcept>

namespace fairy::core {

    static void glfwErrorCallback(int code, const char* desc) {
        std::fprintf(stderr, "[GLFW] %d: %s\n", code, desc ? desc : "Unknown error");
    }

    Window::Window(int width, int height, const std::string& title)
        : m_width(width), m_height(height)
    {
        if (width <= 0 || height <= 0) {
            throw std::invalid_argument("Window dimensions must be positive");
        }

        glfwSetErrorCallback(glfwErrorCallback);
        if (!glfwInit())
            throw std::runtime_error("glfwInit failed");
        m_glfwInitialized = true;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        m_handle = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        if (!m_handle) {
            glfwTerminate();
            m_glfwInitialized = false;
            throw std::runtime_error("glfwCreateWindow failed");
        }

        glfwMakeContextCurrent(m_handle);
        glfwSwapInterval(1);

        if (!gladLoadGL(glfwGetProcAddress)) {
            glfwDestroyWindow(m_handle);
            m_handle = nullptr;
            glfwTerminate();
            m_glfwInitialized = false;
            throw std::runtime_error("gladLoadGL failed");
        }

        glfwSetWindowUserPointer(m_handle, this);
        glfwSetFramebufferSizeCallback(m_handle, framebufferSizeCallback);
        glfwGetFramebufferSize(m_handle, &m_width, &m_height);
        glViewport(0, 0, m_width, m_height);
    }

    Window::~Window() {
        if (m_handle) {
            glfwSetWindowUserPointer(m_handle, nullptr);
            glfwDestroyWindow(m_handle);
        }
        if (m_glfwInitialized) {
            glfwTerminate();
        }
    }

    void Window::framebufferSizeCallback(GLFWwindow* nativeWindow, int width, int height) {
        auto* window = static_cast<Window*>(glfwGetWindowUserPointer(nativeWindow));
        if (!window) {
            return;
        }

        window->m_width = width;
        window->m_height = height;
        if (width > 0 && height > 0) {
            glViewport(0, 0, width, height);
        }
    }

    bool Window::shouldClose() const { return glfwWindowShouldClose(m_handle); }
    void Window::pollEvents() { glfwPollEvents(); }
    void Window::swapBuffers() { glfwSwapBuffers(m_handle); }

    void Window::clear(float r, float g, float b, float a) const {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

} // namespace fairy::core