#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>

struct tVertex2D
{
    glm::vec2 pos;
    glm::vec2 texCoord;

    static std::array<VkVertexInputBindingDescription, 1> GetBindingDescriptions()
    {
        std::array<VkVertexInputBindingDescription, 1> atBindingDescriptions = {};


        // Struct specifying vertex input binding
        atBindingDescriptions[0].binding = 0;                                // binding number for vertices
        atBindingDescriptions[0].stride = sizeof(tVertex2D);                 // distance between two elements
        atBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;    // defines vertex addressing function

        /*atBindingDescriptions[1].binding = 1;
        atBindingDescriptions[1].stride = sizeof(tVertex2D);
        atBindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;*/

        return atBindingDescriptions;
    }

    static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 2> atAttributeDescriptions = {};

        // Attribute at location 0 (pos)
        atAttributeDescriptions[0].binding = 0;
        atAttributeDescriptions[0].location = 0;
        atAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        atAttributeDescriptions[0].offset = offsetof(tVertex2D, pos);

        // Attribute at location 1 (texCoord)
        atAttributeDescriptions[1].binding = 0;
        atAttributeDescriptions[1].location = 1;
        atAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
        atAttributeDescriptions[1].offset = offsetof(tVertex2D, texCoord);

        return atAttributeDescriptions;
    }
};
