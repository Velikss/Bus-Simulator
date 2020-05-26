#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/scene/BusCamera.hpp>
#include <multiplayer/cMultiplayerHandler.hpp>
#include <vulkan/entities/IPassengerHolder.hpp>
#include <vulkan/entities/cBus.hpp>
#include <vulkan/entities/cBusStop.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/entities/cEntityGroup.hpp>

class cBusWorldScene : public cScene
{
public:
    void Update() override;

    void HandleScroll(double dOffsetX, double dOffsetY) override;

protected:
    void Load(cTextureHandler *pTextureHandler, cLogicalDevice *pLogicalDevice) override;

private:
    cNetworkConnection::tNetworkInitializationSettings tConnectNetworkSettings;
    cMultiplayerHandler *poMultiplayerHandler = nullptr;

    ~cBusWorldScene()
    {
        if (poMultiplayerHandler) delete poMultiplayerHandler;
    }

    void LoadTextures(cTextureHandler *pTextureHandler);

    void LoadGeometries(cLogicalDevice *pLogicalDevice);

    void LoadMeshes();

    void LoadObjects();

    bool BusCentered = false;

    cEntityGroup entityGroup;
    cEntityGroup entityGroup2;

    BusCamera *pBusCamera = new BusCamera;
    FirstPersonFlyCamera *pFirstPersonFlyCamera = new FirstPersonFlyCamera;
};

void cBusWorldScene::Load(cTextureHandler *pTextureHandler, cLogicalDevice *pLogicalDevice)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pLogicalDevice);
    LoadMeshes();
    LoadObjects();

    // Connect to multiplayer instance if possbile.
    tConnectNetworkSettings.sAddress = "51.68.34.201";
    tConnectNetworkSettings.usPort = 8080;
    tConnectNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;

    poMultiplayerHandler = new cMultiplayerHandler(&tConnectNetworkSettings, this);
    if (poMultiplayerHandler->Start())
    {
        std::cout << "multiplayer connected." << std::endl;
    } else
    {
        std::cout << "multiplayer failed to connect." << std::endl;
        delete poMultiplayerHandler;
        poMultiplayerHandler = nullptr;
    }

    cScene::Load(pTextureHandler, pLogicalDevice);
}

void cBusWorldScene::Update()
{
    entityGroup.UpdateEntities();

    if (paKeys[GLFW_KEY_Q])
        dynamic_cast<cEntity *>(pmpObjects["entity3"])->SetTarget(
                dynamic_cast<cBus *>(pmpObjects["bus"])->GetDoorPosition());
    if (paKeys[GLFW_KEY_E])
    {
        for (auto &entity : *entityGroup.GetEntities())
        {
            dynamic_cast<cEntity *>(entity)->SetTarget(dynamic_cast<cBus *>(pmpObjects["bus"])->GetDoorPosition());
        }
    }
    if (paKeys[GLFW_KEY_T])
        dynamic_cast<cEntity *>(pmpObjects["entity"])->SetPosition(glm::vec3(5, 5, 5));
    if (paKeys[GLFW_KEY_W])
        BusCentered ? dynamic_cast<cBus *>(pmpObjects["bus"])->Accelerate() : poCamera->Forward();
    if (paKeys[GLFW_KEY_S])
        BusCentered ? dynamic_cast<cBus *>(pmpObjects["bus"])->Decelerate() : poCamera->BackWard();
    if (!paKeys[GLFW_KEY_W] && !paKeys[GLFW_KEY_S])
        if (BusCentered) dynamic_cast<cBus *>(pmpObjects["bus"])->IdleAcceleration();
    if (!paKeys[GLFW_KEY_A] && !paKeys[GLFW_KEY_D])
        if (BusCentered) dynamic_cast<cBus *>(pmpObjects["bus"])->IdleSteering();
    if (paKeys[GLFW_KEY_A])
        BusCentered ? dynamic_cast<cBus *>(pmpObjects["bus"])->Steer("left") : poCamera->MoveLeft();
    if (paKeys[GLFW_KEY_D])
        BusCentered ? dynamic_cast<cBus *>(pmpObjects["bus"])->Steer("right") : poCamera->MoveRight();
    if (paKeys[GLFW_KEY_C])
    {
        BusCentered = false;
        poCamera = pFirstPersonFlyCamera;
    }

    if (paKeys[GLFW_KEY_B])
    {
        BusCentered = true;
        poCamera = pBusCamera;
        poCamera->cameraPivotObject = pmpObjects["bus"];
        poCamera->cameraPivotPos = pmpObjects["bus"]->GetPosition();
        poCamera->cameraHeight = 10.0f;
        poCamera->cameraPivotChanges = glm::vec3(2.0f, 0.5f, 0.0f);
    }

    // temporary flight controls
    if (paKeys[GLFW_KEY_SPACE])
        poCamera->MoveUp();
    if (paKeys[GLFW_KEY_LEFT_SHIFT])
        poCamera->MoveDown();

    if (paKeys[GLFW_KEY_ESCAPE])
        Quit();

    dynamic_cast<cBus *>(pmpObjects["bus"])->Move();

    cScene::Update();
    if (poMultiplayerHandler) poMultiplayerHandler->PushData();
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
    // buildings
    pmpTextures["building"] = pTextureHandler->LoadTextureFromFile("resources/textures/buildings/building.jpg");
    pmpTextures["building2"] = pTextureHandler->LoadTextureFromFile("resources/textures/buildings/building2.jpg");
    pmpTextures["needle"] = pTextureHandler->LoadTextureFromFile("resources/textures/buildings/needle.jpg");
    //street
    pmpTextures["road"] = pTextureHandler->LoadTextureFromFile("resources/textures/streets/road.png");
    pmpTextures["threeWayCrossing"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/streets/threeWayCrossing.png");
    pmpTextures["fourWayCrossing"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/streets/fourWayCrossing.png");
    // streetUtil
    pmpTextures["trafficLight"] = pTextureHandler->LoadTextureFromFile(
            "resources/textures/streetUtil/trafficLight.png");
    pmpTextures["busStop"] = pTextureHandler->LoadTextureFromFile("resources/textures/streetUtil/busStop.png");
    // buses
    pmpTextures["schoolBus"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/schoolBus.png");

    // passengers
    pmpTextures["passenger"] = pTextureHandler->LoadTextureFromFile("resources/textures/penguin.png");
}

void cBusWorldScene::LoadGeometries(cLogicalDevice *pLogicalDevice)
{
    // skybox
    pmpGeometries["skybox"] = cGeometry::FromOBJFile("resources/geometries/skybox.obj", pLogicalDevice);
    // streets
    pmpGeometries["road30-10"] = cGeometry::FromOBJFile("resources/geometries/streets/Road30-10.obj", pLogicalDevice);
    pmpGeometries["twoWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/TwoWayCrossing.obj",
                                                             pLogicalDevice);
    pmpGeometries["threeWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/ThreeWayCrossing.obj",
                                                               pLogicalDevice);
    pmpGeometries["fourWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/FourWayCrossing.obj",
                                                              pLogicalDevice);
    pmpGeometries["cornerRoad"] = cGeometry::FromOBJFile("resources/geometries/streets/CornerRoad.obj", pLogicalDevice);
    // walkways
    pmpGeometries["walkways36-3WithCorners"] = cGeometry::FromOBJFile(
            "resources/geometries/walkways/Walkway36-3WithCorners.obj", pLogicalDevice, 10, 10);
    pmpGeometries["walkways30-3"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway30-3.obj",
                                                           pLogicalDevice, 10, 10);
    pmpGeometries["walkways10-3"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway10-3.obj",
                                                           pLogicalDevice, 10, 10);
    // streetUtil
    pmpGeometries["busStation"] = cGeometry::FromOBJFile("resources/geometries/streetUtil/busStop.obj", pLogicalDevice);
    pmpGeometries["trafficLight"] = cGeometry::FromOBJFile("resources/geometries/streetUtil/trafficLight.obj",
                                                           pLogicalDevice);
    // streetDeco

    // buses
    pmpGeometries["bus"] = cGeometry::FromOBJFile("resources/geometries/busses/SchoolBus.obj", pLogicalDevice);
    // buildings
    pmpGeometries["building"] = cGeometry::FromOBJFile("resources/geometries/buildingTest.obj", pLogicalDevice, 8, 8);
    pmpGeometries["needleBuilding"] = cGeometry::FromOBJFile("resources/geometries/buildings/needleBuilding.obj",
                                                             pLogicalDevice, 8, 8);
    pmpGeometries["blockBuilding2"] = cGeometry::FromOBJFile("resources/geometries/buildings/blockBuilding2.obj",
                                                             pLogicalDevice, 8, 8);
    pmpGeometries["blockBuilding3"] = cGeometry::FromOBJFile("resources/geometries/buildings/blockBuilding3.obj",
                                                             pLogicalDevice, 5, 5);
    pmpGeometries["blockBuilding4"] = cGeometry::FromOBJFile("resources/geometries/buildings/blockBuilding4.obj",
                                                             pLogicalDevice, 8, 8);

    // grass
    pmpGeometries["grassField1"] = cGeometry::FromOBJFile("resources/geometries/grassField1.obj", pLogicalDevice, 8, 8);
    // penguin
    pmpGeometries["passenger"] = cGeometry::FromOBJFile("resources/geometries/penguin.obj", pLogicalDevice);
}

void cBusWorldScene::LoadMeshes()
{
    //skybox
    pmpMeshes["skybox"] = new cMesh(pmpGeometries["skybox"], pmpTextures["skybox"]);
    // streets
    pmpMeshes["road30-10"] = new cMesh(pmpGeometries["road30-10"], pmpTextures["road"]);
    pmpMeshes["twoWayCrossing"] = new cMesh(pmpGeometries["twoWayCrossing"], pmpTextures["road"]);
    pmpMeshes["threeWayCrossing"] = new cMesh(pmpGeometries["threeWayCrossing"], pmpTextures["threeWayCrossing"]);
    // walkways
    pmpMeshes["walkways36-3WithCorners"] = new cMesh(pmpGeometries["walkways36-3WithCorners"],
                                                     pmpTextures["stoneHouse"]);
    pmpMeshes["walkways30-3"] = new cMesh(pmpGeometries["walkways30-3"], pmpTextures["stoneHouse"]);
    pmpMeshes["walkways10-3"] = new cMesh(pmpGeometries["walkways10-3"], pmpTextures["stoneHouse"]);
    // buses
    pmpMeshes["bus"] = new cMesh(pmpGeometries["bus"], pmpTextures["schoolBus"]);
    // streetUtil
    pmpMeshes["busStation"] = new cMesh(pmpGeometries["busStation"], pmpTextures["busStop"]);
    pmpMeshes["trafficLight"] = new cMesh(pmpGeometries["trafficLight"], pmpTextures["trafficLight"]);
    // streetDeco

    // buildings
    pmpMeshes["building"] = new cMesh(pmpGeometries["building"], pmpTextures["building"]);
    pmpMeshes["needleBuilding"] = new cMesh(pmpGeometries["needleBuilding"], pmpTextures["needle"]);
    pmpMeshes["blockBuilding2"] = new cMesh(pmpGeometries["blockBuilding2"], pmpTextures["building2"]);
    pmpMeshes["blockBuilding3"] = new cMesh(pmpGeometries["blockBuilding3"], pmpTextures["building"]);
    pmpMeshes["blockBuilding4"] = new cMesh(pmpGeometries["blockBuilding4"], pmpTextures["needle"]);

    // grass
    pmpMeshes["grassField1"] = new cMesh(pmpGeometries["grassField1"], pmpTextures["grass"]);

    // passenger
    pmpMeshes["passenger"] = new cMesh(pmpGeometries["passenger"], pmpTextures["passenger"]);
}

void cBusWorldScene::LoadObjects()
{
    // skybox
    pmpObjects["skybox"] = new cBaseObject(pmpMeshes["skybox"]);
    pmpObjects["skybox"]->SetScale(glm::vec3(500, 500, 500));
    pmpObjects["skybox"]->SetPosition(glm::vec3(0, -250, 0));

    // straight roads
    pmpObjects["road30-10_1"] = new cBaseObject(pmpMeshes["road30-10"]);

    pmpObjects["road30-10_2"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_2"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_2"]->SetPosition(glm::vec3(-5.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_3"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_3"]->SetPosition(glm::vec3(45.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_4"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_4"]->SetPosition(glm::vec3(0.0f, 0.0f, -50.0f));

    pmpObjects["road30-10_5"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_5"]->SetPosition(glm::vec3(50.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_6"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_6"]->SetPosition(glm::vec3(50.0f, 0.0f, -50.0f));

    pmpObjects["road30-10_7"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_7"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_7"]->SetPosition(glm::vec3(95.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_8"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_8"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_8"]->SetPosition(glm::vec3(95.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_9"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_9"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_9"]->SetPosition(glm::vec3(95.0f, 0.0f, -115.0f));

    pmpObjects["road30-10_10"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_10"]->SetPosition(glm::vec3(50.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_11"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_11"]->SetPosition(glm::vec3(0.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_12"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_12"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_12"]->SetPosition(glm::vec3(-5.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_13"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_13"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_13"]->SetPosition(glm::vec3(-5.0f, 0.0f, -115.0f));

    pmpObjects["road30-10_14"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_14"]->SetPosition(glm::vec3(-50.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_15"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_15"]->SetPosition(glm::vec3(-100.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_16"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_16"]->SetPosition(glm::vec3(-150.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_17"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_17"]->SetPosition(glm::vec3(-50.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_18"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_18"]->SetPosition(glm::vec3(-100.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_19"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_19"]->SetPosition(glm::vec3(-150.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_20"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_20"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_20"]->SetPosition(glm::vec3(-155.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_21"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_21"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_21"]->SetPosition(glm::vec3(-155.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_22"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_22"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_22"]->SetPosition(glm::vec3(-155.0f, 0.0f, -115.0f));

    // two way crossing
    pmpObjects["twoWayCrossing1"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing1"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing1"]->SetPosition(glm::vec3(95.0f, 0.0f, -95.0));

    pmpObjects["twoWayCrossing2"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing2"]->SetPosition(glm::vec3(30.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing3"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing3"]->SetPosition(glm::vec3(-5.0f, 0.0f, -95.0));

    pmpObjects["twoWayCrossing4"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing4"]->SetPosition(glm::vec3(-70.0f, 0.0f, 0.0));

    pmpObjects["twoWayCrossing5"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing5"]->SetPosition(glm::vec3(-120.0f, 0.0f, 0.0));

    pmpObjects["twoWayCrossing6"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing6"]->SetPosition(glm::vec3(-70.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing7"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing7"]->SetPosition(glm::vec3(-120.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing8"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing8"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing8"]->SetPosition(glm::vec3(-155.0f, 0.0f, -45.0));

    pmpObjects["twoWayCrossing9"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing9"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing9"]->SetPosition(glm::vec3(-155.0f, 0.0f, -95.0));

    // three way crossings
    pmpObjects["threeWayCrossing1"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing1"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing1"]->SetPosition(glm::vec3(35.0f, 0.0f, -15.0f));

    pmpObjects["threeWayCrossing2"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing2"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing2"]->SetPosition(glm::vec3(-15.0f, 0.0f, -15.0f));

    pmpObjects["threeWayCrossing3"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["threeWayCrossing3"]->SetPosition(glm::vec3(45.0f, 0.0f, -45.0f));

    pmpObjects["threeWayCrossing4"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing4"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing4"]->SetPosition(glm::vec3(0.0f, 0.0f, -60.0f));

    pmpObjects["threeWayCrossing5"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing5"]->SetPosition(glm::vec3(80.0f, 0.0f, 0.0f));

    pmpObjects["threeWayCrossing6"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing6"]->SetPosition(glm::vec3(80.0f, 0.0f, -50.0f));

    pmpObjects["threeWayCrossing7"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing7"]->SetPosition(glm::vec3(80.0f, 0.0f, -150.0f));

    pmpObjects["threeWayCrossing8"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing8"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["threeWayCrossing8"]->SetPosition(glm::vec3(-5.0f, 0.0f, -145.0f));

    pmpObjects["threeWayCrossing9"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing9"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing9"]->SetPosition(glm::vec3(-150.0f, 0.0f, -10.0f));

    pmpObjects["threeWayCrossing10"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing10"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing10"]->SetPosition(glm::vec3(-150.0f, 0.0f, -160.0f));

    // Walkways
    pmpObjects["walkways36-3WithCorners1"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners1"]->SetPosition(glm::vec3(-5.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners2"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners2"]->SetPosition(glm::vec3(-5.0f, 0.0f, -47.0f));

    pmpObjects["walkways36-3WithCorners3"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners3"]->SetPosition(glm::vec3(45.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners4"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners4"]->SetPosition(glm::vec3(45.0f, 0.0f, -47.0f));

    for (int i = 0; i < 6; i++)
    {
        pmpObjects["walkways36-3WithCorners" + std::to_string(i + 5)] = new cBaseObject(
                pmpMeshes["walkways36-3WithCorners"]);
        pmpObjects["walkways36-3WithCorners" + std::to_string(i + 5)]->SetPosition(
                glm::vec3(45.0f - (40 * i), 0.0f, 3.0f));
    }

    pmpObjects["walkways36-3WithCorners11"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners11"]->SetPosition(glm::vec3(-55.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners12"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners12"]->SetPosition(glm::vec3(-105.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners13"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners13"]->SetPosition(glm::vec3(-155.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners14"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners14"]->SetPosition(glm::vec3(-55.0f, 0.0f, -147.0f));

    pmpObjects["walkways36-3WithCorners15"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners15"]->SetPosition(glm::vec3(-105.0f, 0.0f, -147.0f));

    pmpObjects["walkways36-3WithCorners16"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners16"]->SetPosition(glm::vec3(-155.0f, 0.0f, -147.0f));

    pmpObjects["walkways36-3WithCorners17"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners17"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners17"]->SetPosition(glm::vec3(-15.0f, 0.0f, -57.0f));

    pmpObjects["walkways36-3WithCorners18"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners18"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners18"]->SetPosition(glm::vec3(-15.0f, 0.0f, -107.0f));

    pmpObjects["walkways36-3WithCorners19"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners19"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners19"]->SetPosition(glm::vec3(-152.0f, 0.0f, -57.0f));

    pmpObjects["walkways36-3WithCorners20"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners20"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners20"]->SetPosition(glm::vec3(-152.0f, 0.0f, -107.0f));

    pmpObjects["walkways36-3WithCorners21"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners21"]->SetPosition(glm::vec3(-5.0f, 0.0f, -60.0f));

    pmpObjects["walkways36-3WithCorners22"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners22"]->SetPosition(glm::vec3(45.0f, 0.0f, -60.0f));

    pmpObjects["walkways36-3WithCorners23"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners23"]->SetPosition(glm::vec3(-5.0f, 0.0f, -147.0f));

    pmpObjects["walkways36-3WithCorners24"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners24"]->SetPosition(glm::vec3(45.0f, 0.0f, -147.0f));

    pmpObjects["walkways36-3WithCorners25"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners25"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners25"]->SetPosition(glm::vec3(-2.0f, 0.0f, -107.0f));

    pmpObjects["walkways36-3WithCorners26"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners26"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners26"]->SetPosition(glm::vec3(85.0f, 0.0f, -107.0f));

    pmpObjects["walkways30-3_1"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_1"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_1"]->SetPosition(glm::vec3(35.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_2"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_2"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_2"]->SetPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_3"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_3"]->SetPosition(glm::vec3(48.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_4"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_4"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_4"]->SetPosition(glm::vec3(85.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_5"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_5"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_5"]->SetPosition(glm::vec3(-15.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_6"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_6"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_6"]->SetPosition(glm::vec3(-152.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_7"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_7"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_7"]->SetPosition(glm::vec3(-2.0f, 0.0f, -63.0f));

    pmpObjects["walkways30-3_8"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_8"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_8"]->SetPosition(glm::vec3(85.0f, 0.0f, -63.0f));

    pmpObjects["walkways10-3_1"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_1"]->SetPosition(glm::vec3(-65.0f, 0.0f, -10.0f));

    pmpObjects["walkways10-3_2"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_2"]->SetPosition(glm::vec3(-115.0f, 0.0f, -10.0f));

    pmpObjects["walkways10-3_3"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_3"]->SetPosition(glm::vec3(-65.0f, 0.0f, -147.0f));

    pmpObjects["walkways10-3_4"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_4"]->SetPosition(glm::vec3(-115.0f, 0.0f, -147.0f));

    pmpObjects["walkways10-3_5"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_5"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_5"]->SetPosition(glm::vec3(-15.0f, 0.0f, -47.0f));

    pmpObjects["walkways10-3_6"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_6"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_6"]->SetPosition(glm::vec3(-15.0f, 0.0f, -97.0f));

    pmpObjects["walkways10-3_7"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_7"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_7"]->SetPosition(glm::vec3(-152.0f, 0.0f, -47.0f));

    pmpObjects["walkways10-3_8"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_8"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_8"]->SetPosition(glm::vec3(-152.0f, 0.0f, -97.0f));

    pmpObjects["walkways10-3_9"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_9"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_9"]->SetPosition(glm::vec3(-2.0f, 0.0f, -97.0f));

    pmpObjects["walkways10-3_10"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_10"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_10"]->SetPosition(glm::vec3(85.0f, 0.0f, -97.0f));

    pmpObjects["walkways10-3_11"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_11"]->SetPosition(glm::vec3(35.0f, 0.0f, -60.0f));

    pmpObjects["walkways10-3_12"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_12"]->SetPosition(glm::vec3(35.0f, 0.0f, -147.0f));

    // Bus stations
    pmpObjects["busStation1"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation1"]->SetPosition(glm::vec3(8.603f, 0.15f, -11.0f));

    pmpObjects["busStation2"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation2"]->SetPosition(glm::vec3(-140.0f, 0.15f, -11.0f));

    pmpObjects["busStation3"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation3"]->SetPosition(glm::vec3(-16.0f, 0.15f, -97.0f));
    pmpObjects["busStation3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));

    pmpObjects["busStation4"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation4"]->SetPosition(glm::vec3(66.0f, 0.15f, -61.0f));

    pmpObjects["busStation5"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation5"]->SetPosition(glm::vec3(40.0f, 0.15f, -149.0f));
    pmpObjects["busStation5"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));

    // Traffic lights
    pmpObjects["trafficLight1"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50,
                                                   cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight1"]->SetPosition(glm::vec3(34.0f, 0.15f, 2.0f));

    pmpObjects["trafficLight2"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50,
                                                   cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight2"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight2"]->SetPosition(glm::vec3(33.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight3"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50,
                                                   cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight3"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight3"]->SetPosition(glm::vec3(46.0f, 0.15f, -12.0f));

    pmpObjects["trafficLight4"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight4"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight4"]->SetPosition(glm::vec3(-4.0f, 0.15f, -12.0f));

    pmpObjects["trafficLight5"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight5"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight5"]->SetPosition(glm::vec3(-17.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight6"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight6"]->SetPosition(glm::vec3(-16.0f, 0.15f, 2.0f));

    pmpObjects["trafficLight7"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight7"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight7"]->SetPosition(glm::vec3(-17.0f, 0.15f, -61.0f));

    pmpObjects["trafficLight8"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight8"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight8"]->SetPosition(glm::vec3(-4.0f, 0.15f, -62.0f));

    pmpObjects["trafficLight9"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight9"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight9"]->SetPosition(glm::vec3(-3.0f, 0.15f, -49.0f));

    pmpObjects["trafficLight10"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight10"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight10"]->SetPosition(glm::vec3(46.0f, 0.15f, -62.0f));

    pmpObjects["trafficLight11"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight11"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight11"]->SetPosition(glm::vec3(47.0f, 0.15f, -49.0f));

    pmpObjects["trafficLight12"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight12"]->SetPosition(glm::vec3(34.0f, 0.15f, -48.0f));

    // Buildings
    pmpObjects["building"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building"]->SetPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["needleBuilding"] = new cBaseObject(pmpMeshes["needleBuilding"], cCollider::UnitCollider(34));
    pmpObjects["needleBuilding"]->SetPosition(glm::vec3(48.0f, 0.0f, -13.0f));

    pmpObjects["blockBuilding2_1"] = new cBaseObject(pmpMeshes["blockBuilding2"], cCollider::UnitCollider(34));
    pmpObjects["blockBuilding2_1"]->SetPosition(glm::vec3(-2.0f, 0.0f, -63.0f));

    pmpObjects["blockBuilding2_2"] = new cBaseObject(pmpMeshes["blockBuilding2"], cCollider::UnitCollider(34));
    pmpObjects["blockBuilding2_2"]->SetPosition(glm::vec3(48.0f, 0.0f, -113.0f));

    pmpObjects["blockBuilding3_1"] = new cBaseObject(pmpMeshes["blockBuilding3"], cCollider::RectangleCollider(34, 50));
    pmpObjects["blockBuilding3_1"]->SetPosition(glm::vec3(-2.0f, 0.0f, -97.0f));

    pmpObjects["blockBuilding3_2"] = new cBaseObject(pmpMeshes["blockBuilding3"], cCollider::RectangleCollider(34, 50));
    pmpObjects["blockBuilding3_2"]->SetPosition(glm::vec3(48.0f, 0.0f, -63.0f));

    pmpObjects["blockBuilding4_1"] = new cBaseObject(pmpMeshes["blockBuilding4"], cCollider::RectangleCollider(16, 84));
    pmpObjects["blockBuilding4_1"]->SetPosition(glm::vec3(32.0f, 0.0f, -63.0f));

    pmpObjects["bus"] = new cBus(pmpMeshes["bus"]);
    pmpObjects["bus"]->SetPosition(glm::vec3(12.5f, 0, -7.5f));
    pmpObjects["bus"]->SetRotation(glm::vec3(0.0f, 90.0, 0.0f));
    pmpObjects["bus"]->SetScale(glm::vec3(0.8, 0.8, 0.8));

    // Entities
    pmpObjects["entity"] = new cEntity(pmpMeshes["passenger"]);
    pmpObjects["entity"]->SetPosition(glm::vec3(10.0f, 0.15f, -11.0f));

    pmpObjects["entity2"] = new cEntity(pmpMeshes["passenger"]);
    pmpObjects["entity2"]->SetPosition(glm::vec3(11.0f, 0.15f, -10.5f));

    pmpObjects["entity3"] = new cEntity(pmpMeshes["passenger"]);
    pmpObjects["entity3"]->SetPosition(glm::vec3(14.0f, 0.15f, -11.0f));

    pmpObjects["entity4"] = new cEntity(pmpMeshes["passenger"]);
    pmpObjects["entity4"]->SetPosition(glm::vec3(13.0f, 0.15f, -10.5f));

    for (uint i = 0; i < 10; i++)
    {
        string key = "multiplayer_bus_" + std::to_string(i);
        pmpObjects[key] = new cBus(pmpMeshes["bus"]);
        pmpObjects[key]->SetScale(glm::vec3(0));
        dynamic_cast<cBus *>(pmpObjects[key])->piBusId = i;
    }

    // grass
    pmpObjects["grassField1_1"] = new cEntity(pmpMeshes["grassField1"]);
    pmpObjects["grassField1_1"]->SetPosition(glm::vec3(-152.0f, 0.0f, -13.0f));

    // Init behaviour handler
    cBehaviourHandler::Init();
    cBehaviourHandler::AddBehavioursFromDirectory("resources/scripting");


    cBehaviourHandler *cbSeperation = new cBehaviourHandler("seperation");
    cBehaviourHandler *cbCohesion = new cBehaviourHandler("cohesion");
    cBehaviourHandler *cbSeeking = new cBehaviourHandler("seeking");

    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity2"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity3"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity4"]));

    entityGroup2 = entityGroup;
    entityGroup.AddBehaviour(cbSeeking);
    entityGroup.AddBehaviour(cbCohesion);
    entityGroup.AddBehaviour(cbSeperation);
}
