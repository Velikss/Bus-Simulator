#pragma once
#include <pch.hpp>
#include <buffers/Buffer.hpp>
#include <loaders/objectloader.hpp>
#include <shaders/ShaderProgram.hpp>

/*
Geometry class, seperate from the mesh this only contains buffer data; vertices, etc.
*/
class Geometry
{
protected:
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
public:
	VertexArray vao;
	VertexBuffer vbo_vertices, vbo_texCoords, vbo_normals;
	float height = 0.0f; // the total initial height of the object.
	GLuint bufferSize = 0; // size of the complete geometry.
	void Bind()
	{
		vao.Bind();
	}
	void UnBind()
	{
		vao.UnBind();
	}
	// resizes the uvs to a different scale; this function is to generate more repetitions of a texture.
	void resizeUVs(float scale)
	{
		for (auto& coord : texCoords)
			coord *= scale;
		vao.Destroy();
		build();
	}
	void build()
	{
		bufferSize = vertices.size();

		// find the initial height, nice for calculations later on in positioning.
		float min = 0.0;
		float max = 0.0;
		for (auto& vert : vertices)
		{
			if (vert.y < min) min = vert.y;
			if (vert.y > max) max = vert.y;
		}
		if (min < 0) min *= -1.0;
		if (max < 0) max *= -1.0;
		height = min + max;
		if (height <= 0.0) 
			height = 0.01;

		// create the buffers.
		vao = VertexArray();
		vbo_vertices = VertexBuffer();
		vbo_normals = VertexBuffer();
		vbo_texCoords = VertexBuffer();

		vbo_vertices.Generate();
		vbo_vertices.Bind();
		vbo_vertices.AddBufferData(vertices);
		vbo_vertices.UnBind();
		vbo_vertices.Finalize();

		vbo_normals.Generate();
		vbo_normals.Bind();
		vbo_normals.AddBufferData(normals);
		vbo_normals.UnBind();
		vbo_normals.Finalize();

		vbo_texCoords.Generate();
		vbo_texCoords.Bind();
		vbo_texCoords.AddBufferData(texCoords);
		vbo_texCoords.UnBind();
		vbo_texCoords.Finalize();

		vao.Generate();
		vao.Bind();
		vao.AppendVertexBuffer(AttrPosition, vbo_vertices);
		vao.AppendVertexBuffer(AttrNormals, vbo_normals);
		vao.AppendVertexBuffer(AttrTexCoord, vbo_texCoords);
		vao.UnBind();
		vao.Finalize();
	}
	static Geometry FromObjFile(string filePath);
	static Geometry FromVectorArray(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords);
};

Geometry Geometry::FromObjFile(string filePath)
{
	Geometry geo;
	if (!loadOBJ(filePath.c_str(), geo.vertices, geo.texCoords, geo.normals))
		throw std::runtime_error("failed to load the file: " + filePath + ", is the path correct?");
	geo.build();
	return geo;
}

Geometry Geometry::FromVectorArray(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords)
{
	Geometry geo;
	geo.vertices = vertices;
	geo.normals = normals;
	geo.texCoords = texCoords;
	geo.build();
	return geo;
}