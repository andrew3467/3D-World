//
// Created by apgra on 9/12/2023.
//

#ifndef APPLICATION_SHADER_H
#define APPLICATION_SHADER_H


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>


#include "glm/glm.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "../Engine/Lights.h"

class Shader {
private:
    unsigned int m_Handle;
    mutable std::unordered_map<std::string, int> m_UniformCache;

public:
    Shader();

    Shader(const char *path);

    ~Shader();

private:
    unsigned int createShaderProgram(const char *path);

    std::vector<std::string> parseShader(const char *path);

    unsigned int createShader(unsigned int SHADER_TYPE, const char *src);

    int getUniformLocation(const std::string &name) const;

public:
    void bind() const;
    void unbind() const;

    static void initializeShaders();

    inline unsigned int getID() const { return m_Handle; }

//Uniforms
public:
    void setInt(const std::string &name, int v);

    void setFloat(const std::string &name, float v);

    void setVec4(const std::string &name, glm::vec4 v);

    void setVec4(const std::string &name, float x, float y, float z, float w);

    void setVec3(const std::string &name, glm::vec3 v);

    void setVec3(const std::string &name, float x, float y, float z);

    void setVec3(const std::string &name, float v);

    void setVec2(const std::string &name, glm::vec2 v);

    void setVec2(const std::string &name, float x, float y);

    void setMat4(const std::string &name, glm::mat4 v);

    void setPointLight(std::string &arrIndex, PointLight &light);

};


namespace Shaders{
    inline Shader* solid_unlit;
    inline Shader* solid_lit;
}



#endif //APPLICATION_SHADER_H
