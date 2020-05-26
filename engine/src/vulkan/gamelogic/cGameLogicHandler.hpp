#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/entities/cBus.hpp>
#include "vulkan/scene/Scene.hpp"
#include "cMissionHandler.hpp"

class cGameLogicHandler
{
private:
    cScene* ppScene;
    cMissionHandler* ppMission;
    cBus* ppBus;
public:
    cGameLogicHandler(cScene* pScene, cBus* pBus, cMissionHandler* pMission = nullptr)
    {
        ppScene = pScene;
        ppBus = pBus;
        ppMission = pMission;
    }

    void Update();
    cMissionHandler* GetMissionHandler();
    bool SetMissionHandler(cMissionHandler* pMissionHandler);
};

void cGameLogicHandler::Update()
{
    // TODO Everything from scene update
}

cMissionHandler* cGameLogicHandler::GetMissionHandler()
{
    return ppMission;
}

bool cGameLogicHandler::SetMissionHandler(cMissionHandler* pMissionHandler)
{
    ppMission = pMissionHandler;
}