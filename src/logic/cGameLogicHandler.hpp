#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/scene/Scene.hpp>
#include <entities/cBus.hpp>
#include <logic/cMissionHandler.hpp>

const float C_PASSENGER_ENTER_DISTANCE = 1.5f;
const int C_MAX_BUS_STOP_PASSENGERS = 3;

class cGameLogicHandler
{
private:
    cScene* ppScene;
    cMissionHandler* ppMission;
    cBus* ppBus;
    cBusStop* poCurrentBusStop = nullptr;
    std::map<string, cBaseObject*> pmpObjects;
public:
    cGameLogicHandler(cScene* pScene, cBus* pBus, cMissionHandler* pMission = nullptr)
    {
        ppScene = pScene;
        ppBus = pBus;
        ppMission = pMission;
        pmpObjects = ppScene->GetObjects();
    }

    void Update();
    cMissionHandler* GetMissionHandler();
    bool SetMissionHandler(cMissionHandler* pMissionHandler);
    bool LoadMission();
    void LoadPassengers(cBusStop* oBusStop);
    void UnloadPassengers(cBusStop* oBusStop);
    void ResetBus();
};

void cGameLogicHandler::Update()
{
    if(ppMission != nullptr) {
        // update all passenger entities with their behaviours
        ppMission->Update();

        if (ppBus->oState == cState::eStill) {
            if (ppBus->pfCurrentSpeed > 0.0f)
                ppBus->oState = cState::eDriving;
        }
        if (ppBus->oState == cState::eDriving) {
            if (ppBus->pfCurrentSpeed == 0.0f) {
                // Get bus stop that is within the radius
                cBusStop *oCurrentBusStop = ppMission->BusStopWithinRadius(ppBus->GetDoorPosition());

                // Check if a bus stop was close enough
                if (oCurrentBusStop == nullptr) {
                    ppBus->oState = cState::eStill;
                }
                else {
                    ppBus->oState = cState::eUnloading;
                    poCurrentBusStop = oCurrentBusStop;
                }
            }
        }
        if (ppBus->oState == cState::eUnloading) {
            UnloadPassengers(poCurrentBusStop);
            ppBus->oState = cState::eLoading;
        }
        if (ppBus->oState == cState::eLoading) {
            LoadPassengers(poCurrentBusStop);
            // go to state eStill if bus stop has no more passengers available
            if (!ppMission->PassengersAvailable(poCurrentBusStop))
                ppBus->oState = cState::eStill;
        }
    }
}

// Load al passengers from the given bus stop onto the bus
void cGameLogicHandler::LoadPassengers(cBusStop* oBusStop)
{
    glm::vec3 oBusDoorPos = ppBus->GetDoorPosition();

    std::vector<IEntity *> *entities;
    oBusStop->poEntityGroup->GetEntityList(&entities);

    // Loop through all passengers currently in the bus stop
    for (uint i = 0; i < oBusStop->poEntityGroup->GetEntities()->size(); i++)
    {
        cPassenger* oPassenger = dynamic_cast<cPassenger*>((*entities)[i]);

        // Check if passenger is already at its destination
        if(!oPassenger->DestinationEqual(oBusStop))
        {
            // move passenger to the bus door position
            oPassenger->SetTarget(ppBus->GetDoorPosition());

            glm::vec3 oPassengerPos = oPassenger->GetPosition();

            //check if passenger is within a certain radius of the bus door
            float distSquared = ((oBusDoorPos.x - oPassengerPos.x) * (oBusDoorPos.x - oPassengerPos.x)) +
                                ((oBusDoorPos.z - oPassengerPos.z) * (oBusDoorPos.z - oPassengerPos.z));
            if (distSquared < C_PASSENGER_ENTER_DISTANCE * C_PASSENGER_ENTER_DISTANCE && distSquared > 0)
            {
                oBusStop->RemovePassenger(oPassenger);
                ppBus->AddPassenger(oPassenger);
                oPassenger->pbVisible = false;
                i--;
            }
        }
    }
}

// Unload all passengers in the bus into the given bus stop
void cGameLogicHandler::UnloadPassengers(cBusStop *oBusStop)
{
    glm::vec3 oBusDoorPos = ppBus->GetDoorPosition();

    std::vector<IEntity *> *entities;
    ppBus->poEntityGroup->GetEntityList(&entities);

    // Loop through all passengers currently in the bus
    for (uint i = 0; i < ppBus->poEntityGroup->GetEntities()->size(); i++)
    {
        cPassenger* oPassenger = dynamic_cast<cPassenger*>((*entities)[i]);
        // Check if tis busStop is this passengers destination
        if(oPassenger->DestinationEqual(oBusStop))
        {
            oPassenger->SetPosition(ppBus->GetDoorPosition());
            oPassenger->pbVisible = true;
            // remove from bus entityGroup
            ppBus->RemovePassenger(oPassenger);
            // Add to bus stop entity group and set target to the bus stop position
            oBusStop->AddPassenger(oPassenger);
            oPassenger->SetTarget(oBusStop->GetPosition());
            i--;
        }
    }
}

bool cGameLogicHandler::LoadMission()
{
    if(ppMission == nullptr)
        return false;

    std::deque<cBusStop*> oRoute = ppMission->GetRouteQueue();
    int iRouteSize = oRoute.size();

    // TODO probably need to make a global stack
    // Stack for all the passengers currently available
    std::stack<int> oPassengersQueue;
    for (auto& num : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) { //-V112
        oPassengersQueue.push(num);
    }

    // Loop through all busStops on the route
    for(uint i = 0; i < iRouteSize; i++)
    {
        // Check if busStops already have this behaviour
        if(!oRoute[i]->poEntityGroup->BehaviourExists(ppScene->pcbSeperation))
            oRoute[i]->poEntityGroup->AddBehaviour(ppScene->pcbSeperation);
        if(!oRoute[i]->poEntityGroup->BehaviourExists(ppScene->pcbSeeking))
            oRoute[i]->poEntityGroup->AddBehaviour(ppScene->pcbSeeking);

        // The last buStop can not have any passengers
        if(i == iRouteSize - 1)
            break;

        // get random amount of passengers to spawn next to the busStop
        int iPassengerAmount = rand() % (C_MAX_BUS_STOP_PASSENGERS + 1); // rand between 0 and iMaxBusStopPassengers
        // create the random amount of passengers
        for (int j = 0; j < iPassengerAmount; ++j)
        {
            // Check if there are still passengers available to spawn
            if(!oPassengersQueue.empty())
            {
                // create random destination
                int iRandDestIndex = rand() % (iRouteSize - (i + 1))+ (i + 1);

                // Create key for the passenger
                int iKeyNum = oPassengersQueue.top();
                string key = "passenger" + std::to_string(iKeyNum);

                // Check if addPassenger was successful
                if(oRoute[i]->AddPassenger(dynamic_cast<cPassenger*>(pmpObjects[key])))
                {
                    cPassenger* oCurrentPassenger = dynamic_cast<cPassenger*>(pmpObjects[key]);
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

                    oCurrentPassenger->SetTarget(oRoute[i]->GetPosition()); // set target to bus stop
                    oCurrentPassenger->SetDestination(oRoute[iRandDestIndex]); // set destination to random bus stop
                    oCurrentPassenger->pbVisible = true;

                    // pop used passenger form the available passengers queue
                    oPassengersQueue.pop();
                }
            }
        }
    }

    return true;
}

// Unload all passengers currently in the bus and place them back at de default location
void cGameLogicHandler::ResetBus()
{
    std::vector<IEntity *> *entities;
    ppBus->poEntityGroup->GetEntityList(&entities);
    // loop through all entities inside the entityGroup of the bus
    for (int i = 0; i < ppBus->poEntityGroup->GetEntities()->size(); i++)
    {
        (*entities)[i]->SetPosition(C_DEFAULT_PASSENGER_LOCATION);
        (*entities)[i]->pbVisible = false;
    }
    ppBus->poEntityGroup->ClearEntities();
    ppBus->oState = cState::eStill;
}

cMissionHandler* cGameLogicHandler::GetMissionHandler()
{
    return ppMission;
}

bool cGameLogicHandler::SetMissionHandler(cMissionHandler* pMissionHandler)
{
    // Unload the busStops from current missionHandler
    if(ppMission != nullptr)
    {
        ppMission->UnloadMissionHandler();
        ResetBus();
    }

    ppMission = pMissionHandler;
    return true;
}
