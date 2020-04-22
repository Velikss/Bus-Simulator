#pragma once

#include <pch.hpp>
#include <GL/glew.h>


/*
Basic texture class.
*/
class Texture
{
protected:
    GLuint tex_id = 0;
public:
	static Texture fromFile(const char* fileName)
	{
		Texture tex;
        glGenTextures(1, &tex.tex_id);
        glBindTexture(GL_TEXTURE_2D, tex.tex_id);
        // load image, create texture and generate mipmaps
        int width, height, nrChannels;
        stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
        unsigned char* data = stbi_load(fileName, &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        return tex;
	}

    void SetProperty(GLuint property, GLuint value)
    {
        glTexParameteri(GL_TEXTURE_2D, property, value);
    }

	virtual void Bind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}

    virtual void UnBind()
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
};