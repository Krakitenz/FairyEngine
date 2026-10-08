#include "core/Shader.h"
#include <stdexcept>
#include <fstream>
#include <sstream>

namespace fairy::core {


    static std::string readFile(const std::string& path) {
        std::ifstream f(path, std::ios::binary);
        if (!f) throw std::runtime_error("Cannot open shader: " + path);
        std::stringstream ss;
        ss << f.rdbuf();
        return ss.str();
    }

    Shader::Shader(const std::string& vertPath, const std::string& fragPath, bool /*fromFile*/) {
        std::string vs = readFile(vertPath);
        std::string fs = readFile(fragPath);
        GLuint v = compile(GL_VERTEX_SHADER, vs);
        GLuint f = compile(GL_FRAGMENT_SHADER, fs);
        m_program = glCreateProgram();
        glAttachShader(m_program, v);
        glAttachShader(m_program, f);
        glLinkProgram(m_program);
        glDeleteShader(v);
        glDeleteShader(f);
    }
    GLuint Shader::compile(GLenum type, const std::string& src) {
        GLuint shader = glCreateShader(type);
        const char* c = src.c_str();
        glShaderSource(shader, 1, &c, nullptr);
        glCompileShader(shader);

        GLint ok = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            throw std::runtime_error(std::string("Shader error:\n") + log);
        }
        return shader;
    }

    Shader::Shader(const std::string& vs, const std::string& fs) {
        GLuint v = compile(GL_VERTEX_SHADER, vs);
        GLuint f = compile(GL_FRAGMENT_SHADER, fs);

        m_program = glCreateProgram();
        glAttachShader(m_program, v);
        glAttachShader(m_program, f);
        glLinkProgram(m_program);

        GLint ok = 0;
        glGetProgramiv(m_program, GL_LINK_STATUS, &ok);
        if (!ok) {
            char log[1024];
            glGetProgramInfoLog(m_program, sizeof(log), nullptr, log);
            glDeleteProgram(m_program);
            throw std::runtime_error(std::string("Link error:\n") + log);
        }

        glDeleteShader(v);
        glDeleteShader(f);
    }

    Shader::~Shader() {
        if (m_program) glDeleteProgram(m_program);
    }

    void Shader::bind()   const { glUseProgram(m_program); }
    void Shader::unbind() const { glUseProgram(0); }

    void Shader::setVec2(const std::string& n, float x, float y) {
        glUniform2f(uniformLocation(n), x, y);
    }
    void Shader::setVec3(const std::string& n, float x, float y, float z) {
        glUniform3f(uniformLocation(n), x, y, z);
    }

    void Shader::setVec4(const std::string& n, float x, float y, float z, float w) {
        glUniform4f(uniformLocation(n), x, y, z, w);
    }

    GLint Shader::uniformLocation(const std::string& name) {
        auto it = m_uniformCache.find(name);
        if (it != m_uniformCache.end()) return it->second;
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        m_uniformCache[name] = loc;
        return loc;
    }
    void Shader::setFloat(const std::string& n, float v) {
        glUniform1f(uniformLocation(n), v);
    }

    void Shader::setMat4(const std::string& n, const float* v) {
        glUniformMatrix4fv(uniformLocation(n), 1, GL_FALSE, v);
    }

}