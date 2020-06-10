#pragma once
#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/util/GameManager.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>

class cMessageBoxOverlay : public cOverlayWindow
{
private:
    iGameManager* ppOverlayProvider;
    bool pbMovingWindow = false;
public:
    cMessageBoxOverlay(iGameManager* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {
    }

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["background"] = pTextureHandler->LoadFromFile("resources/textures/background.png");
    }

    void ConstructElements() override
    {
        cStaticElement* oPanel = new cStaticElement({200, 90}, pmpTextures["background"]);
        oPanel->Center();
        pmpOverlay.push_back({"Background", oPanel});
    }

    void Tick() override
    {
    }

    bool ShouldHandleInput() override
    {
        return true;
    }

    void HandleMouse(double dDeltaX, double dDeltaY) override;
    void HandleCharacter(char cCharacter) override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

    void OnOpen() override
    {
        cWindow::SetMouseLocked(false);
    }

    void OnClose() override
    {
        cWindow::SetMouseLocked(true);
    }

public:
    void Show(const string& sMessage);
    void Close();
};

void cMessageBoxOverlay::Show(const string& sMessage)
{
    ppOverlayProvider->ActivateOverlayWindow(this);
}

void cMessageBoxOverlay::Close()
{
    ppOverlayProvider->DeactivateOverlayWindow();
}

void cMessageBoxOverlay::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    if (iAction == GLFW_PRESS)
    {
        cUIElement* oWindow = GetElement("Background");
        //TODO: collison excl. sub-components.
        if (cCollisionHelper::CollidesWithPoint(oWindow->GetScreenEstate(), {dXPos, dYPos}))
            pbMovingWindow = true;
    }
    else
        pbMovingWindow = false;
    cOverlayWindow::HandleMouseButton(uiButton, dXPos, dYPos, iAction);
}

void cMessageBoxOverlay::HandleCharacter(char cCharacter)
{
    cOverlayWindow::HandleCharacter(cCharacter);
}

void cMessageBoxOverlay::HandleMouse(double dDeltaX, double dDeltaY)
{
    if(pbMovingWindow)
    {
        //auto oDiffPosition = glm::vec2{uiDeltaX, uiDeltaY} - oWindow->GetPosition();
        cUIElement* oWindow = GetElement("Background");
        auto oPosition = oWindow->GetPosition();
        oPosition.x += dDeltaX;
        oPosition.y += dDeltaY;
        oWindow->SetPosition(oPosition);
    }
}
