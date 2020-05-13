#version 450 core

layout (location = 0) in vec2 inUV;

layout (binding = 0) uniform sampler2D samplerFont;

layout(binding = 3) uniform OverlayUniformObject {
    vec3 color;
} uniforms;

layout (location = 0) out vec4 outFragColor;

void main(void)
{
    // The texture shows contains all the letters in white,
    // so we're using the red channel as the value for
    // our alpha channel, and combining it with the color
    // from the uniform object
    float alpha = texture(samplerFont, inUV).r;
    outFragColor = vec4(uniforms.color, alpha);
}
