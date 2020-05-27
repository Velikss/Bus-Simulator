#version 450 core

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D samplerFont;

layout(binding = 3) uniform OverlayUniformObject {
    vec3 color;
} uniforms;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
    // The texture shows contains all the letters in red,
    // so we're using the red channel as the value for
    // our alpha channel, and combining it with the color
    // from the uniform object
    float value = texture(samplerFont, inUV).r;
    if (value == 0)
    {
        outFragColor = vec4(0);
    }
    else
    {
        outFragColor = vec4(uniforms.color, value);
    }
}
