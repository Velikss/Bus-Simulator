#pragma once
#include <pch.hpp>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
An abstract buffer class.
*/
class Buffer
{
protected:
	bool locked = false;
public:
	// to initialize a buffer.
	virtual void Generate() = 0;
	// binds the buffer in gl.
	virtual void Bind() = 0;
	// unbinds the buffer in gl.
	virtual void UnBind() = 0;
	// destorys the buffer.
	virtual void Destroy() = 0;
	// locks the buffer for any further additions etc.
	void Finalize()
	{
		locked = true;
	}
};

/*
Basic vbo class.
*/
class VertexBuffer : public Buffer
{
	bool bound = false;
	// a vertex buffer is bound to a data size. thus if you were to add vec3s and later vec2 you would recieve an error.
	GLuint dataSize = 0;
public:
	GLuint vbo = 0;
	void Generate()
	{
		glGenBuffers(1, &vbo);
	}
	void AddBufferData(std::vector<glm::vec3> & data)
	{
		if (locked) throw std::runtime_error("buffer is locked");
		if (dataSize > 0 && dataSize != 3) throw std::runtime_error("invalid data size");
		if (!bound) throw std::runtime_error("VertexBuffer not bound.");
		glBufferData(GL_ARRAY_BUFFER,
			data.size() * sizeof(glm::vec3), &data[0],
			GL_STATIC_DRAW);
		dataSize = 3;
	}
	void AddBufferData(std::vector<glm::vec2> & data)
	{
		if (locked) throw std::runtime_error("buffer is locked");
		if (dataSize > 0 && dataSize != 2) throw std::runtime_error("invalid data size");
		if (!bound) throw std::runtime_error("VertexBuffer not bound.");
		glBufferData(GL_ARRAY_BUFFER,
			data.size() * sizeof(glm::vec2), &data[0],
			GL_STATIC_DRAW);
		dataSize = 2;
	}
	GLuint& GetDataSize()
	{
		return this->dataSize;
	}
	void Bind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		bound = true;
	}
	void UnBind()
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		bound = false;
	}
	void Destroy()
	{
		glDeleteBuffers(1, &vbo);
	}
};

/*
Basic vao class.
*/
class VertexArray : public Buffer
{
	bool bound = false;
	// holds the ids of the added buffers to destroy the whole thing in one go.
	std::stack<VertexBuffer> addedBuffers;
public:
	GLuint vao = 0;
	void Generate()
	{
		glGenVertexArrays(1, &vao);
	}
	void AppendVertexBuffer(GLuint attributeId, VertexBuffer& buffer)
	{
		if (locked) throw std::runtime_error("array is locked");
		if (!bound) throw std::runtime_error("VertexArray not bound");
		buffer.Bind();
		glVertexAttribPointer(attributeId, buffer.GetDataSize(), GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(attributeId);
		buffer.UnBind();
		addedBuffers.push(buffer);
	}
	void Bind()
	{
		glBindVertexArray(vao);
		bound = true;
	}
	void UnBind()
	{
		glBindVertexArray(0);
		bound = false;
	}
	void Destroy()
	{
		while (!addedBuffers.empty())
		{
			addedBuffers.top().Destroy();
			addedBuffers.pop();
		}
		glDeleteVertexArrays(1, &vao);
	}
};