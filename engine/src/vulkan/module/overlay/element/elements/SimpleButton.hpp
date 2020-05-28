#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/element/elements/ClickableElement.hpp>

class cSimpleButton : public cClickableElement
{
public:
    cSimpleButton(const tElementInfo& tInfo, cTexture* pTexture);
};

cSimpleButton::cSimpleButton(const tElementInfo& tInfo, cTexture* pTexture) : cClickableElement(tInfo, pTexture)
{
    tRectangle tRectangle = {};
    tRectangle.aVertices[0] = glm::vec2(0, 0);
    tRectangle.aVertices[1] = glm::vec2(0, (float) tInfo.uiHeight);
    tRectangle.aVertices[2] = glm::vec2((float) tInfo.uiWidth, (float) tInfo.uiHeight);
    tRectangle.aVertices[3] = glm::vec2((float) tInfo.uiWidth, 0);
    patClickableRegions.emplace_back(tRectangle);
}
