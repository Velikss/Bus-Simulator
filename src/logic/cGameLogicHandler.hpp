#pragma once

#include <pch.hpp>
#include <vulkan/entities/cEntity.hpp>
#include <vulkan/scene/Scene.hpp>
#include <entities/cBus.hpp>
#include <logic/cMissionHandler.hpp>
#include <overlay/InGame.hpp>

const float C_PASSENGER_ENTER_DISTANCE = 1.5f;
const int C_MAX_BUS_STOP_PASSENGERS = 3;

class cGameLogicHandler
{
private:
    std::shared_ptr<cMissionHandler> ppMission = nullptr;
    cBusStop* poCurrentBusStop = nullptr;
    cInGame* poInGameOverlay = nullptr;
    bool pbMissionLoaded = false;
public:
    std::map<string, std::shared_ptr<cMissionHandler>> pmpMissions;

    void Update(cBus* pBus);
    std::shared_ptr<cMissionHandler> GetMissionHandler();
    bool LoadMission(string sKey, cScene* pScene);
    void LoadPassengers(cBusStop* oBusStop, cBus* pBus);
    void UnloadPassengers(cBusStop* oBusStop, cBus* pBus);
    void ResetBus(cBus* pBus);
    void SetGameOverlay(cInGame* oGameOverlay);
};

void cGameLogicHandler::Update(cBus* pBus)
{
    // Check if mission is given and is loaded
    if(ppMission != nullptr && pbMissionLoaded) {
        // update all passenger entities with their behaviours
        ppMission->Update();

        if (pBus->oState == cState::eStill) {
            if (pBus->pfCurrentSpeed > 0.0f)
            {
                pBus->oState = cState::eDriving;
                pBus->CloseDoors();
            }
        }
        if (pBus->oState == cState::eDriving) {
            if (pBus->pfCurrentSpeed == 0.0f) {
                // Get bus stop that is within the radius
                cBusStop *oCurrentBusStop = ppMission->BusStopWithinRadius(pBus->GetDoorPosition());

                // Check if a bus stop was close enough
                if (oCurrentBusStop == nullptr) {
                    pBus->oState = cState::eStill;
                }
                else {
                    pBus->oState = cState::eUnloading;
                    poCurrentBusStop = oCurrentBusStop;
                }
            }
        }
        if (pBus->oState == cState::eUnloading) {
            // Makes nothing happen when player is not at the correct bus stop
            if(poCurrentBusStop != (ppMission->GetRouteQueue().front()))
                pBus->oState = cState::eStill;
            else
            {
                if (pBus->pbDoorOpen)
                {
                    UnloadPassengers(poCurrentBusStop, pBus);
                    pBus->oState = cState::eLoading;
                }
                else if (pBus->pfCurrentSpeed > 0.0f)
                    pBus->oState = cState::eDriving;
            }
        }
        if (pBus->oState == cState::eLoading) {
            LoadPassengers(poCurrentBusStop, pBus);
            // go to state eStill if bus stop has no more passengers available
            if (!ppMission->PassengersAvailable(poCurrentBusStop))
            {
                std::deque<cBusStop*>* oRoute = &ppMission->GetRouteQueue();
                // At the last bu stop the player is told that the mission is completed
                // otherwise its the name of the next bus stop
                if(oRoute->size() == 1)
                    poInGameOverlay->SetNextStopName("Mission Completed");
                else
                {
                    oRoute->pop_front();
                    poInGameOverlay->SetNextStopName(oRoute->front()->psName);
                }
                pBus->oState = cState::eStill;
            }
        }
    }
}

// Load al passengers from the given bus stop onto the bus
void cGameLogicHandler::LoadPassengers(cBusStop* oBusStop, cBus* pBus)
{
    glm::vec3 oBusDoorPos = pBus->GetDoorPosition();

    std::vector<IEntity*>* entities;
    oBusStop->poEntityGroup->GetEntityList(&entities);

    // Loop through all passengers currently in the bus stop
    for (uint i = 0; i < oBusStop->poEntityGroup->GetEntities()->size(); i++)
    {
        cPassenger* oPassenger = dynamic_cast<cPassenger*>((*entities)[i]);

        // Check if passenger is already at its destination
        if(!oPassenger->DestinationEqual(oBusStop))
        {
            // move passenger to the bus door position
            oPassenger->SetTarget(pBus->GetDoorPosition());

            glm::vec3 oPassengerPos = oPassenger->GetPosition();

            //check if passenger is within a certain radius of the bus door
            float distSquared = ((oBusDoorPos.x - oPassengerPos.x) * (oBusDoorPos.x - oPassengerPos.x)) +
                                ((oBusDoorPos.z - oPassengerPos.z) * (oBusDoorPos.z - oPassengerPos.z));
            if (distSquared < C_PASSENGER_ENTER_DISTANCE * C_PASSENGER_ENTER_DISTANCE && distSquared > 0)
            {
                oBusStop->RemovePassenger(oPassenger);
                pBus->AddPassenger(oPassenger);
                oPassenger->pbVisible = false;
                i--;
            }
        }
    }
}

// Unload all passengers in the bus into the given bus stop
void cGameLogicHandler::UnloadPassengers(cBusStop *oBusStop, cBus* pBus)
{
    glm::vec3 oBusDoorPos = pBus->GetDoorPosition();

    std::vector<IEntity*>* entities;
    pBus->poEntityGroup->GetEntityList(&entities);

    // Loop through all passengers currently in the bus
    for (uint i = 0; i < pBus->poEntityGroup->GetEntities()->size(); i++)
    {
        cPassenger* oPassenger = dynamic_cast<cPassenger*>((*entities)[i]);
        // Check if tis busStop is this passengers destination
        if(oPassenger->DestinationEqual(oBusStop))
        {
            oPassenger->SetPosition(pBus->GetDoorPosition());
            oPassenger->pbVisible = true;
            // remove from bus entityGroup
            pBus->RemovePassenger(oPassenger);
            // Add to bus stop entity group and set target to the bus stop position
            oBusStop->AddPassenger(oPassenger);
            oPassenger->SetTarget(oBusStop->GetPosition());
            i--;
        }
    }
}

// Unload all passengers currently in the bus and place them back at de default location
void cGameLogicHandler::ResetBus(cBus* pBus)
{
    std::vector<IEntity *>* entities;
    pBus->poEntityGroup->GetEntityList(&entities);
    // loop through all entities inside the entityGroup of the bus
    for (int i = 0; i < pBus->poEntityGroup->GetEntities()->size(); i++)
    {
        (*entities)[i]->SetPosition(C_DEFAULT_PASSENGER_LOCATION);
        (*entities)[i]->pbVisible = false;
    }
    pBus->poEntityGroup->ClearEntities();
    pBus->oState = cState::eStill;
}

std::shared_ptr<cMissionHandler> cGameLogicHandler::GetMissionHandler()
{
    return ppMission;
}

bool cGameLogicHandler::LoadMission(string sKey, cScene* pScene)
{
    // Unload the busStops from current missionHandler and the bus
    if(ppMission != nullptr)
    {
        ppMission->UnloadRouteBusStops();
        ResetBus(dynamic_cast<cBus*>(pScene->GetObjects()["bus"]));
        ppMission->RefillActiveRouteQueue();
    }

    ppMission = pmpMissions[sKey];
    pbMissionLoaded = false;

    if(ppMission == nullptr)
        return false;

    std::deque<cBusStop*> oRoute = ppMission->GetRouteQueue();
    auto iRouteSize = oRoute.size();

    // Set Ingame overlay to first bus stop
    poInGameOverlay->SetNextStopName(oRoute.front()->psName);

    // TODO probably need to make a global stack
    // Stack for all the passengers currently available
    std::stack<int> oPassengersQueue;
    for (auto& num : {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}) { //-V112
        oPassengersQueue.push(num);
    }

    // Loop through all busStops on the route
    for (uint i = 0; i < iRouteSize; i++)
    {
        // Check if busStops already have this behaviour
        if(!oRoute[i]->poEntityGroup->BehaviourExists(pScene->pcbSeperation))
            oRoute[i]->poEntityGroup->AddBehaviour(pScene->pcbSeperation);
        if(!oRoute[i]->poEntityGroup->BehaviourExists(pScene->pcbSeeking))
            oRoute[i]->poEntityGroup->AddBehaviour(pScene->pcbSeeking);

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
                if(oRoute[i]->AddPassenger(dynamic_cast<cPassenger*>(pScene->GetObjects()[key])))
                {
                    cPassenger* oCurrentPassenger = dynamic_cast<cPassenger*>(pScene->GetObjects()[key]);
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
    pbMissionLoaded = true;
    return true;
}

void cGameLogicHandler::SetGameOverlay(cInGame *oGameOverlay)
{
    poInGameOverlay = oGameOverlay;
}
