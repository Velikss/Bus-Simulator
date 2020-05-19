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
        pmpTextures["moon"] = pTextureHandler->LoadTextureFromFile("resources/textures/moon.jpg");
        pmpTextures["street"] = pTextureHandler->LoadTextureFromFile("resources/textures/street.jpg");

        // geometry
        pmpGeometries["sphere"] = cGeometry::FromOBJFile("resources/geometries/sphere.obj", pLogicalDevice);
        pmpGeometries["plane"] = cGeometry::FromOBJFile("resources/geometries/plane.obj", pLogicalDevice);

        // mesh
        pmpMeshes["sphere"] = new cMesh(pmpGeometries["sphere"], pmpTextures["moon"]);
        pmpMeshes["plane"] = new cMesh(pmpGeometries["plane"], pmpTextures["street"]);

        pmpObjects["sphere"] = new cBaseObject(pmpMeshes["sphere"]);

        pmpObjects["light1"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(0, 1, 0), 10.0f);
        pmpObjects["light1"]->SetScale(glm::vec3(0, 0, 0));
        pmpObjects["light1"]->SetPosition(glm::vec3(5, 5, 5));

        pmpObjects["light2"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(1, 0, 1), 10.0f);
        pmpObjects["light2"]->SetScale(glm::vec3(0, 0, 0));
        pmpObjects["light2"]->SetPosition(glm::vec3(-5, 5, 5));

        pmpObjects["light3"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(1, 0, 0), 10.0f);
        pmpObjects["light3"]->SetScale(glm::vec3(0, 0, 0));
        pmpObjects["light3"]->SetPosition(glm::vec3(5, 5, -5));

        pmpObjects["light4"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(0, 1, 1), 10.0f);
        pmpObjects["light4"]->SetScale(glm::vec3(0, 0, 0));
        pmpObjects["light4"]->SetPosition(glm::vec3(-5, 5, -5));
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

        cScene::Update();
    }
};
