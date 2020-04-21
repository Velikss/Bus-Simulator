#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription GetBindingDescription()
    {
        VkVertexInputBindingDescription tBindingDescription = {};
        tBindingDescription.binding = 0;
        tBindingDescription.stride = sizeof(Vertex);
        tBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return tBindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> atAttributeDescriptions = {};

        // Attribute at location 0
        atAttributeDescriptions[0].binding = 0;
        atAttributeDescriptions[0].location = 0;
        atAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        atAttributeDescriptions[0].offset = offsetof(Vertex, pos);

        // Attribute at location 1
        atAttributeDescriptions[1].binding = 0;
        atAttributeDescriptions[1].location = 1;
        atAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        atAttributeDescriptions[1].offset = offsetof(Vertex, color);

        return atAttributeDescriptions;
    }
};