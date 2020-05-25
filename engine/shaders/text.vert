#version 450 core

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inUV;

layout (location = 0) out vec2 outUV;
layout (location = 1) out vec4 text;

layout(set = 1, binding = 1) uniform OverlayElementObject {
    mat4 matrix;
    vec3 color;
    bool text;
} uniforms;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    gl_Position = uniforms.matrix * vec4(inPos, 0.0, 1.0);
    outUV = inUV;
    text = uniforms.text ? vec4(1, uniforms.color) : vec4(0);
}
