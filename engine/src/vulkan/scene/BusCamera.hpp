#pragma once

#include <pch.hpp>
#include "scenes/Camera.hpp"

class BusCamera : public Camera
{

public:

    void Forward()
    {
        if (!lockMovement)
            cameraPos += cameraSpeed * cameraFront;
    }

    void BackWard()
    {
        if (!lockMovement)
            cameraPos -= cameraSpeed * cameraFront;
    }

    void MoveLeft()
    {
        if (!lockMovement)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void MoveRight()
    {
        if (!lockMovement)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    void MoveUp()
    {
        if (!lockMovement)
            if (cameraHeight < 20.0f)
                cameraHeight += cameraSpeed;
    }

    void MoveDown()
    {
        if (!lockMovement)
            if (cameraHeight > 1.0f)
                cameraHeight -= cameraSpeed;
    }

    void MovePivotForward(float fMultiplier)
    {
        glm::vec3 direction(sin(glm::radians(cameraPivotObject->getRotation()->y)), 0, cos(glm::radians(cameraPivotObject->getRotation()->y)));
        cameraPivotPos -= (direction * fMultiplier);
    }

    void MovePivotBackward(float fMultiplier)
    {
        glm::vec3 direction(sin(glm::radians(cameraPivotObject->getRotation()->y)), 0, cos(glm::radians(cameraPivotObject->getRotation()->y)));
        cameraPivotPos += (direction * fMultiplier);
    }

    void LookUp()
    {
        pitch += cameraSpeedLook;
    }

    void LookDown()
    {
        pitch -= cameraSpeedLook;
    }

    void LookLeft()
    {
        yaw -= cameraSpeedLook;
    }

    void LookRight()
    {
        yaw += cameraSpeedLook;
    }

    // is the end op the passthrough from the mouse input.
    void LookMouseDiff(int x, int y)
    {
        yaw += mouseSpeed * x;
        pitch -= mouseSpeed * y;
        // no flipping to the other side
        if (pitch < -80)
            pitch = -80;
        if (pitch > 80)
            pitch = 80;
    }

    void LookMouseWheelDiff(float x, float y)
    {
        if (y < 0 && orbitDistance < 55.0f)
        {
            orbitDistance += cameraScrollSpeed;
        }
        if (y > 0 && orbitDistance > 20.0f)
        {
            orbitDistance -= cameraScrollSpeed;
        }
    }

    // process the commits to the pv.
    void ProcessUpdates()
    {
        cameraPivotPos = *cameraPivotObject->getPosition();
        // Todo make pivot changes property
        MovePivotForward(2.0f);

        cameraPos.x = sin(glm::radians(yaw)) * orbitDistance + cameraPivotPos.x;
        cameraPos.y = cameraPivotPos.y + cameraHeight;
        cameraPos.z = cos(glm::radians(yaw)) * orbitDistance + cameraPivotPos.z;


        cameraFront = glm::normalize((cameraPivotPos) - cameraPos);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));
        cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

        view = glm::lookAt(cameraPos, glm::vec3(cameraPivotPos.x,
                                                cameraPivotPos.y,
                                                cameraPivotPos.z) + cameraFront, cameraUp);

        //TODO move camera pivot with bus rotation
//        cameraPos.x = sin(glm::radians(yaw)) * orbitDistance + cameraPivotObject->getPosition()->x + cameraPivotChanges.x;
//        cameraPos.y = cameraPivotObject->getPosition()->y + cameraHeight + cameraPivotChanges.y;
//        cameraPos.z = cos(glm::radians(yaw)) * orbitDistance + cameraPivotObject->getPosition()->z + cameraPivotChanges.z;
//
//
//        cameraFront = glm::normalize((*cameraPivotObject->getPosition() + cameraPivotChanges) - cameraPos);
//        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
//        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));
//        cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));
//
//        view = glm::lookAt(cameraPos, glm::vec3(cameraPivotObject->getPosition()->x + cameraPivotChanges.x,
//                                                cameraPivotObject->getPosition()->y + cameraPivotChanges.y,
//                                                cameraPivotObject->getPosition()->z + cameraPivotChanges.z) + cameraFront, cameraUp);
//        view = glm::lookAt(cameraPos, *cameraPivotObject + cameraPivotChanges + cameraFront, cameraUp);
    }

    glm::mat4& GetViewMatrix()
    {
        return this->view;
    }

    glm::vec3 GetPosition()
    {
        return this->cameraPos;
    }

    void SetPosition(glm::vec3& position)
    {
        this->cameraPos = position;
    }

    glm::vec3 GetFront()
    {
        return this->cameraFront;
    }

    void SetFront(glm::vec3& front)
    {
        this->cameraFront = front;
    }

    float GetPitch()
    {
        return this->pitch;
    }

    void SetPitch(float pitch)
    {
        this->pitch = pitch;
    }

    float GetYaw()
    {
        return this->yaw;
    }

    void SetYaw(float yaw)
    {
        this->yaw = yaw;
    }
};



