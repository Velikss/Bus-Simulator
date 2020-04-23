#pragma once
#include <pch.hpp>
#include <vulkan/geometry/Geometry.hpp>

class cMesh
{
private:
    cGeometry* ppGeometry;

public:
    cMesh(cGeometry* pGeometry);
};
