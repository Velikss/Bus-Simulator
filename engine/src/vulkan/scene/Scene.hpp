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
#include <vulkan/AudioHandler.hpp>
#include <vulkan/entities/cBehaviourHandler.hpp>

class cScene : public iInputHandler, public iTickTask
{
private:
    bool bQuit = false;

protected:
    Camera* poCamera = new FirstPersonFlyCamera;

    std::map<string, cTexture*> pmpTextures;
    std::map<string, cGeometry*> pmpGeometries;
    std::map<string, cMesh*> pmpMeshes;

    std::map<string, cBaseObject*> pmpObjects;

    bool paKeys[GLFW_KEY_LAST] = {false};

    cColliderSet* ppColliders = new cColliderSet();

    cAudioHandler* ppAudioHandler = nullptr;

private:
    std::vector<cBaseObject*> papMovableObjects;
    std::vector<cLightObject*> papLightObjects;

public:
    float pfAmbientLight = 0.2f;

    cBehaviourHandler* pcbSeperation = nullptr;
    cBehaviourHandler* pcbCohesion = nullptr;
    cBehaviourHandler* pcbSeeking = nullptr;

    cScene();
    virtual ~cScene();

    void Tick() override;
    virtual void Update();

    uint GetObjectCount();
    std::map<string, cBaseObject*>& GetObjects();
    std::vector<cBaseObject*>& GetMovableObjects();
    std::vector<cLightObject*>& GetLightObjects();

    Camera& GetCamera();
    Camera** GetCameraRef();

    bool ShouldQuit();

    virtual void Load(cTextureHandler* pTextureHandler,
                      cLogicalDevice* pLogicalDevice,
                      cAudioHandler* pAudioHandler = nullptr);
    virtual void Unload();
    void UnloadObjects();

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
    void HandleKey(uint uiKeyCode, uint uiAction) override;
    void HandleScroll(double dOffsetX, double dOffsetY) override;
    void HandleCharacter(char cCharacter) override;

    virtual void OnInputDisable();
    void HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction) override;

    virtual void AfterLoad();
protected:
    void Quit();
};

cScene::cScene()
{
}

cScene::~cScene()
{
    delete ppColliders;
    delete poCamera;

    UnloadObjects();

    ENGINE_LOG("Cleaned up scene");
}

void cScene::Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice, cAudioHandler* pAudioHandler)
{
    this->ppAudioHandler = pAudioHandler;

    for (auto oTexture : pmpTextures)
    {
        assert(oTexture.second != nullptr);
    }
    ENGINE_LOG("Loaded " << pmpTextures.size() << " textures");

    for (auto oGeometry : pmpGeometries)
    {
        assert(oGeometry.second != nullptr);
    }
    ENGINE_LOG("Loaded " << pmpGeometries.size() << " geometries");

    for (auto oMesh : pmpMeshes)
    {
        assert(oMesh.second != nullptr);
    }
    ENGINE_LOG("Loaded " << pmpMeshes.size() << " meshes");

    for (auto oObject : pmpObjects)
    {
        assert(oObject.second != nullptr);

        // If the object isn't static, add it to the list of movable objects
        if (!oObject.second->IsStatic())
        {
            papMovableObjects.push_back(oObject.second);
        }

        // If the object is a light source, add it to the list of light sources
        if (instanceof<cLightObject>(oObject.second))
        {
            papLightObjects.push_back(dynamic_cast<cLightObject*>(oObject.second));
        }

        // If the object is a collider, set it up and add it to the collider set
        cCollider* pCollider = oObject.second->GetCollider();
        if (pCollider != nullptr)
        {
            pCollider->Update(oObject.second->GetModelMatrix());
            ppColliders->papColliders.push_back(pCollider);
        }

        oObject.second->Setup(ppColliders);
    }
    ENGINE_LOG("Loaded " << pmpObjects.size() << " objects, of which " << papMovableObjects.size()
                         << " are movable and " << papLightObjects.size() << " are lights");
}

void cScene::Unload()
{
    UnloadObjects();

    papLightObjects.clear();
    papMovableObjects.clear();
    ppColliders->papColliders.clear();
    poCamera->Reset();
}

void cScene::UnloadObjects()
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
    return (uint) pmpObjects.size();
}

std::map<string, cBaseObject*>& cScene::GetObjects()
{
    return pmpObjects;
}

std::vector<cBaseObject*>& cScene::GetMovableObjects()
{
    return papMovableObjects;
}

std::vector<cLightObject*>& cScene::GetLightObjects()
{
    return papLightObjects;
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

void cScene::HandleCharacter(char cCharacter)
{

}

void cScene::HandleMouseButton(uint uiButton, double dXPos, double dYPos, int iAction)
{

}

Camera** cScene::GetCameraRef()
{
    return &poCamera;
}

void cScene::OnInputDisable()
{
    for (uint uiIndex = 0; uiIndex < sizeof(paKeys) / sizeof(paKeys[0]); uiIndex++)
    {
        paKeys[uiIndex] = false;
    }
}

void cScene::AfterLoad()
{
}
