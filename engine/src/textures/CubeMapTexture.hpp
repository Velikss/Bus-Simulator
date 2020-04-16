#pragma once
#include <pch.hpp>
#include <textures/Texture.hpp>

/*
A cubemap is different from a texture, though it is generally loaded the same.
*/
class CubeMapTexture : public Texture
{
public:
    static CubeMapTexture fromFile(std::vector<string> fileNames)
    {
        // create shader and bind.
        CubeMapTexture tex;
        glGenTextures(1, &tex.tex_id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex.tex_id);
        // load image, create texture and generate mipmaps
        stbi_set_flip_vertically_on_load(false); // tell stb_image.h to flip loaded texture's on the y-axis.
        int width, height, nrChannels;
        for (unsigned int i = 0; i < fileNames.size(); i++)
        {
            unsigned char* data = stbi_load(fileNames[i].c_str(), &width, &height, &nrChannels, 0);
            if (data)
            {
                // auto load it at the correct positions, list positions are thus important.
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << fileNames[i] << std::endl;
                stbi_image_free(data);
            }
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        return tex;
    }

    void Bind()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, tex_id);
    }

    void UnBind()
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
};