#pragma once
#include <glad/gl.h>
#include <string>

namespace fairy::core {

    class Texture {
    public:
        Texture(const std::string& path);
        ~Texture();

        void bind(GLuint slot = 0) const;

        int width()  const { return m_width; }
        int height() const { return m_height; }

    private:
        GLuint m_id = 0;
        int m_width = 0;
        int m_height = 0;
    };

}