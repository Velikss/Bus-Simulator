#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/entities/cBus.hpp>
#include "vulkan/scene/Scene.hpp"
#include "cMissionHandler.hpp"

class cGameLogicHandler
{
private:
    const int iMaxBusStopPassengers = 3;

    cScene* ppScene;
    cMissionHandler* ppMission;
    cBus* ppBus;
    cBehaviourHandler* pcbSeperation = nullptr;
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
    bool LoadMission();
};

void cGameLogicHandler::Update()
{
    ppMission->Update();
}

bool cGameLogicHandler::LoadMission()
{
    if(ppMission == nullptr)
        return false;
    // TODO at the moment this has to happen after the behaviours have been loaded in the LoadObject of the scene.
    if(pcbSeperation == nullptr)
        pcbSeperation = new cBehaviourHandler("seperation");

    std::deque<cBusStop*> oRoute = ppMission->GetRouteQueue();
    std::map<string, cBaseObject*> mpObjects = ppScene->GetObjects();

    // TODO probably need to make a global stack
    // stack for all the passengers currently available
    std::stack<int> oPassengersQueue;
    for (auto& num : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
        oPassengersQueue.push(num);
    }

    // loop through all busStops on the route
    for(uint i = 0; i < oRoute.size(); i++)
    {
        // get random amount of passengers to spawn next to the busStop
        int iPassengerAmount = rand() % (iMaxBusStopPassengers + 1); // rand between 0 and iMaxBusStopPassengers
        // create the random amount of passengers
        for (int j = 0; j < iPassengerAmount; ++j)
        {
            // check if there are still passengers available to spawn
            if(!oPassengersQueue.empty())
            {
                int iKeyNum = oPassengersQueue.top();
                string key = "passenger" + std::to_string(iKeyNum);
                // check if addPassenger was successful
                if(oRoute[i]->AddPassenger(dynamic_cast<IPassenger*>(mpObjects[key])))
                {
                    glm::vec3 oBusStopRotation =  oRoute[i]->GetRotation();
                    // Check the rotation of the bus stop and place the passengers accordingly.
                    // The passengers can't be placed on the same position otherwise the behaviours break.
                    // this is why one of the values is multiplied by iKeyNum
                    if(oBusStopRotation.y == 0.0f)
                    {
                        mpObjects[key]->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * 0.1f, 0.0f, 0.5f));
                        mpObjects[key]->SetRotation(glm::vec3(0.0f));
                    }
                    else if(oBusStopRotation.y == 90.0f)
                    {
                        mpObjects[key]->SetPosition(oRoute[i]->GetPosition() + glm::vec3(0.5f, 0.0f, iKeyNum * -0.1f));
                        mpObjects[key]->SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
                    }
                    else if(oBusStopRotation.y == 180.0f)
                    {
                        mpObjects[key]->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * -0.1f, 0.0f, -0.5f));
                        mpObjects[key]->SetRotation(glm::vec3(0.0f, 180.0f, 0.0f));
                    }
                    else if(oBusStopRotation.y == 270.0f)
                    {
                        mpObjects[key]->SetPosition(oRoute[i]->GetPosition() + glm::vec3(-0.5f, 0.0f, iKeyNum * 0.1f));
                        mpObjects[key]->SetRotation(glm::vec3(0.0f, 270.0f, 0.0f));
                    }
                    else{
                        // If rotation is not one of the 4 default rotations
                        // then it will not change the rotation of the passenger and will move them on the X-axis
                        mpObjects[key]->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * 0.1f, 0.0f, 0.0f));
                    }
                    // pop used passenger form the available passengers queue
                    oPassengersQueue.pop();
                }
            }
        }
        // Todo how do we want to add behaviours to the missions, always the same behaviours? (Currently always seperation)
        // Check if busStops already have this behaviour
        if(!oRoute[i]->poEntityGroup->BehaviourExists(pcbSeperation))
            oRoute[i]->poEntityGroup->AddBehaviour(pcbSeperation);
    }

    return true;
}

cMissionHandler* cGameLogicHandler::GetMissionHandler()
{
    return ppMission;
}

bool cGameLogicHandler::SetMissionHandler(cMissionHandler* pMissionHandler)
{
    // Unload the busStops from current missionHandler
    if(ppMission != nullptr)
        ppMission->UnloadMissionHandler();

    ppMission = pMissionHandler;

    // TODO check if already exists
    return true;
}