#version 450
#extension GL_ARB_separate_shader_objects : enable

// Don't forget to compile the shaders after modifying
// them using the provided 'compile.sh' (Linux) script.
// This will also show you any errors in your shaders

// Texture sampler
layout(binding = 1) uniform sampler2D textureSampler;
layout(binding = 2) uniform sampler2D materialSampler;

// Inputs from the vertex shader
layout(location = 0) in vec3 inNormal;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec3 inWorldPos;

// Final color for this fragment
layout(location = 0) out vec4 outPosition;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outAlbedo;
layout(location = 3) out vec2 outMaterial;

void main() {
    outPosition = vec4(inWorldPos, 1.0);
    outNormal = vec4(inNormal, 1.0);
    outAlbedo = texture(textureSampler, inTexCoord);

    // We're just calculating the material values based on the albedo texture
    float value = (outAlbedo.r + outAlbedo.g + outAlbedo.b) / 3;
    outMaterial = vec2(1.0 - value, value);
}
