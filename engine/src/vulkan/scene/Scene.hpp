#pragma once

#include <pch.hpp>
#include "Camera.hpp"
#include "LightObject.hpp"
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/scene/InputHandler.hpp>
#include <vulkan/loop/TickTask.hpp>
#include <vulkan/geometry/ViewportQuadGeometry.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>

class cScene : public iInputHandler, public iTickTask
{
private:
    bool bQuit;

protected:
    Camera* poCamera = new FirstPersonFlyCamera;

    std::map<string, cTexture*> pmpTextures;
    std::map<string, cGeometry*> pmpGeometries;
    std::map<string, cMesh*> pmpMeshes;

    std::map<string, cBaseObject*> pmpObjects;

    std::map<string, cStaticElement*> pmpOverlay;

    bool paKeys[GLFW_KEY_LAST] = {false};

private:
    std::vector<cBaseObject*> papMovableObjects;
    std::vector<cLightObject*> papLightObjects;

public:
    glm::vec3 textColor = glm::vec3(0, 1, 0);
    float pfAmbientLight = 0.2;

    cScene();
    virtual ~cScene();

    void Tick() override;
    virtual void Update();

    uint GetObjectCount();
    std::map<string, cBaseObject*>& GetObjects();
    std::map<string, cMesh*>& GetMeshes();
    std::vector<cBaseObject*>& GetMovableObjects();
    std::vector<cLightObject*>& GetLightObjects();
    std::map<string, cStaticElement*> GetOverlay();

    Camera& GetCamera();

    bool ShouldQuit();

    virtual void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice);

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;
    void HandleScroll(double dOffsetX, double dOffsetY) override;

protected:
    void Quit();
};

cScene::cScene()
{
}

cScene::~cScene()
{
    for (auto oObject : pmpObjects)
    {
        delete oObject.second;
    }

    for (auto oMesh : pmpMeshes)
    {
        delete oMesh.second;
    }

    for (auto oGeometry : pmpGeometries)
    {
        delete oGeometry.second;
    }

    for (auto oTexture : pmpTextures)
    {
        delete oTexture.second;
    }
}

void cScene::Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice)
{
    for (auto oObject : pmpObjects)
    {
        assert(oObject.second != nullptr);

        if (!oObject.second->IsStatic())
        {
            papMovableObjects.push_back(oObject.second);
        }

        if (instanceof<cLightObject>(oObject.second))
        {
            papLightObjects.push_back(dynamic_cast<cLightObject*>(oObject.second));
        }
    }

    for (auto oMesh : pmpMeshes)
    {
        assert(oMesh.second != nullptr);
    }

    for (auto oGeometry : pmpGeometries)
    {
        assert(oGeometry.second != nullptr);
    }

    for (auto oTexture : pmpTextures)
    {
        assert(oTexture.second != nullptr);
    }
}

void cScene::Tick()
{
    Update();
}

void cScene::Update()
{
    poCamera->ProcessUpdates();
}

uint cScene::GetObjectCount()
{
    return pmpObjects.size();
}

std::map<string, cBaseObject*>& cScene::GetObjects()
{
    return pmpObjects;
}

std::map<string, cMesh*>& cScene::GetMeshes()
{
    return pmpMeshes;
}

std::vector<cBaseObject*>& cScene::GetMovableObjects()
{
    return papMovableObjects;
}

std::vector<cLightObject*>& cScene::GetLightObjects()
{
    return papLightObjects;
}

std::map<string, cStaticElement*> cScene::GetOverlay()
{
    return pmpOverlay;
}

Camera& cScene::GetCamera()
{
    return *poCamera;
}

void cScene::Quit()
{
    bQuit = true;
}

bool cScene::ShouldQuit()
{
    return bQuit;
}

void cScene::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{
    poCamera->LookMouseDiff(uiDeltaX, uiDeltaY);
}

void cScene::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (uiKeyCode != 0)
    {
        if (uiAction == GLFW_PRESS) paKeys[uiKeyCode] = true;
        if (uiAction == GLFW_RELEASE) paKeys[uiKeyCode] = false;
    }
}

void cScene::HandleScroll(double dOffsetX, double dOffsetY)
{

}
