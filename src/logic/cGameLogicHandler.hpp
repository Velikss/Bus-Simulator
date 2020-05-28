#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/scene/Scene.hpp>
#include <entities/cBus.hpp>
#include <logic/cMissionHandler.hpp>

class cGameLogicHandler
{
private:
    const int iMaxBusStopPassengers = 3;

    cScene* ppScene;
    cMissionHandler* ppMission;
    cBus* ppBus;
    cBehaviourHandler* pcbSeperation = nullptr;
    cBehaviourHandler* pcbSeeking = nullptr;
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
    if(pcbSeeking == nullptr)
        pcbSeeking = new cBehaviourHandler("seeking");

    std::deque<cBusStop*> oRoute = ppMission->GetRouteQueue();
    std::map<string, cBaseObject*> mpObjects = ppScene->GetObjects();

    // TODO probably need to make a global stack
    // Stack for all the passengers currently available
    std::stack<int> oPassengersQueue;
    for (auto& num : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) {
        oPassengersQueue.push(num);
    }

    // Loop through all busStops on the route
    for(uint i = 0; i < oRoute.size(); i++)
    {
        // Todo how do we want to add behaviours to the missions, always the same behaviours? (Currently always seperation)
        // Check if busStops already have this behaviour
        if(!oRoute[i]->poEntityGroup->BehaviourExists(pcbSeperation))
            oRoute[i]->poEntityGroup->AddBehaviour(pcbSeperation);
        if(!oRoute[i]->poEntityGroup->BehaviourExists(pcbSeeking))
            oRoute[i]->poEntityGroup->AddBehaviour(pcbSeeking);

        // The last buStop can not have any passengers
        if(i == oRoute.size() - 1)
            break;

        // get random amount of passengers to spawn next to the busStop
        int iPassengerAmount = rand() % (iMaxBusStopPassengers + 1); // rand between 0 and iMaxBusStopPassengers
        // create the random amount of passengers
        for (int j = 0; j < iPassengerAmount; ++j)
        {
            // Check if there are still passengers available to spawn
            if(!oPassengersQueue.empty())
            {
                // create random destination
                int iRandDestIndex = rand() % (oRoute.size() - (i + 1))+ (i + 1);

                // Create key for the passenger
                int iKeyNum = oPassengersQueue.top();
                string key = "passenger" + std::to_string(iKeyNum);

                // Check if addPassenger was successful
                if(oRoute[i]->AddPassenger(dynamic_cast<cPassenger*>(mpObjects[key])))
                {
                    cPassenger* oCurrentPassenger = dynamic_cast<cPassenger*>(mpObjects[key]);
                    glm::vec3 oBusStopRotation =  oRoute[i]->GetRotation();

                    // Check the rotation of the bus stop and place the passengers accordingly.
                    // The passengers can't be placed on the same position otherwise the behaviours break.
                    // This is why one of the values is multiplied by iKeyNum
                    if(oBusStopRotation.y == 0.0f)
                    {
                        oCurrentPassenger->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * 0.1f, 0.0f, 0.5f));
                    }
                    else if(oBusStopRotation.y == 90.0f)
                    {
                        oCurrentPassenger->SetPosition(oRoute[i]->GetPosition() + glm::vec3(0.5f, 0.0f, iKeyNum * -0.1f));
                    }
                    else if(oBusStopRotation.y == 180.0f)
                    {
                        oCurrentPassenger->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * -0.1f, 0.0f, -0.5f));
                    }
                    else if(oBusStopRotation.y == 270.0f)
                    {
                        oCurrentPassenger->SetPosition(oRoute[i]->GetPosition() + glm::vec3(-0.5f, 0.0f, iKeyNum * 0.1f));
                    }
                    else
                    {
                        // If rotation is not one of the 4 default rotations, then it will move them on the X-axis
                        oCurrentPassenger->SetPosition(oRoute[i]->GetPosition() + glm::vec3(iKeyNum * 0.1f, 0.0f, 0.0f));
                    }

                    // Set rotation the same as the bus stop
                    oCurrentPassenger->SetRotation(glm::vec3(0.0f, oBusStopRotation.y, 0.0f));
                    oCurrentPassenger->SetTarget(oRoute[i]->GetPosition()); // set target to bus stop
                    oCurrentPassenger->SetDestination(oRoute[iRandDestIndex]); // set destination to random bus stop
                    oCurrentPassenger->pbVisible = true;

                    std::cout << "BusStop" << i << ", "<< key << ", dest = " << iRandDestIndex << std::endl;

                    // pop used passenger form the available passengers queue
                    oPassengersQueue.pop();
                }
            }
        }
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