#pragma once

#include <pch.hpp>
#include <cameras/BusCamera.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/entities/Entity.hpp>
#include <vulkan/entities/EntityGroup.hpp>
#include <vulkan/module/overlay/element/TextElement.hpp>
#include <vulkan/AudioHandler.hpp>
#include <logic/GameLogicHandler.hpp>
#include <logic/TrafficLightController.hpp>
#include <entities/Bus.hpp>
#include <entities/Passenger.hpp>
#include <entities/IPassengerHolder.hpp>
#include <objects//BusStop.hpp>
#include <multiplayer/MultiplayerHandler.hpp>
#include <overlay/InGame.hpp>

class cBusWorldScene : public cScene
{
    cTrafficLightController* ppTrafficController = nullptr;
    const float C_ACTIVATION_FRONT_BUS = 20.0f;
public:
    void Update() override;

    void HandleScroll(double dOffsetX, double dOffsetY) override;

    void HandleKey(uint uiKeyCode, uint uiAction) override;

    void AssignMultiplayerHandler(cMultiplayerHandler* pMultiplayerHandler);
    void ClearMultiplayerHandler();
protected:
    void Load(cTextureHandler* pTextureHandler,
              cGeometryHandler* pGeometryHandler,
              cLogicalDevice* pLogicalDevice,
              cAudioHandler* pAudioHandler) override;
    std::map<string, cTexture*> pmpBusTextures;
private:
    cNetworkConnection::tNetworkInitializationSettings tConnectNetworkSettings;
    cMultiplayerHandler* poMultiplayerHandler = nullptr;

    iGameManager* ppOverlayProvider;
public:
    cBusWorldScene(iGameManager* pOverlayProvider, cGameLogicHandler** oGameLogicHandler)
    {
        pGameLogicHandler = oGameLogicHandler;
        ppOverlayProvider = pOverlayProvider;
    }

    ~cBusWorldScene()
    {
        delete poMultiplayerHandler;
    }

    void LoadTextures(cTextureHandler* pTextureHandler);

    void LoadGeometries(cGeometryHandler* pGeometryHandler);

    void LoadMeshes();

    void LoadObjects(cAudioHandler* pAudioHandler);

    void LoadMissions();
    void AfterLoad() override;
    void Unload() override;

    void LoadBehaviours();

    bool BusCentered = false;

    BusCamera* pBusCamera = new BusCamera;
    FirstPersonFlyCamera* pFirstPersonFlyCamera = new FirstPersonFlyCamera;

    cGameLogicHandler** pGameLogicHandler = nullptr;

    void SetBusSkin(const string& sBusSkin);
};

void cBusWorldScene::Load(cTextureHandler* pTextureHandler,
                          cGeometryHandler* pGeometryHandler,
                          cLogicalDevice* pLogicalDevice,
                          cAudioHandler* pAudioHandler)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pGeometryHandler);
    LoadMeshes();
    LoadObjects(pAudioHandler);
    LoadBehaviours();
    LoadMissions();

    cWindow::SetMouseLocked(true);

    cScene::Load(pTextureHandler, pGeometryHandler, pLogicalDevice, pAudioHandler);
}

void cBusWorldScene::Unload()
{
    delete pBusCamera;
    pBusCamera = new BusCamera;
    poCamera = pFirstPersonFlyCamera;
    BusCentered = false;

    if (poMultiplayerHandler) delete poMultiplayerHandler;
    delete pGameLogicHandler;

    cScene::Unload();
}

void cBusWorldScene::Update()
{
    (*pGameLogicHandler)->Update(dynamic_cast<cBus*>(pmpObjects["bus"]));
    if (instanceof<FirstPersonFlyCamera>(poCamera))
        ppTrafficController->Update(poCamera->GetPosition());
    else
    {
        glm::vec3 oPos = pmpObjects["bus"]->GetPosition();
        glm::vec3 direction(sin(glm::radians(pmpObjects["bus"]->GetRotation().y)), 0, cos(glm::radians(
                pmpObjects["bus"]->GetRotation().y)));
        oPos -= (direction * C_ACTIVATION_FRONT_BUS);
        ppTrafficController->Update(oPos);
    }
    if (paKeys[GLFW_KEY_W])
        BusCentered ? dynamic_cast<cBus*>(pmpObjects["bus"])->Accelerate() : poCamera->Forward();
    if (paKeys[GLFW_KEY_S])
        BusCentered ? dynamic_cast<cBus*>(pmpObjects["bus"])->Decelerate() : poCamera->BackWard();
    if (!paKeys[GLFW_KEY_W] && !paKeys[GLFW_KEY_S])
        if (BusCentered) dynamic_cast<cBus*>(pmpObjects["bus"])->IdleAcceleration();
    if (!paKeys[GLFW_KEY_A] && !paKeys[GLFW_KEY_D])
        if (BusCentered) dynamic_cast<cBus*>(pmpObjects["bus"])->IdleSteering();
    if (paKeys[GLFW_KEY_A])
        BusCentered ? dynamic_cast<cBus*>(pmpObjects["bus"])->Steer(cDirection::Left) : poCamera->MoveLeft();
    if (paKeys[GLFW_KEY_D])
        BusCentered ? dynamic_cast<cBus*>(pmpObjects["bus"])->Steer(cDirection::Right) : poCamera->MoveRight();
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

    if (paKeys[GLFW_KEY_TAB])
        ppOverlayProvider->ActivateOverlayWindow("BusMenu");
    if (paKeys[GLFW_KEY_HOME] || paKeys[GLFW_KEY_ESCAPE])
        ppOverlayProvider->ActivateOverlayWindow("Settings");
    if (paKeys[GLFW_KEY_M])
        ppOverlayProvider->ActivateOverlayWindow("MissionMenu");

    dynamic_cast<cBus*>(pmpObjects["bus"])->Move();

    cScene::Update();
    if (instanceof<cInGame>(ppOverlayProvider->GetActiveOverlayWindow()))
    {
        ((cInGame*) ppOverlayProvider->GetActiveOverlayWindow())->UpdateSpeed(
                dynamic_cast<cBus*>(pmpObjects["bus"])->pfCurrentSpeed);

    }
    if (poMultiplayerHandler) poMultiplayerHandler->PushData();

    static bool bOverlayRequested = false;
    if (ppOverlayProvider->GetActiveOverlayWindow() == nullptr)
    {
        if (!bOverlayRequested)
        {
            ppOverlayProvider->ActivateOverlayWindow("InGame");
            bOverlayRequested = true;
        }
    }
    else
    {
        bOverlayRequested = false;
    }
}

void cBusWorldScene::HandleKey(uint uiKeyCode, uint uiAction)
{
    cScene::HandleKey(uiKeyCode, uiAction);
    // Horn
    if (uiAction == GLFW_PRESS && uiKeyCode == GLFW_KEY_E)
    {
        ppAudioHandler->PlaySound("resources/audio/horn1.wav", dynamic_cast<cBus*>(pmpObjects["bus"])->GetPosition(),
                                  1.0f);
    }
    // Bus door
    if (uiAction == GLFW_PRESS && uiKeyCode == GLFW_KEY_O)
    {
        cBus* bus = dynamic_cast<cBus*>(pmpObjects["bus"]);
        if (!bus->pbDoorOpen)
            bus->OpenDoors();
        else
            bus->CloseDoors();
    }
}

void cBusWorldScene::HandleScroll(double dOffsetX, double dOffsetY)
{
    poCamera->LookMouseWheelDiff((float) dOffsetX, (float) dOffsetY);
}

void cBusWorldScene::LoadMissions()
{
    (*pGameLogicHandler)->pmpMissions["Mission1"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation1"]));
    (*pGameLogicHandler)->pmpMissions["Mission1"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation2"]));
    (*pGameLogicHandler)->pmpMissions["Mission1"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation3"]));
    (*pGameLogicHandler)->pmpMissions["Mission1"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation4"]));
    (*pGameLogicHandler)->pmpMissions["Mission1"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation5"]));

    (*pGameLogicHandler)->pmpMissions["Mission2"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation1"]));
    (*pGameLogicHandler)->pmpMissions["Mission2"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation5"]));
    (*pGameLogicHandler)->pmpMissions["Mission2"]->AddStop(dynamic_cast<cBusStop*>(pmpObjects["busStation4"]));
}

void cBusWorldScene::LoadBehaviours()
{
    // Init behaviour handler
    cBehaviourHandler::Init();
    cBehaviourHandler::AddBehavioursFromDirectory("resources/scripting");

    pcbSeperation = new cBehaviourHandler("seperation");
    pcbCohesion = new cBehaviourHandler("cohesion");
    pcbSeeking = new cBehaviourHandler("seeking");
}

void cBusWorldScene::LoadTextures(cTextureHandler* pTextureHandler)
{
    pmpTextures["roof"] = pTextureHandler->LoadFromFile("resources/textures/roof.jpg");
    pmpTextures["stoneHouse"] = pTextureHandler->LoadFromFile("resources/textures/stone.jpg");
    pmpTextures["grass"] = pTextureHandler->LoadFromFile("resources/textures/grass.jpg");
    pmpTextures["skybox"] = pTextureHandler->LoadFromFile("resources/textures/skybox.jpg",
                                                          pTextureHandler->GetSkyboxSampler());
    // buildings
    pmpTextures["building"] = pTextureHandler->LoadFromFile("resources/textures/buildings/building.jpg");
    pmpTextures["building2"] = pTextureHandler->LoadFromFile("resources/textures/buildings/building2.jpg");
    pmpTextures["needle"] = pTextureHandler->LoadFromFile("resources/textures/buildings/needle.jpg");
    //street
    pmpTextures["road"] = pTextureHandler->LoadFromFile("resources/textures/streets/road.png");
    pmpTextures["cornerRoad"] = pTextureHandler->LoadFromFile("resources/textures/streets/cornerRoad.png");
    pmpTextures["threeWayCrossing"] = pTextureHandler->LoadFromFile(
            "resources/textures/streets/threeWayCrossing.png");
    pmpTextures["fourWayCrossing"] = pTextureHandler->LoadFromFile(
            "resources/textures/streets/fourWayCrossing.png");
    // streetUtil
    pmpTextures["trafficLight"] = pTextureHandler->LoadFromFile(
            "resources/textures/streetUtil/trafficLight.png");
    pmpTextures["busStop"] = pTextureHandler->LoadFromFile("resources/textures/streetUtil/busStop.png");
    // buses
    std::string path = "resources/textures/buses";
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (!entry.is_directory())
        {
#if defined(LINUX)
            std::vector<std::string> soPathSplit = split(split(entry.path(), ".")[0], "/");
#elif defined(WINDOWS)
            std::vector<std::string> soPathSplit = split(split(entry.path().string(), ".")[0], "\\");
#endif
            std::string key = soPathSplit[soPathSplit.size() - 1];
            pmpTextures[key] = pTextureHandler->LoadFromFile(entry.path().string().c_str());
        }
    }

    // passengers
    pmpTextures["passenger"] = pTextureHandler->LoadFromFile("resources/textures/penguin.png");
}

void cBusWorldScene::LoadGeometries(cGeometryHandler* pGeometryHandler)
{
    // skybox
    pmpGeometries["skybox"] = pGeometryHandler->LoadFromFile("resources/geometries/skybox.obj");
    // streets
    pmpGeometries["road30-10"] = pGeometryHandler->LoadFromFile("resources/geometries/streets/Road30-10.obj");
    pmpGeometries["cornerRoad"] = pGeometryHandler->LoadFromFile("resources/geometries/streets/CornerRoad.obj");
    pmpGeometries["twoWayCrossing"] = pGeometryHandler->LoadFromFile("resources/geometries/streets/TwoWayCrossing.obj");
    pmpGeometries["threeWayCrossing"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/streets/ThreeWayCrossing.obj");
    pmpGeometries["fourWayCrossing"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/streets/FourWayCrossing.obj");
    // walkways
    pmpGeometries["walkways36-3WithCorners"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/walkways/Walkway36-3WithCorners.obj", {10, 10});
    pmpGeometries["walkways30-3"] = pGeometryHandler->LoadFromFile("resources/geometries/walkways/Walkway30-3.obj",
                                                                   {10, 10});
    pmpGeometries["walkways10-3"] = pGeometryHandler->LoadFromFile("resources/geometries/walkways/Walkway10-3.obj",
                                                                   {10, 10});
    // streetUtil
    pmpGeometries["busStation"] = pGeometryHandler->LoadFromFile("resources/geometries/streetUtil/busStop.obj");
    pmpGeometries["trafficLight"] = pGeometryHandler->LoadFromFile("resources/geometries/streetUtil/trafficLight.obj");
    // streetDeco

    // buses
    pmpGeometries["bus"] = pGeometryHandler->LoadFromFile("resources/geometries/busses/SchoolBus.obj");
    // buildings
    pmpGeometries["building"] = pGeometryHandler->LoadFromFile("resources/geometries/buildingTest.obj", {8, 8});
    pmpGeometries["needleBuilding"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/needleBuilding.obj", {20, 20});
    pmpGeometries["blockBuilding2"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/blockBuilding2.obj", {10, 10});
    pmpGeometries["blockBuilding3"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/blockBuilding3.obj", {5, 5});
    pmpGeometries["blockBuilding4"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/blockBuilding4.obj", {20, 20});
    pmpGeometries["longSideBuilding1"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/longSideBuilding1.obj", {20, 20});
    pmpGeometries["longSideBuilding2"] = pGeometryHandler->LoadFromFile(
            "resources/geometries/buildings/longSideBuilding2.obj", {11, 12});

    // grass
    pmpGeometries["grassField1"] = pGeometryHandler->LoadFromFile("resources/geometries/grassField1.obj", {8, 8});
    // penguin
    pmpGeometries["passenger"] = pGeometryHandler->LoadFromFile("resources/geometries/penguin.obj");
}

void cBusWorldScene::LoadMeshes()
{
    //skybox
    pmpMeshes["skybox"] = new cMesh(pmpGeometries["skybox"], pmpTextures["skybox"]);
    // streets
    pmpMeshes["road30-10"] = new cMesh(pmpGeometries["road30-10"], pmpTextures["road"]);
    pmpMeshes["cornerRoad"] = new cMesh(pmpGeometries["cornerRoad"], pmpTextures["cornerRoad"]);
    pmpMeshes["twoWayCrossing"] = new cMesh(pmpGeometries["twoWayCrossing"], pmpTextures["road"]);
    pmpMeshes["threeWayCrossing"] = new cMesh(pmpGeometries["threeWayCrossing"], pmpTextures["threeWayCrossing"]);
    // walkways
    pmpMeshes["walkways36-3WithCorners"] = new cMesh(pmpGeometries["walkways36-3WithCorners"],
                                                     pmpTextures["stoneHouse"]);
    pmpMeshes["walkways30-3"] = new cMesh(pmpGeometries["walkways30-3"], pmpTextures["stoneHouse"]);
    pmpMeshes["walkways10-3"] = new cMesh(pmpGeometries["walkways10-3"], pmpTextures["stoneHouse"]);
    // bus
    pmpMeshes["bus"] = new cMesh(pmpGeometries["bus"], pmpTextures["bus-yellow"]);
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
    pmpMeshes["longSideBuilding1"] = new cMesh(pmpGeometries["longSideBuilding1"], pmpTextures["needle"]);
    pmpMeshes["longSideBuilding2"] = new cMesh(pmpGeometries["longSideBuilding2"], pmpTextures["building2"]);

    // grass
    pmpMeshes["grassField1"] = new cMesh(pmpGeometries["grassField1"], pmpTextures["grass"]);

    // passenger
    pmpMeshes["passenger"] = new cMesh(pmpGeometries["passenger"], pmpTextures["passenger"]);
}

void cBusWorldScene::LoadObjects(cAudioHandler* pAudioHandler)
{
    // skybox
    pmpObjects["skybox"] = new cBaseObject(pmpMeshes["skybox"]);
    pmpObjects["skybox"]->SetScale(glm::vec3(500.0f, 500.0f, 500.0f));
    pmpObjects["skybox"]->SetPosition(glm::vec3(0.0f, -250.0f, -50.0f));

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
    pmpObjects["threeWayCrossing5"]->SetPosition(glm::vec3(80.0f, 0.0f, -50.0f));

    pmpObjects["threeWayCrossing6"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing6"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["threeWayCrossing6"]->SetPosition(glm::vec3(-5.0f, 0.0f, -145.0f));

    // corner roads
    pmpObjects["cornerRoad1"] = new cBaseObject(pmpMeshes["cornerRoad"]);
    pmpObjects["cornerRoad1"]->SetPosition(glm::vec3(80.0f, 0.0f, 0.0f));

    pmpObjects["cornerRoad2"] = new cBaseObject(pmpMeshes["cornerRoad"]);
    pmpObjects["cornerRoad2"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["cornerRoad2"]->SetPosition(glm::vec3(95.0f, 0.0f, -145.0f));

    pmpObjects["cornerRoad3"] = new cBaseObject(pmpMeshes["cornerRoad"]);
    pmpObjects["cornerRoad3"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["cornerRoad3"]->SetPosition(glm::vec3(-165.0f, 0.0f, -15.0f));

    pmpObjects["cornerRoad4"] = new cBaseObject(pmpMeshes["cornerRoad"]);
    pmpObjects["cornerRoad4"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["cornerRoad4"]->SetPosition(glm::vec3(-150.0f, 0.0f, -160.0f));

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

    pmpObjects["walkways36-3WithCorners27"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners27"]->SetPosition(glm::vec3(-55.0f, 0.0f, -160.0f));

    pmpObjects["walkways36-3WithCorners28"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners28"]->SetPosition(glm::vec3(-105.0f, 0.0f, -160.0f));

    pmpObjects["walkways36-3WithCorners29"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners29"]->SetPosition(glm::vec3(-155.0f, 0.0f, -160.0f));

    pmpObjects["walkways36-3WithCorners30"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners30"]->SetPosition(glm::vec3(-5.0f, 0.0f, -160.0f));

    pmpObjects["walkways36-3WithCorners31"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners31"]->SetPosition(glm::vec3(45.0f, 0.0f, -160.0f));

    pmpObjects["walkways36-3WithCorners32"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners32"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners32"]->SetPosition(glm::vec3(98.0f, 0.0f, -5.0f));

    pmpObjects["walkways36-3WithCorners33"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners33"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners33"]->SetPosition(glm::vec3(98.0f, 0.0f, -65.0f));

    pmpObjects["walkways36-3WithCorners34"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners34"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners34"]->SetPosition(glm::vec3(98.0f, 0.0f, -115.0f));

    pmpObjects["walkways36-3WithCorners35"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners35"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners35"]->SetPosition(glm::vec3(-165.0f, 0.0f, -5.0f));

    pmpObjects["walkways36-3WithCorners36"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners36"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners36"]->SetPosition(glm::vec3(-165.0f, 0.0f, -65.0f));

    pmpObjects["walkways36-3WithCorners37"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners37"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways36-3WithCorners37"]->SetPosition(glm::vec3(-165.0f, 0.0f, -115.0f));

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

    pmpObjects["walkways10-3_13"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_13"]->SetPosition(glm::vec3(-65.0f, 0.0f, -160.0f));

    pmpObjects["walkways10-3_14"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_14"]->SetPosition(glm::vec3(-115.0f, 0.0f, -160.0f));

    pmpObjects["walkways10-3_15"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_15"]->SetPosition(glm::vec3(-15.0f, 0.0f, -160.0f));

    pmpObjects["walkways10-3_16"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_16"]->SetPosition(glm::vec3(35.0f, 0.0f, -160.0f));

    pmpObjects["walkways10-3_17"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_17"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_17"]->SetPosition(glm::vec3(98.0f, 0.0f, 5.0f));

    pmpObjects["walkways10-3_18"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_18"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_18"]->SetPosition(glm::vec3(98.0f, 0.0f, -45.0f));

    pmpObjects["walkways10-3_19"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_19"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_19"]->SetPosition(glm::vec3(98.0f, 0.0f, -55.0f));

    pmpObjects["walkways10-3_20"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_20"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_20"]->SetPosition(glm::vec3(98.0f, 0.0f, -105.0f));

    pmpObjects["walkways10-3_21"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_21"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_21"]->SetPosition(glm::vec3(98.0f, 0.0f, -155.0f));

    pmpObjects["walkways10-3_22"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_22"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_22"]->SetPosition(glm::vec3(-165.0f, 0.0f, 5.0f));

    pmpObjects["walkways10-3_23"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_23"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_23"]->SetPosition(glm::vec3(-165.0f, 0.0f, -45.0f));

    pmpObjects["walkways10-3_24"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_24"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_24"]->SetPosition(glm::vec3(-165.0f, 0.0f, -55.0f));

    pmpObjects["walkways10-3_25"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_25"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_25"]->SetPosition(glm::vec3(-165.0f, 0.0f, -105.0f));

    pmpObjects["walkways10-3_26"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_26"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways10-3_26"]->SetPosition(glm::vec3(-165.0f, 0.0f, -155.0f));

    pmpObjects["walkways10-3_27"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_27"]->SetPosition(glm::vec3(85.0f, 0.0f, 3.0f));

    pmpObjects["walkways10-3_28"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_28"]->SetPosition(glm::vec3(-165.0f, 0.0f, 3.0f));

    pmpObjects["walkways10-3_29"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_29"]->SetPosition(glm::vec3(-165.0f, 0.0f, -160.0f));

    pmpObjects["walkways10-3_30"] = new cBaseObject(pmpMeshes["walkways10-3"]);
    pmpObjects["walkways10-3_30"]->SetPosition(glm::vec3(85.0f, 0.0f, -160.0f));

    // Bus stations
    pmpObjects["busStation1"] = new cBusStop(pmpMeshes["busStation"], "busStation1");
    pmpObjects["busStation1"]->SetPosition(glm::vec3(8.603f, 0.15f, -11.0f));

    pmpObjects["busStation2"] = new cBusStop(pmpMeshes["busStation"], "busStation2");
    pmpObjects["busStation2"]->SetPosition(glm::vec3(-140.0f, 0.15f, -11.0f));

    pmpObjects["busStation3"] = new cBusStop(pmpMeshes["busStation"], "busStation3");
    pmpObjects["busStation3"]->SetPosition(glm::vec3(-16.0f, 0.15f, -97.0f));
    pmpObjects["busStation3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));

    pmpObjects["busStation4"] = new cBusStop(pmpMeshes["busStation"], "busStation4");
    pmpObjects["busStation4"]->SetPosition(glm::vec3(66.0f, 0.15f, -61.0f));

    pmpObjects["busStation5"] = new cBusStop(pmpMeshes["busStation"], "busStation5");
    pmpObjects["busStation5"]->SetPosition(glm::vec3(40.0f, 0.15f, -149.0f));
    pmpObjects["busStation5"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));

    // Traffic lights
    ppTrafficController = new cTrafficLightController("TrafficController1", pmpObjects, pmpMeshes["skybox"]);
    pmpObjects["trafficLight1"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight1"]->SetPosition(glm::vec3(34.0f, 0.15f, 2.0f));

    pmpObjects["trafficLight2"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight2"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight2"]->SetPosition(glm::vec3(33.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight3"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight3"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight3"]->SetPosition(glm::vec3(46.0f, 0.15f, -12.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight1"], pmpObjects["trafficLight2"], pmpObjects["trafficLight3"]});

    pmpObjects["trafficLight4"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight4"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight4"]->SetPosition(glm::vec3(-4.0f, 0.15f, -12.0f));

    pmpObjects["trafficLight5"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight5"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight5"]->SetPosition(glm::vec3(-17.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight6"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight6"]->SetPosition(glm::vec3(-16.0f, 0.15f, 2.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight4"], pmpObjects["trafficLight5"], pmpObjects["trafficLight6"]});

    pmpObjects["trafficLight7"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight7"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight7"]->SetPosition(glm::vec3(-17.0f, 0.15f, -61.0f));

    pmpObjects["trafficLight8"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight8"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight8"]->SetPosition(glm::vec3(-4.0f, 0.15f, -62.0f));

    pmpObjects["trafficLight9"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight9"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight9"]->SetPosition(glm::vec3(-3.0f, 0.15f, -49.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight7"], pmpObjects["trafficLight8"], pmpObjects["trafficLight9"]});

    pmpObjects["trafficLight10"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight10"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight10"]->SetPosition(glm::vec3(46.0f, 0.15f, -62.0f));

    pmpObjects["trafficLight11"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight11"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight11"]->SetPosition(glm::vec3(47.0f, 0.15f, -49.0f));

    pmpObjects["trafficLight12"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight12"]->SetPosition(glm::vec3(34.0f, 0.15f, -48.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight10"], pmpObjects["trafficLight11"], pmpObjects["trafficLight12"]});

    pmpObjects["trafficLight13"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight13"]->SetPosition(glm::vec3(84.0f, 0.15f, -48.0f));

    pmpObjects["trafficLight14"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight14"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight14"]->SetPosition(glm::vec3(83.0f, 0.15f, -61.0f));

    pmpObjects["trafficLight15"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight15"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight15"]->SetPosition(glm::vec3(97.0f, 0.15f, -49.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight13"], pmpObjects["trafficLight14"], pmpObjects["trafficLight15"]});

    pmpObjects["trafficLight16"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight16"]->SetPosition(glm::vec3(-16.0f, 0.15f, -148.0f));

    pmpObjects["trafficLight17"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight17"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["trafficLight17"]->SetPosition(glm::vec3(-4.0f, 0.15f, -162.0f));

    pmpObjects["trafficLight18"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight18"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["trafficLight18"]->SetPosition(glm::vec3(-3.0f, 0.15f, -149.0f));
    ppTrafficController->AddGroup(
            {pmpObjects["trafficLight16"], pmpObjects["trafficLight17"], pmpObjects["trafficLight18"]});

    // Buildings
    pmpObjects["building1"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building1"]->SetPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

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

    pmpObjects["longSideBuilding1_1"] = new cBaseObject(pmpMeshes["longSideBuilding1"],
                                                        cCollider::RectangleCollider(240, 33));
    pmpObjects["longSideBuilding1_1"]->SetPosition(glm::vec3(-155.0f, 0.0f, 36.0f));

    pmpObjects["longSideBuilding1_2"] = new cBaseObject(pmpMeshes["longSideBuilding1"],
                                                        cCollider::RectangleCollider(240, 33));
    pmpObjects["longSideBuilding1_2"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["longSideBuilding1_2"]->SetPosition(glm::vec3(85.0f, 0.0f, -196.0f));

    pmpObjects["longSideBuilding2_1"] = new cBaseObject(pmpMeshes["longSideBuilding2"],
                                                        cCollider::RectangleCollider(34, 170));
    pmpObjects["longSideBuilding2_1"]->SetPosition(glm::vec3(-202.0f, 0.0f, 5.0f));

    pmpObjects["longSideBuilding2_2"] = new cBaseObject(pmpMeshes["longSideBuilding2"],
                                                        cCollider::RectangleCollider(34, 170));
    pmpObjects["longSideBuilding2_2"]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
    pmpObjects["longSideBuilding2_2"]->SetPosition(glm::vec3(132.0f, 0.0f, -165.0f));

    pmpObjects["building2"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building2"]->SetPosition(glm::vec3(85.0f, 0.0f, 37.0f));

    pmpObjects["building3"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building3"]->SetPosition(glm::vec3(-189.0f, 0.0f, 37.0f));

    pmpObjects["building4"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building4"]->SetPosition(glm::vec3(-189.0f, 0.0f, -163.0f));

    pmpObjects["building5"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building5"]->SetPosition(glm::vec3(85.0f, 0.0f, -163.0f));

    // Grass
    pmpObjects["grassField1_1"] = new cEntity(pmpMeshes["grassField1"]);
    pmpObjects["grassField1_1"]->SetPosition(glm::vec3(-152.0f, 0.0f, -13.0f));

    // Bus
    pmpObjects["bus"] = new cBus(pAudioHandler, pmpMeshes["bus"]);
    pmpObjects["bus"]->SetPosition(glm::vec3(12.5f, 0, -7.5f));
    pmpObjects["bus"]->SetRotation(glm::vec3(0.0f, 90.0, 0.0f));
    pmpObjects["bus"]->SetScale(glm::vec3(0.8, 0.8, 0.8));

    // Entities
    for (uint i = 0; i < 11; i++)
    {
        string key = "passenger" + std::to_string(i);
        pmpObjects[key] = new cPassenger(pmpMeshes["passenger"]);
        pmpObjects[key]->SetPosition(glm::vec3(200.0f, 0.15f, -200.0f));
        pmpObjects[key]->pbVisible = false;
    }

    for (uint i = 0; i < 10; i++)
    {
        string key = "multiplayer_bus_" + std::to_string(i);
        pmpObjects[key] = new cBus(pAudioHandler, pmpMeshes["bus"]);
        pmpObjects[key]->SetScale(glm::vec3(0));
        dynamic_cast<cBus*>(pmpObjects[key])->piBusId = i;
    }
}

void cBusWorldScene::AssignMultiplayerHandler(cMultiplayerHandler* pMultiplayerHandler)
{
    poMultiplayerHandler = pMultiplayerHandler;
}

void cBusWorldScene::ClearMultiplayerHandler()
{
    poMultiplayerHandler = nullptr;
}

void cBusWorldScene::AfterLoad()
{
    cScene::AfterLoad();
    if (poMultiplayerHandler) poMultiplayerHandler->AssignScene(this);
    ppOverlayProvider->ActivateOverlayWindow("InGame");
}

void cBusWorldScene::SetBusSkin(const std::string& sBusSkin)
{
    ENGINE_LOG(sBusSkin);
    pmpObjects["bus"]->GetMesh()->SetTexture(pmpTextures[sBusSkin]);
}
