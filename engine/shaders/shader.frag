#version 450
#extension GL_ARB_separate_shader_objects : enable

// Don't forget to compile the shaders after modifying
// them using the provided 'compile.sh' (Linux) script.
// This will also show you any errors in your shaders

// Texture sampler
layout(binding = 1) uniform sampler2D texSampler;

// Inputs from the vertex shader
layout(location = 0) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

// Final color for this fragment
layout(location = 0) out vec4 outColor;

void main() {
    // Set the color based on the texture
    outColor = texture(texSampler, inTexCoord);
}
