#version 450 core

layout (location = 0) in vec2 inUV;
layout (location = 1) in vec4 text;

layout (set = 1, binding = 0) uniform sampler2D samplerTexture;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
    if (text.x == 0)
    {
        outFragColor = texture(samplerTexture, inUV);
    }
    else
    {
        float value = texture(samplerTexture, inUV).r;
        if (value < 0.6)
        {
            discard;
        }
        else
        {
            outFragColor = vec4(text.gba, 1);
        }
    }
}
