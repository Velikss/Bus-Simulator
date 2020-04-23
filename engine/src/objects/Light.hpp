#pragma once
#include <objects/BaseObject.hpp>
#include <shaders/ShaderProgram.hpp>

/*
The class that represents light in the scene, this could also hold a mesh but isn't required.
*/
class Light : public BaseObject
{
public:
	bool represent = false; // whether it should draw the base object.
	// material attributes for the light.
	glm::vec3 ambient = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f);
	float radius = 25.0f; // the maximum range the light should travel.

	Light(Mesh* mesh, bool render = false) : BaseObject(mesh), represent(render)
	{
		mesh->useLighting = false;
	}

	// sets the light properties in the shader given an index.
	void PutOnShader(ShaderProgram* shader, int i)
	{
		glm::vec3 actual_pos = pPosition;
		actual_pos.y += 1.0f;
		shader->setVec3("lights[" + std::to_string(i) + "].position", actual_pos);
		shader->setVec3("lights[" + std::to_string(i) + "].ambient", ambient);
		shader->setVec3("lights[" + std::to_string(i) + "].diffuse", diffuse);
		shader->setVec3("lights[" + std::to_string(i) + "].specular", specular);
		shader->setFloat("lights[" + std::to_string(i) + "].radius", radius);
	}

	void Render()
	{
		if (represent)
			BaseObject::Render();
	}
};
