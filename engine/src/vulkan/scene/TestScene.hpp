#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>

class cTestScene : public cScene
{
protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice) override
    {
        // texture
        pmpTextures["chalet"] = pTextureHandler->LoadTextureFromFile("resources/chalet.jpg");

        // geometry
        pmpGeometries["chalet"] = cGeometry::FromOBJFile("resources/chalet.obj", pLogicalDevice);

        // mesh
        pmpMeshes["chalet"] = new cMesh(pmpGeometries["chalet"], pmpTextures["chalet"]);

        // object
        pmpObjects["chalet"] = new cBaseObject(pmpMeshes["chalet"]);
        pmpObjects["chalet"]->setScale(glm::vec3(8, 8, 8));
        pmpObjects["chalet"]->setRotation(glm::vec3(270, 0, 0));
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

        if (paKeys[GLFW_KEY_ESCAPE])
            Quit();

        cScene::Update();
    }
};
