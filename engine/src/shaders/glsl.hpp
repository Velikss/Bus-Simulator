#pragma once
#include <pch.hpp>
#include <GL/glew.h>

/*
Sorry Eltjo but I like hpp files, I know I know....

The glsl is know a static class which has a shaderType this way multiple shaders could have the same fragment shader and thus
only need to be compiled once.
*/
class glsl
{
    static bool compiledStatus(GLint shaderID);
public:
    enum class shaderType
    {
        SHADER_UNKOWN,
        SHADER_VERTEX,
        SHADER_FRAGMENT
    };
    static GLuint compileShader(const byte* shaderSource, shaderType type);
	static GLuint makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID);
};

inline bool glsl::compiledStatus(GLint shaderID)
{
    GLint compiled = 0;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compiled);
    if (compiled)
        return true;
    else 
    {
        GLint logLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
        char* msgBuffer = new char[logLength];
        glGetShaderInfoLog(shaderID, logLength, NULL, msgBuffer);
        printf("%s\n", msgBuffer);
        delete[] msgBuffer;
        return false;
    }
}

inline GLuint glsl::compileShader(const byte* shaderSource, glsl::shaderType type)
{
    GLuint shaderID = glCreateShader(
        type == shaderType::SHADER_VERTEX ? 
        GL_VERTEX_SHADER : 
        GL_FRAGMENT_SHADER);
    glShaderSource(shaderID, 1, (const GLchar**)&shaderSource, NULL);
    glCompileShader(shaderID);
    if (!compiledStatus(shaderID))
        return -1;
    return shaderID;
}

inline GLuint glsl::makeShaderProgram(GLuint vertexShaderID, GLuint fragmentShaderID)
{
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShaderID);
    glAttachShader(shaderID, fragmentShaderID);
    glLinkProgram(shaderID);
    return shaderID;
}