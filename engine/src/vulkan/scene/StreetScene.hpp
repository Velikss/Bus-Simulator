#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/scene/BusCamera.hpp>

class cStreetScene : public cScene
{
public:
    void Update() override;

    void HandleScroll(double dOffsetX, double dOffsetY) override;

protected:
    void Load(cTextureHandler *pTextureHandler, cLogicalDevice *pLogicalDevice) override;

private:
    void LoadTextures(cTextureHandler *pTextureHandler);

    void LoadGeometries(cLogicalDevice *pLogicalDevice);

    void LoadMeshes();

    void LoadModels();

    void LoadObjects();

    bool BusCentered = false;
};

void cStreetScene::Load(cTextureHandler *pTextureHandler, cLogicalDevice *pLogicalDevice)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pLogicalDevice);
    LoadMeshes();
    LoadModels();
    LoadObjects();
}

void cStreetScene::Update()
{
    if (paKeys[GLFW_KEY_W])
        BusCentered ? pmpObjects["bus"]->MoveForward() : poCamera->Forward();
    if (paKeys[GLFW_KEY_S])
        BusCentered ? pmpObjects["bus"]->MoveBackward() : poCamera->BackWard();
    if (paKeys[GLFW_KEY_A])
        BusCentered ? pmpObjects["bus"]->MoveLeft(0.3) : poCamera->MoveLeft();
    if (paKeys[GLFW_KEY_D])
        BusCentered ? pmpObjects["bus"]->MoveRight(0.3) : poCamera->MoveRight();
    if (paKeys[GLFW_KEY_C])
    {
        BusCentered = false;
        poCamera = new FirstPersonFlyCamera;
    }
    if (paKeys[GLFW_KEY_B])
    {
        BusCentered = true;
        poCamera = new BusCamera;
        poCamera->cameraPivot = pmpObjects["bus"]->getPosition();
        poCamera->cameraHeight = 15.0f;
        poCamera->cameraPivotChanges = glm::vec3(0.0f, 5.0f, 0.0f);
    }

    // temporary flight controls
    if (paKeys[GLFW_KEY_SPACE])
        poCamera->cameraHeight += 0.01;
    if (paKeys[GLFW_KEY_LEFT_SHIFT])
        poCamera->cameraHeight -= 0.01;

    if (paKeys[GLFW_KEY_ESCAPE])
        Quit();

    cScene::Update();
}

void cStreetScene::HandleScroll(double dOffsetX, double dOffsetY)
{
    poCamera->LookMouseWheelDiff((float) dOffsetX, (float) dOffsetY);
}

void cStreetScene::LoadTextures(cTextureHandler *pTextureHandler)
{
    pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
    pmpTextures["stoneHouse"] = pTextureHandler->LoadTextureFromFile("resources/textures/stone.jpg");
    pmpTextures["grass"] = pTextureHandler->LoadTextureFromFile("resources/textures/grass.jpg");
    pmpTextures["street"] = pTextureHandler->LoadTextureFromFile("resources/textures/street.jpg");
    pmpTextures["moon"] = pTextureHandler->LoadTextureFromFile("resources/textures/moon.jpg");
    pmpTextures["skybox"] = pTextureHandler->LoadTextureFromFile("resources/textures/skybox.jpg");
    pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/grey.jpg");
}

void cStreetScene::LoadGeometries(cLogicalDevice *pLogicalDevice)
{
    pmpGeometries["tree"] = cGeometry::FromOBJFile("resources/geometries/tree.obj", pLogicalDevice);
    pmpGeometries["plane"] = cGeometry::FromOBJFile("resources/geometries/plane.obj", pLogicalDevice);
    pmpGeometries["cube"] = cGeometry::FromOBJFile("resources/geometries/box.obj", pLogicalDevice);
    pmpGeometries["sphere"] = cGeometry::FromOBJFile("resources/geometries/sphere.obj", pLogicalDevice);
    pmpGeometries["cylinder"] = cGeometry::FromOBJFile("resources/geometries/cylinder32.obj", pLogicalDevice);
    pmpGeometries["skybox"] = cGeometry::FromOBJFile("resources/geometries/skybox.obj", pLogicalDevice);
    pmpGeometries["bus"] = cGeometry::FromOBJFile("resources/geometries/American_School_Bus.obj", pLogicalDevice);

    pmpGeometries["houseBase"] = cGeometry::FromOBJFile("resources/geometries/houseBase.obj", pLogicalDevice, 3);
    pmpGeometries["roof"] = cGeometry::FromOBJFile("resources/geometries/roof.obj", pLogicalDevice, 5);
    pmpGeometries["street"] = cGeometry::FromOBJFile("resources/geometries/plane.obj", pLogicalDevice, 10);
    pmpGeometries["grasslawn"] = cGeometry::FromOBJFile("resources/geometries/plane.obj", pLogicalDevice, 10);
}

void cStreetScene::LoadMeshes()
{
    pmpMeshes["moon"] = new cMesh(pmpGeometries["sphere"], pmpTextures["moon"]);
    pmpMeshes["houseBase"] = new cMesh(pmpGeometries["houseBase"], pmpTextures["stoneHouse"]);
    pmpMeshes["roof"] = new cMesh(pmpGeometries["roof"], pmpTextures["roof"]);
    pmpMeshes["street"] = new cMesh(pmpGeometries["street"], pmpTextures["street"]);
    pmpMeshes["grasslawn"] = new cMesh(pmpGeometries["grasslawn"], pmpTextures["grass"]);
    pmpMeshes["parkLight"] = new cMesh(pmpGeometries["cube"], pmpTextures["stoneHouse"]);
    pmpMeshes["poleLight"] = new cMesh(pmpGeometries["sphere"], pmpTextures["stoneHouse"]);
    pmpMeshes["tree"] = new cMesh(pmpGeometries["tree"], pmpTextures["grass"]);
    pmpMeshes["pole"] = new cMesh(pmpGeometries["cylinder"], pmpTextures["stoneHouse"]);
    pmpMeshes["skybox"] = new cMesh(pmpGeometries["skybox"], pmpTextures["skybox"]);
    pmpMeshes["bus"] = new cMesh(pmpGeometries["bus"], pmpTextures["grey"]);
}

void cStreetScene::LoadModels()
{
    pmpModels["street"] = new cModel(pmpMeshes["street"]);
    pmpModels["street"]->setScale(glm::vec3(40, 1, 20));

    pmpModels["grasslawn"] = new cModel(pmpMeshes["grasslawn"]);
    pmpModels["grasslawn"]->setScale(glm::vec3(40, 1, 20));

    pmpModels["tree"] = new cModel(pmpMeshes["tree"]);
    pmpModels["tree"]->setScale(glm::vec3(0.2, 0.2, 0.2));

    pmpModels["houseBase"] = new cModel(pmpMeshes["houseBase"]);
    pmpModels["houseBase"]->setPosition(glm::vec3(0, 4.5, 0));

    pmpModels["roof"] = new cModel(pmpMeshes["roof"]);
    pmpModels["roof"]->setPosition(glm::vec3(0, 12.5, 0));

    pmpModels["bus"] = new cModel(pmpMeshes["bus"]);
}

void cStreetScene::LoadObjects()
{
    pmpObjects["skybox"] = new cBaseObject(pmpMeshes["skybox"]);
    pmpObjects["skybox"]->setScale(glm::vec3(500, 500, 500));
    pmpObjects["skybox"]->setPosition(glm::vec3(0, -250, 0));

    pmpObjects["moon"] = new cBaseObject(pmpMeshes["moon"]);
    pmpObjects["moon"]->setScale(glm::vec3(2, 2, 2));

    pmpObjects["street"] = new cBaseObject(*pmpModels["street"]);
    pmpObjects["street"]->setScale(glm::vec3(200, 1, 100));

    pmpObjects["bus"] = new cBaseObject(*pmpModels["bus"]);
    pmpObjects["bus"]->setScale(glm::vec3(1.5, 1.5, 1.5));
    pmpObjects["bus"]->setPosition(glm::vec3(30, 0, 15));
    pmpObjects["bus"]->setRotation(glm::vec3(270, 0, 0));

    /*pmpObjects["grasslawn1"] = new cBaseObject(*pmpModels["grasslawn"]);
    pmpObjects["grasslawn1"]->setPosition(glm::vec3(80.0, 0.01, 0));

    pmpObjects["grasslawn2"] = new cBaseObject(*pmpModels["grasslawn"]);
    pmpObjects["grasslawn2"]->setPosition(glm::vec3(0, 0.01, 0));
    pmpObjects["grasslawn2"]->setScale(glm::vec3(80, 1, 20));

    pmpObjects["grasslawn3"] = new cBaseObject(*pmpModels["grasslawn"]);
    pmpObjects["grasslawn3"]->setPosition(glm::vec3(-80.0, 0.01, 0));*/

    for (int i = 0; i < 20; i++)
    {
        string sName = "tree" + std::to_string(i);
        pmpObjects[sName] = new cBaseObject(*pmpModels["tree"]);
        pmpObjects[sName]->setPosition(glm::vec3(-95 + 10 * i, 0, -45));
    }

    for (int i = 0; i < 8; i++)
    {
        string sHouseName = "house" + std::to_string(i);
        pmpObjects[sHouseName] = new cBaseObject(*pmpModels["houseBase"]);
        pmpObjects[sHouseName]->setPosition(glm::vec3(-80 + (25 * i), 0, 43.35));
        string sRoofName = "roof" + std::to_string(i);
        pmpObjects[sRoofName] = new cBaseObject(*pmpModels["roof"]);
        pmpObjects[sRoofName]->setPosition(glm::vec3(-80 + (25 * i), 5.5, 43.35));
    }

    for (int i = 0; i <= 18; i += 3)
    {
        string sPoleName = "pole" + std::to_string(i);
        pmpObjects[sPoleName] = new cBaseObject(pmpMeshes["pole"]);
        pmpObjects[sPoleName]->setScale(glm::vec3(0.1, 6, 0.1));
        pmpObjects[sPoleName]->setPosition(glm::vec3(-90 + 10 * i, 0, 30));
        string sLightName = "light" + std::to_string(i);
        pmpObjects[sLightName] = new cBaseObject(pmpMeshes["poleLight"]);
        pmpObjects[sLightName]->setScale(glm::vec3(0.3, 0.3, 0.3));
        pmpObjects[sLightName]->setPosition(glm::vec3(-90 + 10 * i, 6, 30));
    }
}
