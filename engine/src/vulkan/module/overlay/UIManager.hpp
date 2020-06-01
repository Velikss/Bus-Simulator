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
    std::vector<VkBuffer> patBuffers;
    std::vector<VkDeviceMemory> patBufferMemory;
    std::vector<void*> papMappedMemory;

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

    void CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer, tElementObject* pElement, uint uiIndex);

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
    uint uiCount = pElement->ppElement->GetChildCount();
    pElement->patBuffers.resize(uiCount);
    pElement->patBufferMemory.resize(uiCount);
    pElement->papMappedMemory.resize(uiCount);

    for (uint uiIndex = 0; uiIndex < uiCount; uiIndex++)
    {
        cBufferHelper::CreateBuffer(ppLogicalDevice, pElement->ppElement->GetMemorySize(uiIndex),
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    pElement->patBuffers[uiIndex], pElement->patBufferMemory[uiIndex]);


        ppLogicalDevice->MapMemory(pElement->patBufferMemory[uiIndex], 0,
                                   pElement->ppElement->GetMemorySize(uiIndex),
                                   0, &pElement->papMappedMemory[uiIndex]);
    }
}

void cUIManager::FreeElementMemory(tElementObject* pElement)
{
    for (uint uiIndex = 0; uiIndex < pElement->patBuffers.size(); uiIndex++)
    {
        ppLogicalDevice->UnmapMemory(pElement->patBufferMemory[uiIndex]);
        ppLogicalDevice->FreeMemory(pElement->patBufferMemory[uiIndex], nullptr);
        ppLogicalDevice->DestroyBuffer(pElement->patBuffers[uiIndex], nullptr);
    }
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
                for (uint uiIndex = 0; uiIndex < tElement.patBuffers.size(); uiIndex++)
                {
                    tElement.ppElement->FillMemory(tElement.papMappedMemory[uiIndex], uiIndex);
                }
                tElement.ppElement->Validate();
                uiCount++;
            }
        }

        ppCommandBufferHolder->GetCommandBuffers()[2]->RecordBuffers(ppCommandRecorder->GetCommandRecorder());

        Validate();

        ENGINE_LOG("Updated " << uiCount << " UI elements");
    }
}

void cUIManager::CmdBindVertexBuffer(VkCommandBuffer& oCommandBuffer, tElementObject* pElement, uint uiIndex)
{
    VkDeviceSize ulOffset = 0;
    vkCmdBindVertexBuffers(oCommandBuffer, 0, 1, &pElement->patBuffers[uiIndex], &ulOffset);
}
