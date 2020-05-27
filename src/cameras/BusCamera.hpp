#pragma once

#include <pch.hpp>
#include <entities/cBus.hpp>
#include "vulkan/scene/Camera.hpp"

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

    void MovePivotX(float fMultiplier)
    {
        glm::vec3 direction(sin(glm::radians(cameraPivotObject->GetRotation().y)), 0, cos(glm::radians(
                cameraPivotObject->GetRotation().y)));
        cameraPivotPos -= (direction * fMultiplier);
    }

    void MovePivotY(float fMultiplier)
    {
        cameraPivotPos.y += fMultiplier;
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
        yaw -= mouseSpeed * x;
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
        // calculate new cameraPivot with cameraPivotChanges
        cameraPivotPos = cameraPivotObject->GetPosition();
        MovePivotX(cameraPivotChanges.x);
        MovePivotY(cameraPivotChanges.y);
        // Todo move pivot along the Z (sideways)

        // camera rotation
        if(yaw >= 360)
            yaw = 0.1;
        if(yaw <= 0)
            yaw = 359.9;
        // rotate camera when current speed is higher then 8
        if(dynamic_cast<cBus *>(cameraPivotObject)->pfCurrentSpeed > 8) {
            float rotation = cameraPivotObject->GetRotation().y;
            float rotationDifference = rotation - yaw;
            // difference is positive
            if (rotationDifference > 0) {
                // rotate faster when further away from the back of the bus
                if (rotationDifference > 5.0)
                {
                    if (rotationDifference < 180)
                        yaw += 0.5;
                    else
                        yaw -= 0.5;
                }
                // rotate slower when closer to the back of the bus
                else if (rotationDifference > 1.0)
                {
                    if (rotationDifference < 180)
                        yaw += 0.1;
                    else
                        yaw -= 0.1;
                }
            }
            // difference is negative
            else if (rotationDifference < 0) {
                // rotate faster when further away from the back of the bus
                if ((-1 * rotationDifference) > 5.0)
                {
                    if ((-1 * rotationDifference) < 180)
                        yaw -= 0.5;
                    else
                        yaw += 0.5;
                }
                // rotate slower when closer to the back of the bus
                else if ((-1 * rotationDifference) > 1.0)
                {
                    if ((-1 * rotationDifference) < 180)
                        yaw -= 0.1;
                    else
                        yaw += 0.1;
                }
            }
        }
        // calculate camera position
        cameraPos.x = sin(glm::radians(yaw)) * orbitDistance + cameraPivotPos.x;
        cameraPos.y = cameraPivotPos.y + cameraHeight;
        cameraPos.z = cos(glm::radians(yaw)) * orbitDistance + cameraPivotPos.z;

        // calculate lookat variables
        cameraFront = glm::normalize((cameraPivotPos) - cameraPos);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraFront));
        cameraUp = glm::normalize(glm::cross(cameraFront, cameraRight));

        view = glm::lookAt(cameraPos, glm::vec3(cameraPivotPos.x,
                                                cameraPivotPos.y,
                                                cameraPivotPos.z) + cameraFront, cameraUp);
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



