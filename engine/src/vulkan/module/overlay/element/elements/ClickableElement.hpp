#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/collision/CollisionHelper.hpp>

class cClickableElement : public cStaticElement
{
protected:
    std::vector<tRectangle> patClickableRegions;

public:
    cClickableElement(const tElementInfo& tInfo, cTexture* pTexture);

    bool DidClick(glm::vec2 tClickPos);
};

cClickableElement::cClickableElement(const tElementInfo& tInfo, cTexture* pTexture) : cStaticElement(tInfo, pTexture)
{}

bool cClickableElement::DidClick(glm::vec2 tClickPos)
{
    for (tRectangle& tRegion : patClickableRegions)
    {
        if (cCollisionHelper::CollidesWithPoint(GetScreenEstate(), tClickPos))
        {
            return true;
        }
    }
    return false;
}
