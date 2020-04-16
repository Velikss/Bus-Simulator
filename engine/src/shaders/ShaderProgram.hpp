#pragma once
#include <pch.hpp>
#include <shaders/shader.hpp>
#include <objects/Material.hpp>
#include <glm/glm.hpp>

/*
Since I like generic shaders; [in] pipes are set on forces locations.
If (some) of these aren't implemented glsl doesn't bother throwing an error and just continues.
*/
const GLuint AttrPosition = 0;
const GLuint AttrTexCoord = 1;
const GLuint AttrNormals  = 2;

/*
Compiles the final shader, class with a lot of uniforms.
*/
class ShaderProgram
{
	GLuint program_id;
public:
	ShaderProgram(Shader& vert, Shader& frag)
	{
		program_id = glsl::makeShaderProgram(vert.shaderId, frag.shaderId);
	}

	void Bind()
	{
		glUseProgram(program_id);
	}

    void setBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(program_id, name.c_str()), (int)value);
    }
    
    void setInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(program_id, name.c_str()), value);
    }
    
    void setFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(program_id, name.c_str()), value);
    }
    
    void setVec2(const std::string& name, const glm::vec2& value) const
    {
        glUniform2fv(glGetUniformLocation(program_id, name.c_str()), 1, &value[0]);
    }

    void setVec2(const std::string& name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(program_id, name.c_str()), x, y);
    }
    
    void setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(program_id, name.c_str()), 1, &value[0]);
    }

    void setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(program_id, name.c_str()), x, y, z);
    }
    
    void setVec4(const std::string& name, const glm::vec4& value) const
    {
        glUniform4fv(glGetUniformLocation(program_id, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string& name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(program_id, name.c_str()), x, y, z, w);
    }
    
    void setMat2(const std::string& name, const glm::mat2& mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat3(const std::string& name, const glm::mat3& mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setMat4(const std::string& name, const glm::mat4& mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program_id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

	GLuint GetAttribute(const char* name)
	{
		return glGetAttribLocation(program_id, name);
	}

	GLuint GetUniform(const char* name)
	{
		return glGetUniformLocation(program_id, name);
	}
};