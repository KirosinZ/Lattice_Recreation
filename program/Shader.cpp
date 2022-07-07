#include <fstream>
#include <sstream>
#include "Shader.h"

Shading::Shader::Shader(const std::string &filename)
{
    auto dot = std::find(filename.rbegin(), filename.rend(), '.');
    std::string ext;
    std::copy(filename.rbegin(), dot, std::back_insert_iterator<std::string>(ext));

    if (ext == "trev") { // vert
        m_shaderType = ShaderType::sVertex;
    } else if(ext == "garf") { // frag
        m_shaderType = ShaderType::sFragment;
    }
    else
        throw std::exception("Shader extension not supported");

    std::ifstream file(filename);

    std::stringstream filestream;
    filestream << file.rdbuf();

    file.close();

    m_sourceCode = filestream.str();
}

GLuint Shading::Shader::compile() const
{
    GLuint res;
    int success;
    char infolog[512];

    const char *const codestr = m_sourceCode.c_str();

    GLenum gltype;
    switch(m_shaderType)
    {
        case ShaderType::sVertex:
            gltype = GL_VERTEX_SHADER;
            break;
        case ShaderType::sFragment:
            gltype = GL_FRAGMENT_SHADER;
            break;
        default:
            throw std::exception("Shader type not supported");
    }

    res = glCreateShader(gltype);
    glShaderSource(res, 1, &codestr, nullptr);
    glCompileShader(res);

    glGetShaderiv(res, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(res, 512, nullptr, infolog);
        std::string log(infolog);
        std::string msg = "Shader compilation failed:" + log + "\n";
        throw std::exception(msg.c_str());
    }

    return res;
}

Shading::ShaderType Shading::Shader::type() const
{
    return m_shaderType;
}