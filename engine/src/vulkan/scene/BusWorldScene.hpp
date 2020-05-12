#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/scene/BusCamera.hpp>

class cBusWorldScene : public cScene
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

void cBusWorldScene::Load(cTextureHandler *pTextureHandler, cLogicalDevice *pLogicalDevice)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pLogicalDevice);
    LoadMeshes();
    LoadModels();
    LoadObjects();
}

void cBusWorldScene::Update()
{
    if (paKeys[GLFW_KEY_W])
        BusCentered ? pmpObjects["bus"]->MoveForward() : poCamera->Forward();
    if (paKeys[GLFW_KEY_S])
        BusCentered ? pmpObjects["bus"]->MoveBackward() : poCamera->BackWard();
    if (paKeys[GLFW_KEY_A])
        BusCentered ? pmpObjects["bus"]->MoveLeft(0.02) : poCamera->MoveLeft();
    if (paKeys[GLFW_KEY_D])
        BusCentered ? pmpObjects["bus"]->MoveRight(0.02) : poCamera->MoveRight();
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
        poCamera->cameraPivotChanges = glm::vec3(5.0f, 5.0f, 0.0f);
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

void cBusWorldScene::HandleScroll(double dOffsetX, double dOffsetY)
{
    poCamera->LookMouseWheelDiff((float) dOffsetX, (float) dOffsetY);
}

void cBusWorldScene::LoadTextures(cTextureHandler *pTextureHandler)
{
    pmpTextures["roof"] = pTextureHandler->LoadTextureFromFile("resources/textures/roof.jpg");
    pmpTextures["stoneHouse"] = pTextureHandler->LoadTextureFromFile("resources/textures/stone.jpg");
    pmpTextures["grass"] = pTextureHandler->LoadTextureFromFile("resources/textures/grass.jpg");
    pmpTextures["street"] = pTextureHandler->LoadTextureFromFile("resources/textures/street.jpg");
    pmpTextures["moon"] = pTextureHandler->LoadTextureFromFile("resources/textures/moon.jpg");
    pmpTextures["skybox"] = pTextureHandler->LoadTextureFromFile("resources/textures/skybox.jpg");
    pmpTextures["grey"] = pTextureHandler->LoadTextureFromFile("resources/textures/uvtemplate.bmp");
    pmpTextures["building"] = pTextureHandler->LoadTextureFromFile("resources/textures/building.jpg");
    pmpTextures["road"] = pTextureHandler->LoadTextureFromFile("resources/textures/road.jpg");
}

void cBusWorldScene::LoadGeometries(cLogicalDevice *pLogicalDevice)
{
    // roads
    pmpGeometries["road30-10"] = cGeometry::FromOBJFile("resources/geometries/streets/Road30-10.obj", pLogicalDevice);
    pmpGeometries["twoWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/TwoWayCrossing.obj", pLogicalDevice);
    pmpGeometries["threeWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/ThreeWayCrossing.obj", pLogicalDevice);
    pmpGeometries["fourWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/FourWayCrossing.obj", pLogicalDevice);
    pmpGeometries["cornerRoad"] = cGeometry::FromOBJFile("resources/geometries/streets/CornerRoad.obj", pLogicalDevice);
    pmpGeometries["walkways36-3WithCorners"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway36-3WithCorners.obj", pLogicalDevice, 10, 10);
    pmpGeometries["walkways30-3"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway30-3.obj", pLogicalDevice, 10, 10);

    pmpGeometries["skybox"] = cGeometry::FromOBJFile("resources/geometries/skybox.obj", pLogicalDevice);

    // buses
    pmpGeometries["bus"] = cGeometry::FromOBJFile("resources/geometries/Busses/SchoolBus.obj", pLogicalDevice);
    pmpGeometries["busStation"] = cGeometry::FromOBJFile("resources/geometries/BusStation.obj", pLogicalDevice);
    pmpGeometries["building"] = cGeometry::FromOBJFile("resources/geometries/buildingTest.obj", pLogicalDevice, 8, 8);
}

void cBusWorldScene::LoadMeshes()
{
    pmpMeshes["road30-10"] = new cMesh(pmpGeometries["road30-10"], pmpTextures["road"]);
    pmpMeshes["threeWayCrossing"] = new cMesh(pmpGeometries["threeWayCrossing"], pmpTextures["street"]);
    pmpMeshes["walkways36-3WithCorners"] = new cMesh(pmpGeometries["walkways36-3WithCorners"], pmpTextures["stoneHouse"]);
    pmpMeshes["walkways30-3"] = new cMesh(pmpGeometries["walkways30-3"], pmpTextures["stoneHouse"]);
    pmpMeshes["skybox"] = new cMesh(pmpGeometries["skybox"], pmpTextures["skybox"]);
    pmpMeshes["bus"] = new cMesh(pmpGeometries["bus"], pmpTextures["grey"]);
    pmpMeshes["busStation"] = new cMesh(pmpGeometries["busStation"], pmpTextures["grey"]);
    pmpMeshes["building"] = new cMesh(pmpGeometries["building"], pmpTextures["building"]);
}

void cBusWorldScene::LoadModels()
{
    pmpModels["road30-10"] = new cModel(pmpMeshes["road30-10"]);
    pmpModels["threeWayCrossing"] = new cModel(pmpMeshes["threeWayCrossing"]);
    pmpModels["walkways36-3WithCorners"] = new cModel(pmpMeshes["walkways36-3WithCorners"]);
    pmpModels["walkways30-3"] = new cModel(pmpMeshes["walkways30-3"]);

    pmpModels["bus"] = new cModel(pmpMeshes["bus"]);
    pmpModels["busStation"] = new cModel(pmpMeshes["busStation"]);

    pmpModels["building"] = new cModel(pmpMeshes["building"]);
}

void cBusWorldScene::LoadObjects()
{
    // straight roads
    pmpObjects["road30-10_1"] = new cBaseObject(pmpMeshes["road30-10"]);

    pmpObjects["road30-10_2"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_2"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_2"]->setPosition(glm::vec3(-5.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_3"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_3"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_3"]->setPosition(glm::vec3(45.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_4"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_4"]->setPosition(glm::vec3(0.0f, 0.0f, -50.0f));

    pmpObjects["road30-10_5"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_5"]->setPosition(glm::vec3(50.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_6"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_6"]->setPosition(glm::vec3(50.0f, 0.0f, -50.0f));

    pmpObjects["road30-10_7"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_7"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_7"]->setPosition(glm::vec3(95.0f, 0.0f, -15.0f));

    // three way crossings
    pmpObjects["threeWayCrossing1"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing1"]->setRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing1"]->setPosition(glm::vec3(35.0f, 0.0f, -15.0f));

    pmpObjects["threeWayCrossing2"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing2"]->setRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing2"]->setPosition(glm::vec3(-15.0f, 0.0f, -15.0f));

    pmpObjects["threeWayCrossing3"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing3"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["threeWayCrossing3"]->setPosition(glm::vec3(45.0f, 0.0f, -45.0f));

    pmpObjects["threeWayCrossing4"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing4"]->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing4"]->setPosition(glm::vec3(0.0f, 0.0f, -60.0f));

    pmpObjects["threeWayCrossing5"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing5"]->setPosition(glm::vec3(80.0f, 0.0f, 0.0f));

    pmpObjects["threeWayCrossing6"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing6"]->setPosition(glm::vec3(80.0f, 0.0f, -50.0f));

    // skybox
    pmpObjects["skybox"] = new cBaseObject(pmpMeshes["skybox"]);
    pmpObjects["skybox"]->setScale(glm::vec3(500, 500, 500));
    pmpObjects["skybox"]->setPosition(glm::vec3(0, -250, 0));

    // Walkways
    pmpObjects["walkways36-3WithCorners1"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners1"]->setPosition(glm::vec3(-5.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners2"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners2"]->setPosition(glm::vec3(-5.0f, 0.0f, -47.0f));

    pmpObjects["walkways30-3_1"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_1"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_1"]->setPosition(glm::vec3(35.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_2"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_2"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_2"]->setPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    // Bus stations
    pmpObjects["busStation1"] = new cBaseObject(*pmpModels["busStation"]);
    pmpObjects["busStation1"]->setPosition(glm::vec3(8.603f, 0.15f, -11.0f));

    pmpObjects["building"] = new cBaseObject(*pmpModels["building"]);
    pmpObjects["building"]->setPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["bus"] = new cBaseObject(*pmpModels["bus"]);
    pmpObjects["bus"]->setPosition(glm::vec3(12.5f, 0, -7.5f));
    pmpObjects["bus"]->setRotation(glm::vec3(0.0f, 90.0, 0.0f));
    pmpObjects["bus"]->setScale(glm::vec3(0.8, 0.8, 0.8));
}
