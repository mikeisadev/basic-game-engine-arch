#include "graphics/ShaderProgram.h"
#include "graphics/Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace eng
{
    ShaderProgram::ShaderProgram(GLuint shaderProgramID) : m_shaderProgramID(shaderProgramID)
    {

    }

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_shaderProgramID);
    }

    void ShaderProgram::Bind()
    {
        glUseProgram(m_shaderProgramID);
    }

    GLint ShaderProgram::GetUniformLocation(const std::string& name)
    {
        auto it = m_uniformLocationCache.find(name);

        if (it != m_uniformLocationCache.end())
        {
            return it->second;
        }

        GLint location = glGetUniformLocation(m_shaderProgramID, name.c_str());

        m_uniformLocationCache[name] = location;

        return location;
    }

    void ShaderProgram::SetUniform(const std::string& name, int value)
    {
        auto location = GetUniformLocation(name);

        glUniform1i(location, value);
    }

    void ShaderProgram::SetUniform(const std::string& name, float value)
    {
        auto location = GetUniformLocation(name);

        glUniform1f(location, value);
    }

    void ShaderProgram::SetUniform(const std::string& name, float v0, float v1)
    {
        auto location = GetUniformLocation(name);

        glUniform2f(location, v0, v1);
    }

    void ShaderProgram::SetUniform(const std::string& name, const glm::mat4& mat)
    {
        auto location = GetUniformLocation(name);

        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }

    void ShaderProgram::SetUniform(const std::string& name, const glm::vec3& value)
    {
        auto location = GetUniformLocation(name);

        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::SetUniform(const std::string& name, const glm::vec4& value)
    {
        auto location = GetUniformLocation(name);

        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    int ShaderProgram::GetTextureUnit(const std::string& name)
    {
        auto it = m_textureUnitCache.find(name);

        if (it != m_textureUnitCache.end())
        {
            return it->second;
        }

        static GLint maxUnits = 0;

        if (maxUnits == 0)
        {
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
        }

        if (m_nextTextureUnit >= maxUnits)
        {
            std::cerr << "[ShaderProgram] Troppi sampler nel programma " << m_shaderProgramID
                    << ": '" << name << "' non ha una texture unit libera (max "
                    << maxUnits << ")" << std::endl;
            return -1;
        }

        const int unit = m_nextTextureUnit++;
        m_textureUnitCache[name] = unit;
  
        glUniform1i(GetUniformLocation(name), unit);

        return unit;
    }

    void ShaderProgram::SetTexture(const std::string& name, Texture* texture)
    {
        const int unit = GetTextureUnit(name);

        if (unit < 0)
        {
            return;
        }

        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture ? texture->GetID() : 0);
    }
}