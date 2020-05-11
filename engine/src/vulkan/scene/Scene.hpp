#pragma once

#include <pch.hpp>
#include <scenes/Camera.hpp>
#include <vulkan/mesh/Mesh.hpp>
#include <vulkan/geometry/Geometry.hpp>
#include <vulkan/scene/BaseObject.hpp>
#include <vulkan/texture/TextureHandler.hpp>
#include <vulkan/scene/InputHandler.hpp>

class cScene : public iInputHandler
{
private:
    bool bQuit;

protected:
    Camera poCamera;

    std::map<string, cTexture*> pmpTextures;
    std::map<string, cGeometry*> pmpGeometries;
    std::map<string, cMesh*> pmpMeshes;
    std::map<string, cModel*> pmpModels;

    std::map<string, cBaseObject*> pmpObjects;

    bool paKeys[GLFW_KEY_LAST] = {false};

public:
    glm::vec3 color = glm::vec3(0, 1, 0);
    cGeometry* pQuadsGeometry;

    cScene();
    virtual ~cScene();

    virtual void Update();

    uint GetObjectCount();
    std::map<string, cBaseObject*>& GetObjects();

    Camera& GetCamera();

    bool ShouldQuit();

    virtual void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice);

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;

protected:
    void Quit();
};

cScene::cScene()
{
}

cScene::~cScene()
{
    for (auto oModel : pmpModels)
    {
        delete oModel.second;
    }

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
    pQuadsGeometry = cGeometry::DisplayQuads(pLogicalDevice);

    for (auto oModel : pmpModels)
    {
        assert(oModel.second != nullptr);
    }

    for (auto oObject : pmpObjects)
    {
        assert(oObject.second != nullptr);
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

void cScene::Update()
{
    poCamera.ProcessUpdates();
}

uint cScene::GetObjectCount()
{
    return pmpObjects.size();
}

std::map<string, cBaseObject*>& cScene::GetObjects()
{
    return pmpObjects;
}

Camera& cScene::GetCamera()
{
    return poCamera;
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
    poCamera.LookMouseDiff(uiDeltaX, uiDeltaY);
}

void cScene::HandleKey(uint uiKeyCode, uint uiAction)
{
    if (uiKeyCode != 0)
    {
        if (uiAction == GLFW_PRESS) paKeys[uiKeyCode] = true;
        if (uiAction == GLFW_RELEASE) paKeys[uiKeyCode] = false;
    }
}
