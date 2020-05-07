#version 450
#extension GL_ARB_separate_shader_objects : enable

// Don't forget to compile the shaders after modifying
// them using the provided 'compile.sh' (Linux) script.
// This will also show you any errors in your shaders

// Uniforms related to the Object we're drawing
layout(set = 0, binding = 0) uniform ObjectUniforms {
    mat4 model;// model matrix
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
layout(location = 0) out vec3 outColor;
layout(location = 1) out vec2 outTexCoord;

void main() {
    // Calculate the position of this vertex based on the object and camera position
    gl_Position = cam.proj * cam.view * obj.model * vec4(inPosition, 1.0);

    // Pass the color and texture coordinate on to the fragments
    outColor = inColor;
    outTexCoord = inTexCoord;
}
