#pragma once

#include <pch.hpp>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cstring>
#include <optional>

// A struct for storing the QueueFamily indices
struct tQueueFamilyIndices
{
    std::optional<uint32_t> oulGraphicsFamily;

    // Return true if the oulGraphicsFamily optional has a value
    bool IsComplete()
    {
        return oulGraphicsFamily.has_value();
    }
};

class Engine
{
public:
    // Window size
    const uint WIDTH = 800;
    const uint HEIGHT = 600;

    // Validation layers are hooks inside of Vulkan that can be used for debugging
    const std::vector<const char *> VALIDATION_LAYERS = {
    };

    // Disable validation layers when not debugging
#ifdef NDEBUG
    const bool ENABLE_VALIDATION_LAYERS = false;
#else
    const bool ENABLE_VALIDATION_LAYERS = true;
#endif

    // Initializes and starts the engine and all of it's sub-components
    void Run()
    {
        InitWindow();
        InitVulkan();
        MainLoop();
        Cleanup();
    }

private:
    // Pointer to the GLFW window instance
    GLFWwindow *ppWindow;

    // Main Vulkan instance object
    VkInstance poInstance;

    // Selected physical graphics device
    VkPhysicalDevice poPhysicalDevice = VK_NULL_HANDLE;

    // Logical device handle
    VkDevice poDevice;

    // Handle for interfacing with the graphics queue
    VkQueue poGraphicsQueue;

    // Initializes and creates the GLFW window
    void InitWindow()
    {
        glfwInit();

        // We don't want to create an OpenGL context, so specify NO_API
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // Window resizing is temporarily disabled
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        ppWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    // Initializes and creates the Vulkan instance
    void InitVulkan()
    {
        // Create the Vulkan instance
        CreateInstance();

        // Select a physical graphics device
        PickPhysicalDevice();

        // Create the logical device
        CreateLogicalDevice();
    }

    // Creates the Vulkan instance
    void CreateInstance()
    {
        // If validation layers are enabled, check if they are supported
        if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        // Struct with information about our application (optional)
        VkApplicationInfo tAppInfo = {};
        tAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        // Name + version of our application
        tAppInfo.pApplicationName = "Vulkan Engine";
        tAppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);

        // Can be used to tell Vulkan what engine we're using
        tAppInfo.pEngineName = "No Engine";
        tAppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        // Vulkan API version
        tAppInfo.apiVersion = VK_API_VERSION_1_0;

        // Struct with information for creating the Vulkan instance
        VkInstanceCreateInfo tCreateInfo = {};
        tCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        tCreateInfo.pApplicationInfo = &tAppInfo;

        // Get the extensions required for GLFW
        uint32_t ulGLFWExtensionCount = 0;
        const char **ppGLFWExtensions = glfwGetRequiredInstanceExtensions(&ulGLFWExtensionCount);

        tCreateInfo.enabledExtensionCount = ulGLFWExtensionCount;
        tCreateInfo.ppEnabledExtensionNames = ppGLFWExtensions;

        if (ENABLE_VALIDATION_LAYERS)
        {
            // If validation layers are enabled, set the validation layers
            tCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            tCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        }
        else
        {
            // If validation layers are disabled, set the count to 0
            tCreateInfo.enabledLayerCount = 0;
        }

        // Create the Vulkan instance, and throw an error on failure
        if (vkCreateInstance(&tCreateInfo, nullptr, &poInstance) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void PickPhysicalDevice()
    {
        // Get the number of graphics devices with Vulkan support
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(poInstance, &deviceCount, nullptr);

        // If none found, throw an error
        if (deviceCount == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        // Get all the devices with Vulkan support
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(poInstance, &deviceCount, devices.data());

        // Set physicalDevice to the first device that is suitable
        for (const auto &device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                poPhysicalDevice = device;
                break;
            }
        }

        // If no suitable device is found, throw an error
        if (poPhysicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void CreateLogicalDevice() {
        // Get supported QueueFamilies from the physical device
        tQueueFamilyIndices tQueueIndices = FindQueueFamilies(poPhysicalDevice);

        // Struct with information about for creating the Queue
        VkDeviceQueueCreateInfo tQueueCreateInfo = {};
        tQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        tQueueCreateInfo.queueCount = 1;

        // Set the family index
        tQueueCreateInfo.queueFamilyIndex = tQueueIndices.oulGraphicsFamily.value();

        // Priority is required even with only a single queue
        float fQueuePriority = 1.0f;
        tQueueCreateInfo.pQueuePriorities = &fQueuePriority;

        // Struct with information about the physical device features we want to enable
        VkPhysicalDeviceFeatures tDeviceFeatures = {};

        // Struct with info for creating the logical device
        VkDeviceCreateInfo tCreateInfo = {};
        tCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        // Set the queue create info
        tCreateInfo.pQueueCreateInfos = &tQueueCreateInfo;
        tCreateInfo.queueCreateInfoCount = 1;

        // Set the device features we want to enable
        tCreateInfo.pEnabledFeatures = &tDeviceFeatures;

        // We don't have any device specific extensions right now
        tCreateInfo.enabledExtensionCount = 0;

        // Setup validation layers if they are enabled
        if (ENABLE_VALIDATION_LAYERS) {
            tCreateInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            tCreateInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();
        } else {
            tCreateInfo.enabledLayerCount = 0;
        }

        // Create the logical device, and throw an error on failure
        if (vkCreateDevice(poPhysicalDevice, &tCreateInfo, nullptr, &poDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        // Get the graphics queue handle from the logical device
        vkGetDeviceQueue(poDevice, tQueueIndices.oulGraphicsFamily.value(), 0, &poGraphicsQueue);
    }

    void MainLoop()
    {
        // Keep the main loop running until the window should be closed
        while (!glfwWindowShouldClose(ppWindow))
        {
            glfwPollEvents();
        }
    }

    void Cleanup()
    {
        // Destroy the logical device
        vkDestroyDevice(poDevice, nullptr);

        // Destroy the Vulkan instance (implicitly destroys oPhysicalDevice)
        vkDestroyInstance(poInstance, nullptr);

        // Destroy the GLFW window
        glfwDestroyWindow(ppWindow);

        // Terminate GLFW
        glfwTerminate();
    }

    // Checks if all the layers in VALIDATION_LAYERS are supported
    bool CheckValidationLayerSupport()
    {
        // Get the amount of available layers
        uint32_t ulLayerCount;
        vkEnumerateInstanceLayerProperties(&ulLayerCount, nullptr);

        // Get all the available layers
        std::vector<VkLayerProperties> availableLayers(ulLayerCount);
        vkEnumerateInstanceLayerProperties(&ulLayerCount, availableLayers.data());

        // Loop over all the layers in VALIDATION_LAYERS and if we find one that's not available, return false
        for (const char *layerName : VALIDATION_LAYERS)
        {
            bool layerFound = false;
            for (const auto &layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) return false;
        }

        return true;
    }

    tQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice oDevice)
    {
        // Get the amount of supported QueueFamilies
        uint32_t ulQueueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, nullptr);

        // Get all the supported QueueFamilies
        std::vector<VkQueueFamilyProperties> atQueueFamilies(ulQueueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(oDevice, &ulQueueFamilyCount, atQueueFamilies.data());

        tQueueFamilyIndices tIndices;

        // Find one family that supports VK_QUEUE_GRAPHICS_BIT and store it in tIndices
        int iIndex = 0;
        for (const auto &tQueueFamily : atQueueFamilies)
        {
            if (tQueueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                tIndices.oulGraphicsFamily = iIndex;
            }
            iIndex++;
        }

        return tIndices;
    }

    bool IsDeviceSuitable(VkPhysicalDevice oDevice)
    {
        // Device properties include things like name, type and versions
        VkPhysicalDeviceProperties tDeviceProperties;
        vkGetPhysicalDeviceProperties(oDevice, &tDeviceProperties);
        // Device features include all the features supported by this device
        VkPhysicalDeviceFeatures tDeviceFeatures;
        vkGetPhysicalDeviceFeatures(oDevice, &tDeviceFeatures);

        tQueueFamilyIndices indices = FindQueueFamilies(oDevice);

        // Here we can check if the device is suitable and has the features we need
        // Right now we only care about a supported QueueFamily
        return indices.IsComplete();
    }
};
