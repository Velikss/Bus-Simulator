#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

uniform mat4 pv;
uniform mat4 model;
layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec2 TexCoords;
layout(location = 2) out vec3 Normal;

void main()
{
    gl_Position = pv * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = normal;
    TexCoords = texCoord;
}
