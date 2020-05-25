#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include "LightObject.hpp"

class cTestScene : public cScene
{
protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice) override
    {
        // textures
        pmpTextures["street"] = pTextureHandler->LoadTextureFromFile("resources/textures/street.jpg");
        pmpTextures["moon"] = pTextureHandler->LoadTextureFromFile("resources/textures/moon.jpg");

        // geometry
        pmpGeometries["box"] = cGeometry::FromOBJFile("resources/geometries/box.obj", pLogicalDevice);

        // mesh
        pmpMeshes["box1"] = new cMesh(pmpGeometries["box"], pmpTextures["street"]);
        pmpMeshes["box2"] = new cMesh(pmpGeometries["box"], pmpTextures["moon"]);

        pmpObjects["box1"] = new cBaseObject(pmpMeshes["box1"], cCollider::UnitCollider(0.5f), false);
        pmpObjects["box1"]->SetPosition(glm::vec3(2.1, 0, 0));
        pmpObjects["box1"]->pbLighting = false;

        pmpObjects["box2"] = new cBaseObject(pmpMeshes["box2"], cCollider::UnitCollider(0.5f), false);
        pmpObjects["box2"]->SetPosition(glm::vec3(1, 0, 0));
        pmpObjects["box2"]->SetRotation(glm::vec3(0, 45, 0));
        pmpObjects["box2"]->pbLighting = false;

        cScene::Load(pTextureHandler, pLogicalDevice);
    }

public:
    void Update() override
    {
        if (paKeys[GLFW_KEY_W])
            poCamera->Forward();
        if (paKeys[GLFW_KEY_S])
            poCamera->BackWard();
        if (paKeys[GLFW_KEY_A])
            poCamera->MoveLeft();
        if (paKeys[GLFW_KEY_D])
            poCamera->MoveRight();
        if (paKeys[GLFW_KEY_SPACE])
            poCamera->cameraHeight += 0.1;
        if (paKeys[GLFW_KEY_LEFT_SHIFT])
            poCamera->cameraHeight -= 0.1;

        if (paKeys[GLFW_KEY_ESCAPE])
            Quit();

        glm::vec3 pos = poCamera->GetPosition();
        pos.y -= 2;
        pmpObjects["box2"]->SetPosition(pos);
        glm::vec3 newPos = pmpObjects["box2"]->GetPosition();
        poCamera->SetPosition(newPos);

        cBaseObject* pObject = pmpObjects["box2"];
        glm::mat4 matrix = pObject->GetModelMatrix();
        ENGINE_LOG(ppColliders->Collides(pObject->ppCollider, matrix));

        cScene::Update();
    }
};
