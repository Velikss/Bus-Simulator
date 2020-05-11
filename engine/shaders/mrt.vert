#version 450
#extension GL_ARB_separate_shader_objects : enable

// Don't forget to compile the shaders after modifying
// them using the provided 'compile.sh' (Linux) script.
// This will also show you any errors in your shaders

// Uniforms related to the Object we're drawing
layout(set = 0, binding = 0) uniform ObjectUniforms {
    mat4 model;// model matrix
    bool lighting;
} obj;

// Uniforms related to the Camera
layout(set = 1, binding = 0) uniform CameraUniforms {
    mat4 view;// view matrix
    mat4 proj;// projection matrix
} cam;

// Vertex information
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

// Information to pass to the fragments
layout(location = 0) out vec3 outNormal;
layout(location = 1) out vec2 outTexCoord;
layout(location = 2) out vec3 outColor;
layout(location = 3) out vec3 outWorldPos;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() {
    vec4 pos = vec4(inPosition, 1.0);

    gl_Position = cam.proj * cam.view * obj.model * pos;

    outTexCoord = inTexCoord;
    outTexCoord.t = 1.0 - outTexCoord.t;

    if (obj.lighting)
    {
        // Vertex position in world space
        outWorldPos = vec3(obj.model * pos);
        // GL to Vulkan coord space
        outWorldPos.y = -outWorldPos.y;
    }
    else
    {
        outWorldPos = vec3(999999, 999999, 999999);
    }

    // Normal in world space
    mat3 mNormal = transpose(inverse(mat3(obj.model)));
    outNormal = mNormal * normalize(inNormal);

    // Currently just vertex color
    outColor = inColor;
}
