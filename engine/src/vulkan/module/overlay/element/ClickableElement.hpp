#pragma once
#include <pch.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/collision/CollisionHelper.hpp>

class cClickableElement : public cStaticElement
{
protected:
    std::vector<tRectangle> paClickableRegions;
public:
    ClickableElement() : cStaticE
};