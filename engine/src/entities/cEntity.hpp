#pragma once

#include <pch.hpp>
#include <objects/BaseObject.hpp>

class cEntity
{
public:
    glm::vec3 ptPosition = glm::vec3(0.0f, 0.0f, 0.0f);

    cEntity(glm::vec3 tPosition)
    {
        ptPosition = tPosition;
    }
};

