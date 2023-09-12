//
// Created by apgra on 9/12/2023.
//

#include "Shader.h"


#include <glad/glad.h>
#include "Shader.h"


namespace Renderer {
    Shader::Shader() : m_RendererID(0) {

    }

    Shader::Shader(const char *vertPath, const char *fragPath) {
        m_RendererID = createShaderProgram(vertPath, fragPath);
    }

    unsigned int Shader::createShaderProgram(const char *vertPath, const char *fragPath) {
        unsigned int vert, frag;
        vert = createShader(GL_VERTEX_SHADER, readSourceFile(vertPath).c_str());
        frag = createShader(GL_FRAGMENT_SHADER, readSourceFile(fragPath).c_str());


        unsigned int id = glCreateProgram();
        glAttachShader(id, vert);
        glAttachShader(id, frag);

        glLinkProgram(id);

        int success;
        char infoLog[512];
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 512, nullptr, infoLog);
            std::cout << "ERROR::SHADER::LINKING_FAILURE\n" << infoLog << "\n";
        }

        //Cleanup
        glDeleteShader(vert);
        glDeleteShader(frag);

        return id;
    }

    unsigned int Shader::createShader(unsigned int SHADER_TYPE, const char *src) {
        if (src == nullptr) {
            throw std::runtime_error("ERROR: Shader received null source");
        }
        unsigned int shader = glCreateShader(SHADER_TYPE);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);


        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, nullptr, infoLog);
            std::cout << "ERROR::" << ((SHADER_TYPE == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT")
                      << "::SHADER::COMPILATION_FAILED\n" << infoLog << "\n";
        }

        return shader;
    }

    std::string Shader::readSourceFile(const char *srcPath) {
        std::ifstream file;
        std::stringstream stream;

        try {
            file.open(srcPath);
            stream << file.rdbuf();
        } catch (std::exception &e) {
            throw std::runtime_error(e.what());
        }


        return stream.str();
    }

    void Shader::bind() const {
        glUseProgram(m_RendererID);
    }

    void Shader::unbind() const {
        glUseProgram(0);
    }

    int Shader::getUniformLocation(const std::string &name) const {
        if (m_UniformCache.find(name) != m_UniformCache.end()) {
            return m_UniformCache[name];
        }

        int location = glGetUniformLocation(m_RendererID, name.c_str());
        m_UniformCache[name] = location;

        return location;
    }


    void Shader::setInt(const std::string &name, int v) {
        glUniform1i(getUniformLocation(name), v);
    }

    void Shader::setFloat(const std::string &name, float v) {
        glUniform1f(getUniformLocation(name), v);
    }


    void Shader::setVec2(const std::string &name, glm::vec2 v) {
        glUniform2f(getUniformLocation(name), v.x, v.y);
    }

    void Shader::setVec2(const std::string &name, float x, float y) {
        glUniform2f(getUniformLocation(name), x, y);
    }


    void Shader::setVec3(const std::string &name, glm::vec3 v) {
        glUniform3f(getUniformLocation(name), v.x, v.y, v.z);
    }

    void Shader::setVec3(const std::string &name, float x, float y, float z) {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void Shader::setVec3(const std::string &name, float v) {
        glUniform3f(getUniformLocation(name), v, v, v);
    }

    void Shader::setVec4(const std::string &name, glm::vec4 v) {
        glUniform4f(getUniformLocation(name), v.x, v.y, v.z, v.w);
    }

    void Shader::setVec4(const std::string &name, float x, float y, float z, float w) {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }


    void Shader::setMat4(const std::string &name, glm::mat4 v) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(v));
    }


//Cleanup
    Shader::~Shader() {
        glDeleteProgram(m_RendererID);
    }
}