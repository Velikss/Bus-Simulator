#pragma once

#define GLM_ENABLE_EXPERIMENTAL // We need GLM experimental features for our hash function

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <glm/gtx/hash.hpp>

// Struct describing a vertex
struct Vertex
{
    // Position of this vertex in 3D space
    glm::vec3 pos;

    // Normal of this vertex
    glm::vec3 normal;

    // Color of this vertex
    glm::vec3 color;

    // Texture coordinate to use for this vertex
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        // Struct specifying vertex input binding
        VkVertexInputBindingDescription tBindingDescription = {};
        tBindingDescription.binding = 0;                                // binding number for vertices
        tBindingDescription.stride = sizeof(Vertex);                    // distance between two elements
        tBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // defines vertex addressing function

        return tBindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 4> atAttributeDescriptions = {};

        // Attribute at location 0 (pos)
        atAttributeDescriptions[0].binding = 0;
        atAttributeDescriptions[0].location = 0;
        atAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[0].offset = offsetof(Vertex, pos);

        // Attribute at location 1 (normal)
        atAttributeDescriptions[1].binding = 0;
        atAttributeDescriptions[1].location = 1;
        atAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[1].offset = offsetof(Vertex, normal);

        // Attribute at location 2 (color)
        atAttributeDescriptions[2].binding = 0;
        atAttributeDescriptions[2].location = 2;
        atAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[2].offset = offsetof(Vertex, color);

        // Attribute at location 3 (texCoord)
        atAttributeDescriptions[3].binding = 0;
        atAttributeDescriptions[3].location = 3;
        atAttributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        atAttributeDescriptions[3].offset = offsetof(Vertex, texCoord);

        return atAttributeDescriptions;
    }

    bool operator==(const Vertex& other) const
    {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && normal == other.normal;
    }
};

// Hash function for vertices
namespace std
{
    template<>
    struct hash<Vertex>
    {
        size_t operator()(Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   ((hash<glm::vec2>()(vertex.texCoord) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1);
        }
    };
}
