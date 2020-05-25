#version 450 core

layout (location = 0) in vec2 inUV;

layout (set = 1, binding = 0) uniform sampler2D samplerTexture;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
    outFragColor = texture(samplerTexture, inUV);
}
