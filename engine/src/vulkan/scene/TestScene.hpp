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

        // object
        /*pmpObjects["sphere"] = new cBaseObject(pmpMeshes["sphere"]);
        pmpObjects["sphere"]->setScale(glm::vec3(2, 2, 2));*/

        // plane
        pmpObjects["plane"] = new cBaseObject(pmpMeshes["plane"]);
        pmpObjects["plane"]->setScale(glm::vec3(200, 2, 200));

        // moon
        pmpObjects["moon"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(0, 1, 0), 5.0f);
        pmpObjects["moon"]->setScale(glm::vec3(0.1, 0.1, 0.1));
        pmpObjects["moon"]->setPosition(glm::vec3(5, 2, 0));

        // object
        pmpObjects["light2"] = new cLightObject(pmpMeshes["sphere"], glm::vec3(0, 1, 0), 5.0f);
        pmpObjects["light2"]->setScale(glm::vec3(0.1, 0.1, 0.1));
        pmpObjects["light2"]->setPosition(glm::vec3(-5, 2, 0));
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
