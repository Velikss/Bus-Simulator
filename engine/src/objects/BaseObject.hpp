#pragma once
#include <pch.hpp>
#include <buffers/Buffer.hpp>
#include <objects/Mesh.hpp>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>

/*
A basic object is an object that can be drawn in 3d space.
It only holds positional data and is linked to a static mesh.
Which keeps memory as low as possible.
*/
class BaseObject
{
public:
	bool render = true; // wether the object should be rendered or not.
	Mesh* mesh = nullptr; // pointer to the linked mesh.

	// components of the model matrix.
	glm::vec3 rot = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f); 

	BaseObject(Mesh* mesh, float posOnScale1 = false)
		: mesh(mesh)
	{
		// a plain generally doesn't have any height and thus these are positioned in a different way.
		if (posOnScale1)
			SetPosScale1(0.0, 0.0, 0.0);
		else
			SetPos(0.0, 0.0, 0.0);
	}

	virtual void SetSize(float width, float length, float height)
	{
		scale = glm::vec3(width / 2, height / 2, length / 2);
	}

	virtual void SetPositionalHeigth(float y)
	{
		pos.y = y - (mesh->geo->height > 0 ? (mesh->geo->height / 2) : 0);
	}

	virtual void SetScale(float scaler)
	{
		scale = glm::vec3(scaler, scaler, scaler);
	}

	virtual void SetHeightWidth(float width, float length)
	{
		scale.x = width;
		scale.z = length;
	}

	virtual void SetLength(float length)
	{
		scale.z = length;
	}

	virtual void SetWidth(float width)
	{
		scale.x = width;
	}

	virtual void SetHeight(float height)
	{
		scale.y = height;
	}

	virtual void SetPos(float x, float y, float z)
	{
		// return if base is a group.
		if (!mesh) return;
		pos.x = x;
		pos.y = y - (mesh->geo->height > 0 ? (mesh->geo->height / 2) : 0);
		pos.z = z;
	}

	virtual void SetPosition(float x, float z)
	{
		pos.x = x;
		pos.z = z;
	}

	virtual void SetPosScale1(float x, float y, float z)
	{
		// return if base is a group.
		if (!mesh) return;
		pos.x = x;
		pos.y = (y - scale.y) * (1.0f / scale.y);
		pos.z = z;
	}

	virtual void MoveForward(float multiplier = 1.0)
	{
		glm::vec3 direction(sin(glm::radians(rot.y)), 0, cos(glm::radians(rot.y)));
		pos -= (direction * multiplier);
	}

	virtual void MoveBackward(float multiplier = 1.0)
	{
		glm::vec3 direction(sin(glm::radians(rot.y)), 0, cos(glm::radians(rot.y)));
		pos += (direction * multiplier);
	}

	virtual void MoveLeft(float angle_diff)
	{
		if (rot.y >= 360.0f)
			rot.y = 0;
		rot.y += angle_diff;
	}

	virtual void MoveRight(float angle_diff)
	{
		if (rot.y < 0.0f)
			rot.y = 360.0f;
		rot.y -= angle_diff;
	}

	virtual void Update()
    {

    }

	virtual void Render()
	{
		if(render)
			Render(pos, rot, scale);
	}

	virtual void Render(glm::vec3& _pos, glm::vec3& _rot, glm::vec3& _scale)
	{
		glm::mat4 model(1.0f);

		// Translation
		model = glm::translate(
			model,
			_pos);

		// Scaling
		model = glm::scale(
			model,
			_scale);

		// Rotation_X
		if (_rot.x > 0.0)
		{
			const glm::vec3 rot_x = glm::vec3(1.0f, 0.0f, 0.0f);
			model = glm::rotate(
				model,
				glm::radians(_rot.x),
				rot_x);
		}

		// Rotation_Y
		if (_rot.y > 0.0)
		{
			const glm::vec3 rot_y = glm::vec3(0.0f, 1.0f, 0.0f);
			model = glm::rotate(
				model,
				glm::radians(_rot.y),
				rot_y);
		}

		// Rotation_Z
		if (_rot.z > 0.0)
		{
			const glm::vec3 rot_z = glm::vec3(0.0f, 0.0f, 1.0f);
			model = glm::rotate(
				model,
				glm::radians(_rot.z),
				rot_z);
		}

		mesh->Bind();

		mesh->PrepareForRender();

		mesh->shader->setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, mesh->GetMeshSize());

		mesh->UnBind();
	}
};

typedef BaseObject Model;