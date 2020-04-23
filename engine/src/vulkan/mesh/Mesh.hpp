#pragma once

#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>

// TODO: Implement mesh class
class cMesh
{
private:
    cGeometry* ppGeometry;

public:
    cMesh(cGeometry* pGeometry);
};
