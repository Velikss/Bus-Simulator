#include "pch.hpp"
#include <iostream>
#include <Engine.hpp>
#include <vulkan/entities/cBehaviourHandler.hpp>
#include <vulkan/entities/cEntityGroup.hpp>
#include <vulkan/entities/cEntity.hpp>

int main(int argc, char** argv)
{

    // Create static behaviours
    cBehaviourHandler::AddBehaviour("seperation", "src/scripting/seperation.js");
    cBehaviourHandler::AddBehaviour("cohesion", "src/scripting/cohesion.js");

//    cEntityGroup entityGroup;
//    cBehaviourHandler *cbSeperation = new cBehaviourHandler("seperation"); // behaviourname should be a name previously defined as static behaviour
//    cBehaviourHandler *cbCohesion = new cBehaviourHandler("cohesion"); // behaviourname should be a name previously defined as static behaviour
//
//    cEntity en2(nullptr);
//    en2.setPosition(glm::vec3(11.1f, 0, 17.1f));
//    en2.pfMaxSpeed = 10.0f;
//    cEntity en3(nullptr);
//    en3.setPosition(glm::vec3(12.1f, 0, 14.1f));
//    en3.pfMaxSpeed = 10.0f;
//    cEntity en4(nullptr);
//    en4.setPosition(glm::vec3(13.1f, 0, 12.1f));
//    en4.pfMaxSpeed = 10.0f;
//    cEntity en5(nullptr);
//    en5.setPosition(glm::vec3(14.1f, 0, 6.1f));
//    en5.pfMaxSpeed = 10.0f;
//
//    entityGroup.AddEntity(&en2);
//    entityGroup.AddEntity(&en3);
//    entityGroup.AddEntity(&en4);
//    entityGroup.AddEntity(&en5);
//
//    entityGroup.AddBehaviour(cbSeperation);
//    entityGroup.AddBehaviour(cbCohesion);
//
//    for(int i = 0; i<100; i++){
//        entityGroup.UpdateEntities();
//
//        std::cout << " p2 " << en2.getPosition()->x << " - " << en2.getPosition()->y
//                  << "        p3 " << en3.getPosition()->x << " - " << en3.getPosition()->y
//                  << "        p4 " << en4.getPosition()->x << " - " << en4.getPosition()->y
//                  << "        p5 " << en5.getPosition()->x << " - " << en5.getPosition()->y << std::endl;
//    }

    Engine oEngine;

    try
    {
        oEngine.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
