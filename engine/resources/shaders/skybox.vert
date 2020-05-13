#version 410 core
layout(location = 0) in vec3 FragPos;

out vec3 TexCoords;

uniform mat4 pv;
uniform mat4 model;

void main()
{
    TexCoords = FragPos;
    vec4 pPosition = pv * model * vec4(FragPos, 1.0);
    gl_Position = pPosition.xyww;
}  