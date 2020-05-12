if [[ -z "${VULKAN_SDK}" ]]; then
    glslc mrt.vert -o compiled/mrt.vert.spv
    glslc mrt.frag -o compiled/mrt.frag.spv
    glslc lighting.vert -o compiled/lighting.vert.spv
    glslc lighting.frag -o compiled/lighting.frag.spv
    glslc text.frag -o compiled/text.frag.spv
    glslc text.vert -o compiled/text.vert.spv
else
    "${VULKAN_SDK}/bin/glslc" mrt.vert -o compiled/mrt.vert.spv
    "${VULKAN_SDK}/bin/glslc" mrt.frag -o compiled/mrt.frag.spv
    "${VULKAN_SDK}/bin/glslc" lighting.vert -o compiled/lighting.vert.spv
    "${VULKAN_SDK}/bin/glslc" lighting.frag -o compiled/lighting.frag.spv
    "${VULKAN_SDK}/bin/glslc" text.frag -o compiled/text.frag.spv
    "${VULKAN_SDK}/bin/glslc" text.vert -o compiled/text.vert.spv
fi