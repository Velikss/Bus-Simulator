#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <glm/gtx/hash.hpp>

struct Vertex
{
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription tBindingDescription = {};
        tBindingDescription.binding = 0;
        tBindingDescription.stride = sizeof(Vertex);
        tBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return tBindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> atAttributeDescriptions = {};

        // Attribute at location 0 (pos)
        atAttributeDescriptions[0].binding = 0;
        atAttributeDescriptions[0].location = 0;
        atAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[0].offset = offsetof(Vertex, pos);

        // Attribute at location 1 (color)
        atAttributeDescriptions[1].binding = 0;
        atAttributeDescriptions[1].location = 1;
        atAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[1].offset = offsetof(Vertex, color);

        // Attribute at location 2 (texCoord)
        atAttributeDescriptions[2].binding = 0;
        atAttributeDescriptions[2].location = 2;
        atAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        atAttributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        return atAttributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord;
    }
};

namespace std
{
    template<>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   (hash<glm::vec2>()(vertex.texCoord) << 1);
        }
    };
}
