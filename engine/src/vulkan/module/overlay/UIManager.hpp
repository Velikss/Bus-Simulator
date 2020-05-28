#pragma once

#include <pch.hpp>
#include <vulkan/vulkan.h>
#include <vulkan/LogicalDevice.hpp>
#include <vulkan/module/overlay/element/StaticElement.hpp>
#include <vulkan/util/CommandBufferHolder.hpp>
#include <vulkan/util/CommandRecorderProvider.hpp>

struct tElementObject
{
    cUIElement* ppElement = nullptr;
    VkBuffer poBuffer = VK_NULL_HANDLE;
    VkDeviceMemory poBufferMemory = VK_NULL_HANDLE;
    void* ppMappedMemory = nullptr;

    tElementObject(cUIElement* ppElement) : ppElement(ppElement)
    {}
};

class cUIManager : public cInvalidatable
{
private:
    cLogicalDevice* ppLogicalDevice = nullptr;
    iCommandBufferHolder* ppCommandBufferHolder = nullptr;
    iCommandRecorderProvider* ppCommandRecorder = nullptr;

public:
    std::vector<tElementObject> patElements;

    cUIManager(cLogicalDevice* pLogicalDevice,
               iCommandBufferHolder* pCommandBufferHolder,
               iCommandRecorderProvider* pCommandRecorder);
    ~cUIManager();

    void Update();

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer, tElementObject* pElement);

public:
    void AllocateElementsMemory();
    void AllocateElementMemory(tElementObject* pElement);
    void FreeElementMemory(tElementObject* pElement);
};

cUIManager::cUIManager(cLogicalDevice* pLogicalDevice,
                       iCommandBufferHolder* pCommandBufferHolder,
                       iCommandRecorderProvider* pCommandRecorder)
{
    assert(pLogicalDevice != nullptr);
    assert(pCommandBufferHolder != nullptr);
    assert(pCommandRecorder != nullptr);

    ppLogicalDevice = pLogicalDevice;
    ppCommandBufferHolder = pCommandBufferHolder;
    ppCommandRecorder = pCommandRecorder;
}

cUIManager::~cUIManager()
{
    for (auto& tElement : patElements)
    {
        FreeElementMemory(&tElement);
    }
}

void cUIManager::AllocateElementsMemory()
{
    for (auto& tElement : patElements)
    {
        tElement.ppElement->OnLoadVertices();
        AllocateElementMemory(&tElement);
    }
}

void cUIManager::AllocateElementMemory(tElementObject* pElement)
{
    cBufferHelper::CreateBuffer(ppLogicalDevice, pElement->ppElement->GetMemorySize(),
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                pElement->poBuffer, pElement->poBufferMemory);


    ppLogicalDevice->MapMemory(pElement->poBufferMemory, 0,
                               pElement->ppElement->GetMemorySize(),
                               0, &pElement->ppMappedMemory);
}

void cUIManager::FreeElementMemory(tElementObject* pElement)
{
    ppLogicalDevice->UnmapMemory(pElement->poBufferMemory);
    ppLogicalDevice->FreeMemory(pElement->poBufferMemory, nullptr);
    ppLogicalDevice->DestroyBuffer(pElement->poBuffer, nullptr);
}

void cUIManager::Update()
{
    if (Invalidated())
    {
        ENGINE_LOG("UI manager invalidated, updating...");

        ppLogicalDevice->WaitUntilIdle();

        uint uiCount = 0;
        for (auto& tElement : patElements)
        {
            if (tElement.ppElement->Invalidated())
            {
                tElement.ppElement->FillMemory(tElement.ppMappedMemory);
                tElement.ppElement->Validate();
                uiCount++;
            }
        }

        ppCommandBufferHolder->GetCommandBuffers()[2]->RecordBuffers(ppCommandRecorder->GetCommandRecorder());

        Validate();

        ENGINE_LOG("Updated " << uiCount << " UI elements");
    }
}

void cUIManager::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer, tElementObject* pElement)
{
    VkDeviceSize ulOffset = 0;
    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, &pElement->poBuffer, &ulOffset);
}
