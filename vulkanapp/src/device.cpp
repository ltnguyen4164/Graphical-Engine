#include "vk_config.hpp"

void VulkanDevice::pickPhysicalDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface)
{
    std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    auto const devIter = std::ranges::find_if( physicalDevices, [&]( auto const &physicalDevice ) { return isDeviceSuitable( physicalDevice, surface ); } );
    if (devIter == physicalDevices.end())
    {
        throw std::runtime_error( "failed to find GPUs with Vulkan support! ");
    }
    physicalDevice = *devIter;
}

bool VulkanDevice::isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface)
{
    // check if the physical device supports the Vulkan 1.3 API version
    bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

    // check if any of the queue families support graphics operations
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t qfpIndex = 0;
    bool supportsGraphics = std::ranges::any_of(queueFamilies, [&physicalDevice, &surface = surface, &qfpIndex](auto const & qfp) 
        {
            bool const suitable = (qfp.queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface);
            qfpIndex++;
            return suitable;
        });

    // check if all required physical device extensions are available
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions = std::ranges::all_of( requiredDeviceExtension, 
                                                              [&availableDeviceExtensions]( auto const &requiredDeviceExtension )
                                                              {
                                                                return std::ranges::any_of ( availableDeviceExtensions, 
                                                                                             [requiredDeviceExtension]( auto const &availableDeviceExtensions )
                                                                                             { return strcmp( availableDeviceExtensions.extensionName, requiredDeviceExtension ) == 0; } );
                                                              } );
    // check if the physical device supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
    auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
                                                         vk::PhysicalDeviceVulkan11Features,
                                                         vk::PhysicalDeviceVulkan13Features,
                                                         vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
                                    features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
                                    features.template get<vk::PhysicalDeviceVulkan13Features>().synchronization2 &&
                                    features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;
    
    // return true if the physical device meets all the criteria
    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;                                
}

void VulkanDevice::createLogicalDevice(const vk::raii::SurfaceKHR& surface, uint32_t &queueIndex)
{
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both graphics and present
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
    {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) && physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
        {
            // found a queue family that supports both graphics and present
            queueIndex = qfpIndex;
            break;
        }
    }

    if (queueIndex == ~0)
    {
        throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                       vk::PhysicalDeviceVulkan11Features,
                       vk::PhysicalDeviceVulkan13Features,
                       vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {
            {},                                                             // vk::PhysicalDeviceFeatures2
		    {.shaderDrawParameters = true},                                 // vk::PhysicalDeviceVulkan11Features
		    {.synchronization2 = true, .dynamicRendering = true},           // vk::PhysicalDeviceVulkan13Features
		    {.extendedDynamicState = true}                                  // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
        };
    
    //create a device
    float queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{.queueFamilyIndex = queueIndex, 
                                                    .queueCount = 1, 
                                                    .pQueuePriorities = &queuePriority};
    vk::DeviceCreateInfo deviceCreateInfo{.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
                                          .queueCreateInfoCount = 1,
                                          .pQueueCreateInfos = &deviceQueueCreateInfo,
                                          .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
                                          .ppEnabledExtensionNames = requiredDeviceExtension.data()};
    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queue = vk::raii::Queue(device, queueIndex, 0);
}