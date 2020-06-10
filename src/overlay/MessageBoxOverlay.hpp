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
    enum class cButtonComposition
    {
        eNone,
        eOk
    };

    cMessageBoxOverlay(iGameManager* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {
    }

protected:
    void LoadTextures(cTextureHandler* pTextureHandler) override
    {
        pmpTextures["buttonTexture"] = pTextureHandler->LoadFromFile("resources/textures/button.png");
        pmpTextures["background"] = pTextureHandler->LoadFromFile("resources/textures/background.png");
    }

    void ConstructElements() override
    {
        cStaticElement* oPanel = new cStaticElement({100, 100}, pmpTextures["background"]);
        oPanel->Center();
        pmpOverlay.emplace_back("Background", oPanel);

        cTextElement* oTitleLabel = new cTextElement();
        oTitleLabel->SetFont(5, cOverlayRenderModule::FONT,
                             glm::vec3(1,1,1));
        pmpOverlay.emplace_back("Title", oTitleLabel);

        cTextElement* oTextLabel = new cTextElement();
        oTextLabel->SetFont(8, cOverlayRenderModule::FONT,
                             glm::vec3(1,1,1));
        pmpOverlay.emplace_back("Text", oTextLabel);

        cButton* oOk = new cButton({100, 30}, 0, pmpTextures["buttonTexture"], pmpTextures["buttonTexture"], cOverlayRenderModule::FONT, 5,
                                   glm::vec3(0, 0, 0));
        oOk->SetLabel("Ok");
        pmpOverlay.emplace_back("Ok", oOk);

        Update();
    }

    void Tick() override
    {
    }

    bool ShouldHandleInput() override
    {
        return true;
    }

    void HandleMouse(double dDeltaX, double dDeltaY) override;
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

    void OnOpen() override
    {
        cWindow::SetMouseLocked(false);
    }

    void OnClose() override
    {
        cWindow::SetMouseLocked(true);
    }
    void Update();
public:
    void Show(const string& sMessage, const string& sTitle = "", cButtonComposition eButtons = cButtonComposition::eNone, std::function<void(cButton*)> fButton1Click = {});
    void Close();
};

void cMessageBoxOverlay::Show(const string& sMessage, const string& sTitle, cButtonComposition eButtons, std::function<void(cButton*)> fButton1Click)
{
    cTextElement* oTitle = (cTextElement*)GetElement("Title");
    cTextElement* oText = (cTextElement*)GetElement("Text");
    cButton* oOk = (cButton*)GetElement("Ok");
    oOk->ppaCallbacks.clear();
    oOk->ppaCallbacks.push_back(fButton1Click);

    oTitle->UpdateText(sTitle);
    oText->UpdateText(sMessage);
    if (eButtons == cButtonComposition::eOk)
        oOk->Show();
    else
        oOk->Hide();

    Update();
    ppOverlayProvider->ActivateOverlayWindow(this);
}

void cMessageBoxOverlay::Close()
{
    ppOverlayProvider->DeactivateOverlayWindow();
}

void cMessageBoxOverlay::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{
    auto oClickPoint = glm::vec2{dXPos, dYPos};
    if (iAction == GLFW_PRESS)
    {
        cUIElement* oWindow = GetElement("Background");
        //TODO: collison excl. sub-components.
        if (cCollisionHelper::CollidesWithPoint(oWindow->GetScreenEstate(), oClickPoint))
            pbMovingWindow = true;
    }
    else
        pbMovingWindow = false;

    cButton* oOk = (cButton*)GetElement("Ok");
    if (cCollisionHelper::CollidesWithPoint(oOk->GetScreenEstate(), oClickPoint) && iAction == GLFW_PRESS)
    {
        oOk->Click();
    }
}

void cMessageBoxOverlay::HandleMouse(double dDeltaX, double dDeltaY)
{
    if(pbMovingWindow)
    {
        cUIElement* oWindow = GetElement("Background");
        auto oPosition = oWindow->GetPosition();
        oPosition.x += (dDeltaX / 2.5);
        oPosition.y += (dDeltaY / 2.5);
        oWindow->SetPosition(oPosition);
        Update();
    }
}

void cMessageBoxOverlay::Update()
{
    cUIElement* oWindow = GetElement("Background");
    cUIElement* oTitle = GetElement("Title");
    cUIElement* oText = GetElement("Text");
    cUIElement* oOk = GetElement("Ok");

    auto oTitleSize = oTitle->GetSize();
    auto oTextSize = oText->GetSize();
    auto oOkSize = oOk->GetSize();
    auto oOldWindowSize = oWindow->GetSize();
    auto oOldWindowPosition = oWindow->GetPosition();

    uint uiMaxWidth = (oTextSize.uiWidth > oTitleSize.uiWidth ? oTextSize.uiWidth : oTitleSize.uiWidth)
                      + 20;
    if(uiMaxWidth < oOkSize.uiWidth) uiMaxWidth = oOkSize.uiWidth;
    uint uiMaxHeight = oTextSize.uiHeight + oTitleSize.uiHeight + ((oOkSize.uiHeight > 0) ? oOkSize.uiHeight + 20 : 0)
                      + 25;
    oWindow->SetScale({(uiMaxWidth / 100.0f), (uiMaxHeight / 100.0f)});

    oOldWindowPosition.x -= (((float)uiMaxWidth - (float)oOldWindowSize.uiWidth)) / 2;
    oOldWindowPosition.y -= (((float)uiMaxHeight - (float)oOldWindowSize.uiHeight)) / 2;
    oWindow->SetPosition(oOldWindowPosition);

    auto oWindowEstate = oWindow->GetScreenEstate();
    oTitle->SetPosition({((oWindowEstate.aVertices[0].x +
                                    ((oWindowEstate.aVertices[2].x -
                                      oWindowEstate.aVertices[0].x) / 2))) - (oTitleSize.uiWidth / 2),
                                      oWindowEstate.aVertices[0].y + 5});
    oText->SetPosition({((oWindowEstate.aVertices[0].x +
                                   ((oWindowEstate.aVertices[2].x -
                                     oWindowEstate.aVertices[0].x) / 2))) - (oTextSize.uiWidth / 2),
                                     oWindowEstate.aVertices[0].y + 35});
    oOk->SetPosition({((oWindowEstate.aVertices[0].x +
                                 ((oWindowEstate.aVertices[2].x -
                                   oWindowEstate.aVertices[0].x) / 2))) - (oOkSize.uiWidth / 2),
                                oWindowEstate.aVertices[0].y + oTextSize.uiHeight + oTitleSize.uiHeight + 35});
}
