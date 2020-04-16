#pragma once
#include <pch.hpp>
#include <textures/Texture.hpp>
#include <geometries/Geometry.hpp>
#include <objects/Material.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

/*
A mesh holds all properties of the base for a model.
The shader, material or color when necessary, the geometry and a texture if necessary.
*/
class Mesh
{
public:
	// mesh settings.
	bool useTexture = false;
	bool useColor = true;
	bool useLighting = true;

	// shader properties.
	glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
	Material material = materials.at("white plastic");

	// draw data.
	Texture* tex = nullptr;
	Geometry* geo;
	ShaderProgram* shader;

	Mesh(ShaderProgram* shader, Geometry* geo, Texture* tex = nullptr)
	{
		this->shader = shader;
		this->geo = geo;
		this->tex = tex;
		if (tex != nullptr)
		{
			useTexture = true;
			useColor = false;
		}
	}

	void Bind()
	{
		geo->Bind();
		if (useTexture) tex->Bind();
		shader->Bind();
	}

	void PrepareForRender()
	{
		//set material
		shader->setVec3("material.ambient", material.ambient.x, material.ambient.y, material.ambient.z);
		shader->setVec3("material.diffuse", material.diffuse.x, material.diffuse.y, material.diffuse.z);
		shader->setVec3("material.specular", material.specular.x, material.specular.y, material.specular.z);
		shader->setFloat("material.shininess", material.shininess);

		shader->setBool("useColor", useColor);
		shader->setBool("useTexture", useTexture);
		shader->setBool("useLighting", useLighting);
		shader->setVec3("objectColor", color);
	}

	GLuint GetMeshSize()
	{
		return this->geo->bufferSize;
	}

	void UnBind()
	{
		if (useTexture) tex->UnBind();
		geo->UnBind();
	}
};