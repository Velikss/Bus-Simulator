#pragma once
#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/scene/BusCamera.hpp>
#include <multiplayer/cMultiplayerHandler.hpp>
#include <vulkan/entities/cBus.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/entities/cEntityGroup.hpp>

class cBusWorldScene : public cScene
{
public:
    void Update() override;

    void HandleScroll(double dOffsetX, double dOffsetY) override;

protected:
    void Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice) override;

private:
    cNetworkConnection::tNetworkInitializationSettings tConnectNetworkSettings;
    cMultiplayerHandler* poMultiplayerHandler = nullptr;
    ~cBusWorldScene()
    {
        if (poMultiplayerHandler) delete poMultiplayerHandler;
    }

    void LoadTextures(cTextureHandler* pTextureHandler);

    void LoadGeometries(cLogicalDevice* pLogicalDevice);

    void LoadMeshes();

    void LoadModels();

    void LoadObjects();

    bool BusCentered = false;

    cEntityGroup entityGroup;
    cEntityGroup entityGroup2;

    BusCamera *pBusCamera = new BusCamera;
    FirstPersonFlyCamera *pFirstPersonFlyCamera = new FirstPersonFlyCamera;
};

void cBusWorldScene::Load(cTextureHandler* pTextureHandler, cLogicalDevice* pLogicalDevice)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pLogicalDevice);
    LoadMeshes();
    LoadModels();
    LoadObjects();

    // Connect to multiplayer instance if possbile.
    tConnectNetworkSettings.sAddress = "51.68.34.201";
    tConnectNetworkSettings.usPort = 8080;
    tConnectNetworkSettings.eMode = cNetworkConnection::cMode::eNonBlocking;

    poMultiplayerHandler = new cMultiplayerHandler(&tConnectNetworkSettings, this);
    if (poMultiplayerHandler->Start())
    {
        std::cout << "multiplayer connected." << std::endl;
    }
    else
    {
        std::cout << "multiplayer failed to connect." << std::endl;
        delete poMultiplayerHandler;
        poMultiplayerHandler = nullptr;
    }
}

void cBusWorldScene::Update()
{
    if(paKeys[GLFW_KEY_Q])
        entityGroup.UpdateEntities();
    if(paKeys[GLFW_KEY_Q])
        entityGroup2.UpdateEntities();
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
        poCamera->cameraPivotPos = *pmpObjects["bus"]->getPosition();
        poCamera->cameraHeight = 10.0f;
        poCamera->cameraPivotChanges = glm::vec3(2.0f, 0.5f, 0.0f);
    }

    // temporary flight controls
    if (paKeys[GLFW_KEY_SPACE])
        poCamera->MoveUp();
//        poCamera->cameraHeight += 0.01;
    if (paKeys[GLFW_KEY_LEFT_SHIFT])
        poCamera->MoveDown();
//        poCamera->cameraHeight -= 0.01;

    if (paKeys[GLFW_KEY_ESCAPE])
        Quit();

    dynamic_cast<cBus *>(pmpObjects["bus"])->Move();



    cScene::Update();
    if(poMultiplayerHandler) poMultiplayerHandler->PushData();
}

void cBusWorldScene::HandleScroll(double dOffsetX, double dOffsetY)
{
    poCamera->LookMouseWheelDiff((float) dOffsetX, (float) dOffsetY);
}

void cBusWorldScene::LoadTextures(cTextureHandler* pTextureHandler)
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
    pmpTextures["needle"] = pTextureHandler->LoadTextureFromFile("resources/textures/buildings/needle.jpg");
    //street
    pmpTextures["road"] = pTextureHandler->LoadTextureFromFile("resources/textures/streets/road.png");
    pmpTextures["threeWayCrossing"] = pTextureHandler->LoadTextureFromFile("resources/textures/streets/threeWayCrossing.png");
    pmpTextures["fourWayCrossing"] = pTextureHandler->LoadTextureFromFile("resources/textures/streets/fourWayCrossing.png");
    // streetUtil
    pmpTextures["trafficLight"] = pTextureHandler->LoadTextureFromFile("resources/textures/streetUtil/trafficLight.png");
    pmpTextures["busStop"] = pTextureHandler->LoadTextureFromFile("resources/textures/streetUtil/busStop.png");
    // buses
    pmpTextures["schoolBus"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/schoolBus.png");

    //entities
    pmpTextures["entity"] = pTextureHandler->LoadTextureFromFile("resources/textures/buses/schoolBus.png");
}

void cBusWorldScene::LoadGeometries(cLogicalDevice *pLogicalDevice)
{
    // skybox
    pmpGeometries["skybox"] = cGeometry::FromOBJFile("resources/geometries/skybox.obj", pLogicalDevice);
    // streets
    pmpGeometries["road30-10"] = cGeometry::FromOBJFile("resources/geometries/streets/Road30-10.obj", pLogicalDevice);
    pmpGeometries["twoWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/TwoWayCrossing.obj", pLogicalDevice);
    pmpGeometries["threeWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/ThreeWayCrossing.obj", pLogicalDevice);
    pmpGeometries["fourWayCrossing"] = cGeometry::FromOBJFile("resources/geometries/streets/FourWayCrossing.obj", pLogicalDevice);
    pmpGeometries["cornerRoad"] = cGeometry::FromOBJFile("resources/geometries/streets/CornerRoad.obj", pLogicalDevice);
    // walkways
    pmpGeometries["walkways36-3WithCorners"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway36-3WithCorners.obj", pLogicalDevice, 10, 10);
    pmpGeometries["walkways30-3"] = cGeometry::FromOBJFile("resources/geometries/walkways/Walkway30-3.obj", pLogicalDevice, 10, 10);
    // streetUtil
    pmpGeometries["busStation"] = cGeometry::FromOBJFile("resources/geometries/streetUtil/busStop.obj", pLogicalDevice);
    pmpGeometries["trafficLight"] = cGeometry::FromOBJFile("resources/geometries/streetUtil/trafficLight.obj", pLogicalDevice);
    // streetDeco

    // buses
    pmpGeometries["bus"] = cGeometry::FromOBJFile("resources/geometries/busses/SchoolBus.obj", pLogicalDevice);
    // buildings
    pmpGeometries["building"] = cGeometry::FromOBJFile("resources/geometries/buildingTest.obj", pLogicalDevice, 8, 8);
    pmpGeometries["needleBuilding"] = cGeometry::FromOBJFile("resources/geometries/buildings/needleBuilding.obj", pLogicalDevice, 8, 8);

    // entities
    pmpGeometries["entity"] = cGeometry::FromOBJFile("resources/geometries/busses/SchoolBus.obj", pLogicalDevice);
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
    pmpMeshes["walkways36-3WithCorners"] = new cMesh(pmpGeometries["walkways36-3WithCorners"], pmpTextures["stoneHouse"]);
    pmpMeshes["walkways30-3"] = new cMesh(pmpGeometries["walkways30-3"], pmpTextures["stoneHouse"]);
    // buses
    pmpMeshes["bus"] = new cMesh(pmpGeometries["bus"], pmpTextures["schoolBus"]);
    // streetUtil
    pmpMeshes["busStation"] = new cMesh(pmpGeometries["busStation"], pmpTextures["busStop"]);
    pmpMeshes["trafficLight"] = new cMesh(pmpGeometries["trafficLight"], pmpTextures["trafficLight"]);
    // streetDeco

    // buildings
    pmpMeshes["building"] = new cMesh(pmpGeometries["building"], pmpTextures["building"]);
    pmpMeshes["needleBuilding"] = new cMesh(pmpGeometries["needleBuilding"], pmpTextures["needle"]);

    // entities
    pmpMeshes["entity"] = new cMesh(pmpGeometries["entity"], pmpTextures["entity"]);
}

void cBusWorldScene::LoadModels()
{
    // streets
    pmpModels["road30-10"] = new cModel(pmpMeshes["road30-10"]);
    pmpModels["twoWayCrossing"] = new cModel(pmpMeshes["twoWayCrossing"]);
    pmpModels["threeWayCrossing"] = new cModel(pmpMeshes["threeWayCrossing"]);
    // walkways
    pmpModels["walkways36-3WithCorners"] = new cModel(pmpMeshes["walkways36-3WithCorners"]);
    pmpModels["walkways30-3"] = new cModel(pmpMeshes["walkways30-3"]);
    // buses
    pmpModels["bus"] = new cModel(pmpMeshes["bus"]);
    // streetUtil
    pmpModels["busStation"] = new cModel(pmpMeshes["busStation"]);
    pmpModels["trafficLight"] = new cModel(pmpMeshes["trafficLight"]);
    // streetDeco

    // buildings
    pmpModels["building"] = new cModel(pmpMeshes["building"]);
    pmpModels["needleBuilding"] = new cModel(pmpMeshes["needleBuilding"]);

    // entities
    pmpModels["entity"] = new cModel(pmpMeshes["entity"]);

}

void cBusWorldScene::LoadObjects()
{
    // skybox
    pmpObjects["skybox"] = new cBaseObject(pmpMeshes["skybox"]);
    pmpObjects["skybox"]->setScale(glm::vec3(500, 500, 500));
    pmpObjects["skybox"]->setPosition(glm::vec3(0, -250, 0));

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

    pmpObjects["road30-10_8"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_8"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_8"]->setPosition(glm::vec3(95.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_9"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_9"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_9"]->setPosition(glm::vec3(95.0f, 0.0f, -115.0f));

    pmpObjects["road30-10_10"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_10"]->setPosition(glm::vec3(50.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_11"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_11"]->setPosition(glm::vec3(0.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_12"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_12"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_12"]->setPosition(glm::vec3(-5.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_13"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_13"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_13"]->setPosition(glm::vec3(-5.0f, 0.0f, -115.0f));

    pmpObjects["road30-10_14"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_14"]->setPosition(glm::vec3(-50.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_15"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_15"]->setPosition(glm::vec3(-100.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_16"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_16"]->setPosition(glm::vec3(-150.0f, 0.0f, 0.0f));

    pmpObjects["road30-10_17"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_17"]->setPosition(glm::vec3(-50.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_18"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_18"]->setPosition(glm::vec3(-100.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_19"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_19"]->setPosition(glm::vec3(-150.0f, 0.0f, -150.0f));

    pmpObjects["road30-10_20"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_20"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_20"]->setPosition(glm::vec3(-155.0f, 0.0f, -15.0f));

    pmpObjects["road30-10_21"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_21"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_21"]->setPosition(glm::vec3(-155.0f, 0.0f, -65.0f));

    pmpObjects["road30-10_22"] = new cBaseObject(pmpMeshes["road30-10"]);
    pmpObjects["road30-10_22"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["road30-10_22"]->setPosition(glm::vec3(-155.0f, 0.0f, -115.0f));

    // two way crossing
    pmpObjects["twoWayCrossing1"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing1"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing1"]->setPosition(glm::vec3(95.0f, 0.0f, -95.0));

    pmpObjects["twoWayCrossing2"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing2"]->setPosition(glm::vec3(30.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing3"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing3"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing3"]->setPosition(glm::vec3(-5.0f, 0.0f, -95.0));

    pmpObjects["twoWayCrossing4"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing4"]->setPosition(glm::vec3(-70.0f, 0.0f, 0.0));

    pmpObjects["twoWayCrossing5"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing5"]->setPosition(glm::vec3(-120.0f, 0.0f, 0.0));

    pmpObjects["twoWayCrossing6"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing6"]->setPosition(glm::vec3(-70.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing7"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing7"]->setPosition(glm::vec3(-120.0f, 0.0f, -150.0));

    pmpObjects["twoWayCrossing8"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing8"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing8"]->setPosition(glm::vec3(-155.0f, 0.0f, -45.0));

    pmpObjects["twoWayCrossing9"] = new cBaseObject(pmpMeshes["twoWayCrossing"]);
    pmpObjects["twoWayCrossing9"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0));
    pmpObjects["twoWayCrossing9"]->setPosition(glm::vec3(-155.0f, 0.0f, -95.0));

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

    pmpObjects["threeWayCrossing7"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing7"]->setPosition(glm::vec3(80.0f, 0.0f, -150.0f));

    pmpObjects["threeWayCrossing8"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing8"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["threeWayCrossing8"]->setPosition(glm::vec3(-5.0f, 0.0f, -145.0f));

    pmpObjects["threeWayCrossing9"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing9"]->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing9"]->setPosition(glm::vec3(-150.0f, 0.0f, -10.0f));

    pmpObjects["threeWayCrossing10"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing10"]->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["threeWayCrossing10"]->setPosition(glm::vec3(-150.0f, 0.0f, -160.0f));

    // Walkways
    pmpObjects["walkways36-3WithCorners1"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners1"]->setPosition(glm::vec3(-5.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners2"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners2"]->setPosition(glm::vec3(-5.0f, 0.0f, -47.0f));

    pmpObjects["walkways36-3WithCorners3"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners3"]->setPosition(glm::vec3(45.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners4"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners4"]->setPosition(glm::vec3(45.0f, 0.0f, -47.0f));

    for(int i = 0; i < 6; i++)
    {
        pmpObjects["walkways36-3WithCorners" + std::to_string(i + 5)] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
        pmpObjects["walkways36-3WithCorners" + std::to_string(i + 5)]->setPosition(glm::vec3(45.0f - (40 * i), 0.0f, 3.0f));
    }

    pmpObjects["walkways30-3_1"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_1"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_1"]->setPosition(glm::vec3(35.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_2"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_2"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_2"]->setPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_3"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_3"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_3"]->setPosition(glm::vec3(48.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_4"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_4"]->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_4"]->setPosition(glm::vec3(85.0f, 0.0f, -13.0f));

    // Bus stations
    pmpObjects["busStation1"] = new cBaseObject(*pmpModels["busStation"]);
    pmpObjects["busStation1"]->setPosition(glm::vec3(8.603f, 0.15f, -11.0f));

    // Traffic lights
    pmpObjects["trafficLight1"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50);
    pmpObjects["trafficLight1"]->setPosition(glm::vec3(34.0f, 0.15f, 2.0f));

    pmpObjects["trafficLight2"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50);
    pmpObjects["trafficLight2"]->setRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight2"]->setPosition(glm::vec3(33.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight3"] = new cLightObject(pmpMeshes["trafficLight"], glm::vec3(1, 0, 0), 50);
    pmpObjects["trafficLight3"]->setRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight3"]->setPosition(glm::vec3(46.0f, 0.15f, -12.0f));

    // Buildings
    pmpObjects["building"] = new cBaseObject(pmpMeshes["building"]);
    pmpObjects["building"]->setPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["needleBuilding"] = new cBaseObject(pmpMeshes["needleBuilding"]);
    pmpObjects["needleBuilding"]->setPosition(glm::vec3(48.0f, 0.0f, -13.0f));

    pmpObjects["bus"] = new cBus(pmpMeshes["bus"]);
    pmpObjects["bus"]->setPosition(glm::vec3(12.5f, 0, -7.5f));
    pmpObjects["bus"]->setRotation(glm::vec3(0.0f, 90.0, 0.0f));
    pmpObjects["bus"]->setScale(glm::vec3(0.8, 0.8, 0.8));

    // Entities
    pmpObjects["entity"] = new cEntity(pmpMeshes["entity"]);
    pmpObjects["entity"]->setPosition(glm::vec3(-10.0f, 0.0f, -26.0f));

    pmpObjects["entity2"] = new cEntity(pmpMeshes["entity"]);
    pmpObjects["entity2"]->setPosition(glm::vec3(-14.0f, 0.0f, -26.0f));

    pmpObjects["entity3"] = new cEntity(pmpMeshes["entity"]);
    pmpObjects["entity3"]->setPosition(glm::vec3(-18.0f, 0.0f, -26.0f));

    pmpObjects["entity4"] = new cEntity(pmpMeshes["entity"]);
    pmpObjects["entity4"]->setPosition(glm::vec3(-22.0f, 0.0f, -26.0f));

    for (uint i = 0; i < 10; i++)
    {
        string key = "multiplayer_bus_" + std::to_string(i);
        pmpObjects[key] = new cBus(pmpMeshes["bus"]);
        pmpObjects[key]->setScale(glm::vec3(0));
        dynamic_cast<cBus*>(pmpObjects[key])->piBusId = i;
    }

    // Create static behaviours
    cBehaviourHandler::AddBehaviour("seperation", "src/scripting/seperation.js");

    cBehaviourHandler *cbSeperation = new cBehaviourHandler("seperation");

    cBehaviourHandler::AddBehaviour("cohesion", "src/scripting/cohesion.js");

    cBehaviourHandler *cbCohesion = new cBehaviourHandler("cohesion");


    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity2"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity3"]));
    entityGroup.AddEntity(dynamic_cast<cEntity *>(pmpObjects["entity4"]));

    entityGroup2 = entityGroup;
    entityGroup.AddBehaviour(cbSeperation);
    entityGroup2.AddBehaviour(cbCohesion);
}
