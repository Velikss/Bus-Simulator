#pragma once
#include <pch.hpp>
#include <loaders/fileloader.hpp>
#include <vector>
#include <string>
#include <GL/glew.h>
#include <shaders/glsl.hpp>

/*
Main sub-shader holder. at the moment either a vertex or fragment shader.
*/
class Shader
{
public:
	// props
	GLuint shaderId = -1;
	string filename;
	glsl::shaderType type = glsl::shaderType::SHADER_UNKOWN;

	// func
	static Shader fromFile(string fileName);
};

inline Shader Shader::fromFile(string fileName)
{
	Shader shader;
	shader.filename = fileName;
	std::vector<std::string> words;
	//str::split(fileName, words, ".");
	if (words.back() == "vert")
		shader.type = glsl::shaderType::SHADER_VERTEX;
	else if (words.back() == "frag")
		shader.type = glsl::shaderType::SHADER_FRAGMENT;
	else
		shader.type = glsl::shaderType::SHADER_UNKOWN;

	byte* vertexshader = fs::readFile(fileName.c_str());
	shader.shaderId = glsl::compileShader(vertexshader, shader.type);

	return shader;
}