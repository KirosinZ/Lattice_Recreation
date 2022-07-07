#include "Program.h"
#include <glm/gtc/type_ptr.hpp>


Shading::Program::Program(const Shader &vertex, const Shader &fragment)
{
    if(vertex.type() != ShaderType::sVertex || fragment.type() != ShaderType::sFragment) throw std::exception("Shader type mismatch");

    auto v = vertex.compile();
    auto f = fragment.compile();
    
    ID = glCreateProgram();
    glAttachShader(ID, v);
    glAttachShader(ID, f);
    glLinkProgram(ID);
    
    int success;
    char infolog[512];
    
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(ID, 512, nullptr, infolog);
        std::string log(infolog);
        std::string msg = "Program linking failure:" + log + "\n";
        throw std::exception(msg.c_str());
    }

    glDeleteShader(v);
    glDeleteShader(f);
}

Shading::Program::~Program()
{
    glDeleteProgram(ID);
}

void Shading::Program::use() const
{
    glUseProgram(ID);
}

[[maybe_unused]] int Shading::Program::location(const std::string &name) const
{
    return glGetUniformLocation(ID, name.c_str());
}

void Shading::Program::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shading::Program::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shading::Program::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shading::Program::setVec2(const std::string &name, glm::vec2 value) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shading::Program::setVec3(const std::string &name, glm::vec3 value) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shading::Program::setVec4(const std::string &name, glm::vec4 value) const
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shading::Program::setMat4(const std::string &name, glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}