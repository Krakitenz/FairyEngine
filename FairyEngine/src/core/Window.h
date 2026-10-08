#pragma once
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <string>

namespace fairy::core {

    class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool shouldClose() const;
        void pollEvents();
        void swapBuffers();
        void clear(float r, float g, float b, float a) const;

        GLFWwindow* handle() const { return m_handle; }
        int width()  const { return m_width; }
        int height() const { return m_height; }

    private:
        GLFWwindow* m_handle = nullptr;
        int m_width;
        int m_height;
    };

} // namespace fairy::core