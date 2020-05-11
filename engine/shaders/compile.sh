if [[ -z "${VULKAN_SDK}" ]]; then
    glslc shader.vert -o vert.spv
    glslc shader.frag -o frag.spv
    glslc text.frag -o text.frag.spv
    glslc text.vert -o text.vert.spv
    glslc mrt.vert -o mrt.vert.spv
    glslc mrt.frag -o mrt.frag.spv
else
    "${VULKAN_SDK}/bin/glslc" shader.vert -o vert.spv
    "${VULKAN_SDK}/bin/glslc" shader.frag -o frag.spv
    "${VULKAN_SDK}/bin/glslc" text.frag -o text.frag.spv
    "${VULKAN_SDK}/bin/glslc" text.vert -o text.vert.spv
    "${VULKAN_SDK}/bin/glslc" mrt.vert -o mrt.vert.spv
    "${VULKAN_SDK}/bin/glslc" mrt.frag -o mrt.frag.spv
fi