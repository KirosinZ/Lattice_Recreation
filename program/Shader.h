#ifndef PROGRAM_SHADER_H
#define PROGRAM_SHADER_H

#include <string>
#include <gl/glew.h>

namespace Shading {

    enum class ShaderType
    {
        sVertex,
        sFragment,
    };

    class Shader
    {
        std::string m_sourceCode;
        ShaderType m_shaderType;

    public:
        explicit Shader(const std::string& filename);

        [[nodiscard]]
        GLuint compile() const;

        [[nodiscard]]
        ShaderType type() const;
    };

}

#endif //PROGRAM_SHADER_H
