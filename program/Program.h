#ifndef PROGRAM_PROGRAM_H
#define PROGRAM_PROGRAM_H


#include "Shader.h"
#include <glm/glm.hpp>

namespace Meta {
    template<class T>
    constexpr bool always_false() { return false; }
}

namespace Shading {

    class Program
    {
        unsigned int ID = 0;


    public:
        Program(const Shader &vertex, const Shader &fragment);

        ~Program();

        void use() const;

        [[maybe_unused]] [[nodiscard]]
        int location(const std::string &name) const;

        void setBool(const std::string &name, bool value) const;
        void setInt(const std::string &name, int value) const;
        void setFloat(const std::string &name, float value) const;

        void setVec2(const std::string &name, glm::vec2 value) const;

        void setVec3(const std::string &name, glm::vec3 value) const;

        void setVec4(const std::string &name, glm::vec4 value) const;

        void setMat4(const std::string &name, glm::mat4 value) const;

        template<typename T>
        void set(const std::string& name, T val) const;
    };

    template<typename T>
    void Program::set(const std::string& name, T val) const
    {
        if constexpr(std::same_as<T, bool>)
        {
            setBool(name, val);
        }
        else if constexpr(std::same_as<T, int>)
        {
            setInt(name, val);
        }
        else if constexpr(std::same_as<T, float>)
        {
            setFloat(name, val);
        }
        else if constexpr(std::same_as<T, glm::vec2>)
        {
            setVec2(name, val);
        }
        else if constexpr(std::same_as<T, glm::vec3>)
        {
            setVec3(name, val);
        }
        else if constexpr(std::same_as<T, glm::vec4>)
        {
            setVec4(name, val);
        }
        else if constexpr(std::same_as<T, glm::mat4>)
        {
            setMat4(name, val);
        }
        else
        {
            static_assert(Meta::always_false<T>(), "Argument type not supported");
        }
    }


}

#endif //PROGRAM_PROGRAM_H
