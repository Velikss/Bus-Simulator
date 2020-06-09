#pragma once

#include <pch.hpp>
#include <vulkan/scene/Scene.hpp>
#include <vulkan/util/GameManager.hpp>
#include <vulkan/scene/LightObject.hpp>

class cMainMenuScene : public cScene
{
    iGameManager* ppOverlayProvider;

protected:
    void Load(cTextureHandler* pTextureHandler, cGeometryHandler* pGeometryHandler, cLogicalDevice* pLogicalDevice,
              cAudioHandler* pAudioHandler) override;

    void AfterLoad() override;

    void Unload() override;

    void HandleMouse(uint uiDeltaX, uint uiDeltaY) override;
public:
    cMainMenuScene(iGameManager* pOverlayProvider)
    {
        ppOverlayProvider = pOverlayProvider;
    }

    ~cMainMenuScene()
    {
    }

    void LoadTextures(cTextureHandler* pTextureHandler);

    void LoadGeometries(cGeometryHandler* pGeometryHandler);

    void LoadMeshes();

    void LoadObjects(cAudioHandler* pAudioHandler);
};

void
cMainMenuScene::Load(cTextureHandler* pTextureHandler,
                     cGeometryHandler* pGeometryHandler,
                     cLogicalDevice* pLogicalDevice,
                     cAudioHandler* pAudioHandler)
{
    LoadTextures(pTextureHandler);
    LoadGeometries(pGeometryHandler);
    LoadMeshes();
    LoadObjects(pAudioHandler);

    poCamera->cameraPos = glm::vec3(3.3f, 1.75f, -1.8f);
    poCamera->pitch = -4.3f;
    poCamera->yaw = -46.68f;

    cScene::Load(pTextureHandler, nullptr, pLogicalDevice, pAudioHandler);
}

void cMainMenuScene::Unload()
{
    cScene::Unload();
}

void cMainMenuScene::AfterLoad()
{
    ppOverlayProvider->ActivateOverlayWindow("MainMenu");
}

void cMainMenuScene::LoadTextures(cTextureHandler* pTextureHandler)
{
    // buildings
    pmpTextures["building"] = pTextureHandler->LoadFromFile("resources/textures/buildings/building.jpg");
    pmpTextures["needle"] = pTextureHandler->LoadFromFile("resources/textures/buildings/needle.jpg");
    // street
    pmpTextures["stoneHouse"] = pTextureHandler->LoadFromFile("resources/textures/stone.jpg");
    pmpTextures["road"] = pTextureHandler->LoadFromFile("resources/textures/streets/road.png");
    pmpTextures["threeWayCrossing"] = pTextureHandler->LoadFromFile(
            "resources/textures/streets/threeWayCrossing.png");
    pmpTextures["fourWayCrossing"] = pTextureHandler->LoadFromFile(
            "resources/textures/streets/fourWayCrossing.png");
    // streetUtil
    pmpTextures["trafficLight"] = pTextureHandler->LoadFromFile(
            "resources/textures/streetUtil/trafficLight.png");
    pmpTextures["busStop"] = pTextureHandler->LoadFromFile("resources/textures/streetUtil/busStop.png");
    // buses
    pmpTextures["bus-yellow"] = pTextureHandler->LoadFromFile("resources/textures/buses/bus-yellow.png");
}

void cMainMenuScene::LoadGeometries(cGeometryHandler* pGeometryHandler)
{
    // streets
    pmpGeometries["road30-10"] = pGeometryHandler->LoadFromFile("resources/geometries/streets/Road30-10.obj");
    pmpGeometries["twoWayCrossing"] =  pGeometryHandler->LoadFromFile("resources/geometries/streets/TwoWayCrossing.obj");
    pmpGeometries["threeWayCrossing"] =  pGeometryHandler->LoadFromFile("resources/geometries/streets/ThreeWayCrossing.obj");
    pmpGeometries["fourWayCrossing"] =  pGeometryHandler->LoadFromFile("resources/geometries/streets/FourWayCrossing.obj");
    // walkways
    pmpGeometries["walkways36-3WithCorners"] =  pGeometryHandler->LoadFromFile("resources/geometries/walkways/Walkway36-3WithCorners.obj", {10, 10});
    pmpGeometries["walkways30-3"] =  pGeometryHandler->LoadFromFile("resources/geometries/walkways/Walkway30-3.obj", {10, 10});
    pmpGeometries["walkways10-3"] =  pGeometryHandler->LoadFromFile("resources/geometries/walkways/Walkway10-3.obj", {10, 10});
    // streetUtil
    pmpGeometries["busStation"] =  pGeometryHandler->LoadFromFile("resources/geometries/streetUtil/busStop.obj");
    pmpGeometries["trafficLight"] =  pGeometryHandler->LoadFromFile("resources/geometries/streetUtil/trafficLight.obj");

    // buses
    pmpGeometries["bus"] =  pGeometryHandler->LoadFromFile("resources/geometries/busses/SchoolBus.obj");
    // buildings
    pmpGeometries["building"] =  pGeometryHandler->LoadFromFile("resources/geometries/buildingTest.obj", {8, 8});
    pmpGeometries["needleBuilding"] =  pGeometryHandler->LoadFromFile("resources/geometries/buildings/needleBuilding.obj", {8, 8});
}

void cMainMenuScene::LoadMeshes()
{
    // streets
    pmpMeshes["road30-10"] = new cMesh(pmpGeometries["road30-10"], pmpTextures["road"]);
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

    // buildings
    pmpMeshes["building"] = new cMesh(pmpGeometries["building"], pmpTextures["building"]);
    pmpMeshes["needleBuilding"] = new cMesh(pmpGeometries["needleBuilding"], pmpTextures["needle"]);
}

void cMainMenuScene::LoadObjects(cAudioHandler* pAudioHandler)
{
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

    // three way crossings
    pmpObjects["threeWayCrossing1"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing1"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing1"]->SetPosition(glm::vec3(35.0f, 0.0f, -15.0f));

    pmpObjects["threeWayCrossing2"] = new cBaseObject(pmpMeshes["threeWayCrossing"]);
    pmpObjects["threeWayCrossing2"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["threeWayCrossing2"]->SetPosition(glm::vec3(-15.0f, 0.0f, -15.0f));

    // Walkways
    pmpObjects["walkways36-3WithCorners1"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners1"]->SetPosition(glm::vec3(-5.0f, 0.0f, -10.0f));

    pmpObjects["walkways36-3WithCorners2"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners2"]->SetPosition(glm::vec3(-5.0f, 0.0f, -47.0f));

    pmpObjects["walkways36-3WithCorners3"] = new cBaseObject(pmpMeshes["walkways36-3WithCorners"]);
    pmpObjects["walkways36-3WithCorners3"]->SetPosition(glm::vec3(45.0f, 0.0f, -10.0f));


    pmpObjects["walkways30-3_1"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_1"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_1"]->SetPosition(glm::vec3(35.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_2"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_2"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_2"]->SetPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["walkways30-3_3"] = new cBaseObject(pmpMeshes["walkways30-3"]);
    pmpObjects["walkways30-3_3"]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    pmpObjects["walkways30-3_3"]->SetPosition(glm::vec3(48.0f, 0.0f, -13.0f));


    // Bus stations
    pmpObjects["busStation1"] = new cBusStop(pmpMeshes["busStation"]);
    pmpObjects["busStation1"]->SetPosition(glm::vec3(8.603f, 0.15f, -11.0f));

    // Traffic lights
    pmpObjects["trafficLight1"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight1"]->SetPosition(glm::vec3(34.0f, 0.15f, 2.0f));

    pmpObjects["trafficLight2"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
    pmpObjects["trafficLight2"]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
    pmpObjects["trafficLight2"]->SetPosition(glm::vec3(33.0f, 0.15f, -11.0f));

    pmpObjects["trafficLight3"] = new cBaseObject(pmpMeshes["trafficLight"], cCollider::UnitCollider(0.4f));
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

    // Buildings
    pmpObjects["building1"] = new cBaseObject(pmpMeshes["building"], cCollider::UnitCollider(34));
    pmpObjects["building1"]->SetPosition(glm::vec3(-2.0f, 0.0f, -13.0f));

    pmpObjects["needleBuilding"] = new cBaseObject(pmpMeshes["needleBuilding"], cCollider::UnitCollider(34));
    pmpObjects["needleBuilding"]->SetPosition(glm::vec3(48.0f, 0.0f, -13.0f));

    // Bus
    pmpObjects["bus"] = new cBus(pAudioHandler, pmpMeshes["bus"]);
    pmpObjects["bus"]->SetPosition(glm::vec3(12.5f, 0, -7.5f));
    pmpObjects["bus"]->SetRotation(glm::vec3(0.0f, 90.0, 0.0f));
    pmpObjects["bus"]->SetScale(glm::vec3(0.8, 0.8, 0.8));

    // Light
    pmpObjects["light"] = new cLightObject(pmpMeshes["building"], glm::vec3(1, 1, 0.3), 40);
    pmpObjects["light"]->SetPosition(glm::vec3(12.5f, 10.0f, -7.5f));
    pmpObjects["light"]->SetScale(glm::vec3(0));
}

void cMainMenuScene::HandleMouse(uint uiDeltaX, uint uiDeltaY)
{
}
