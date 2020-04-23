#version 450
#extension GL_ARB_separate_shader_objects : enable

// Don't forget to compile the shaders after modifying
// them using the provided 'compile.sh' (Linux) script.
// This will also show you any errors in your shaders

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}
