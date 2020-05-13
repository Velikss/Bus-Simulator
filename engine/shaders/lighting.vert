#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 3) in vec2 inTexCoord;

layout (location = 0) out vec2 outTexCoord;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    outTexCoord = inTexCoord;
    gl_Position = vec4(inPosition, 1.0f);
}