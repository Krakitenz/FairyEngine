#pragma once
#include <glad/gl.h>
#include <string>
#include <unordered_map>

namespace fairy::core {

    class Shader {
    public:
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        Shader(const std::string& vertPath, const std::string& fragPath, bool fromFile);

        ~Shader();

        void bind() const;
        void unbind() const;
        void setVec2(const std::string& name, float x, float y);
        void setVec3(const std::string& name, float x, float y, float z);
        void setVec4(const std::string& name, float x, float y, float z, float w);
        void setMat4(const std::string& name, const float* value);
        void setFloat(const std::string& name, float value);

    private:
        GLuint m_program = 0;
        std::unordered_map<std::string, GLint> m_uniformCache;

        GLint uniformLocation(const std::string& name);
        static GLuint compile(GLenum type, const std::string& src);
    };

}