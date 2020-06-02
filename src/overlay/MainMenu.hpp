#pragma once

#include <pch.hpp>
#include <vulkan/module/overlay/OverlayWindow.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/module/overlay/OverlayRenderModule.hpp>
#include <vulkan/module/overlay/element/elements/SimpleButton.hpp>
#include <vulkan/module/overlay/element/elements/TextBoxElement.hpp>
#include <vulkan/module/overlay/element/elements/PasswordTextBox.hpp>

class cMainMenu : public cOverlayWindow
{
private:
    iOverlayProvider* ppOverlayProvider;
protected:
    void LoadTextures(cTextureHandler* pTextureHandler)
    {
        pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
        pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
        pmpTextures["buttonTexture"] = pTextureHandler->LoadTextureFromFile("resources/textures/button.jpg");
        pmpTextures["background"] = pTextureHandler->LoadTextureFromFile("resources/textures/background.jpg");
    }

    void ConstructElements()
    {
        pmpOverlay["oPanel"] = new cStaticElement({1, 1}, pmpTextures["background"]);

        pmpOverlay["oPanel"]->SetScale({(uint)((double)WIDTH * 0.85), (uint)((double)HEIGHT * 0.85)});
        Center(pmpOverlay["oPanel"]);

        pmpOverlay["oUserName"] = new cTextBoxElement({400, 120}, 2, pmpTextures["roof"],
                                                     cOverlayRenderModule::FONT, 13,
                                                     glm::vec3(1, 1, 0));
        pmpOverlay["oUserName"]->SetPosition(glm::vec2(0, 500));

        pmpOverlay["oPassword"] = new cPasswordTextBox({300, 80}, 2, pmpTextures["roof"],
                                                      cOverlayRenderModule::FONT, 13,
                                                      glm::vec3(1, 1, 0));
        pmpOverlay["oPassword"]->SetPosition(glm::vec2(500, 500));

        pmpOverlay["oSubmit"] = new cSimpleButton({250, 75}, pmpTextures["buttonTexture"]);
        pmpOverlay["oSubmit"]->SetPosition({0, 100});
    }

    void Center(cUIElement* poElem);
    void CenterHorizontal(cUIElement* poElem);
    void CenterVertical(cUIElement* poElem);

public:
    cMainMenu(iOverlayProvider* pOverlayProvider) : ppOverlayProvider(pOverlayProvider)
    {
    }

    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
    {
        if (((cSimpleButton*)pmpOverlay["oSubmit"])->DidClick({dXPos, dYPos}) && iAction == GLFW_PRESS) HandleOnSubmit();
        cOverlayWindow::HandleMouseButton(uiButton, dXPos, dYPos, iAction);
    }

    void Tick()
    {
    }

    void HandleOnSubmit();

    bool ShouldHandleInput() override
    {
        return true;
    }
};

void cMainMenu::HandleOnSubmit()
{
    std::cout << "submit" << std::endl;
}

void cMainMenu::Center(cUIElement* poElem)
{
    CenterHorizontal(poElem);
    CenterVertical(poElem);
}

void cMainMenu::CenterHorizontal(cUIElement* poElem)
{
    glm::vec2 oPosition = poElem->GetPosition();
    glm::vec2 oSize = poElem->GetScale();
    poElem->SetPosition({(((float)WIDTH - oSize.x) / 2), oPosition.y});
}

void cMainMenu::CenterVertical(cUIElement* poElem)
{
    glm::vec2 oPosition = poElem->GetPosition();
    glm::vec2 oSize = poElem->GetScale();
    poElem->SetPosition({oPosition.x, (((float)HEIGHT - oSize.y) / 2), });
}
