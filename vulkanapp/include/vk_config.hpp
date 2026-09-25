#pragma once

#include "vk_settings.hpp"
#include "vk_present.hpp"
#include "vk_graphics.hpp"
#include "vk_draw.hpp"

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;

/********** SETUP **********/

const std::vector<char const*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                      vk::DebugUtilsMessageTypeFlagsEXT type,
                                                      const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      void* pUserData)
{
    if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
    }

    return vk::False;
}

class VulkanInstance
{
    public:
        void createInstance();
        void setupDebugMessenger();

        const vk::raii::Instance& getInstance() const { return instance; }

    private:
        vk::raii::Context context;
        vk::raii::Instance instance = nullptr;
        vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

        std::vector<const char*> getRequiredInstanceExtensions();
};


const std::vector<const char*> requiredDeviceExtension = {vk::KHRSwapchainExtensionName};

class VulkanDevice
{
    public:
        void pickPhysicalDevice(const vk::raii::Instance& instance, const vk::raii::SurfaceKHR& surface);
        void createLogicalDevice(const vk::raii::SurfaceKHR& surface, uint32_t &queueIndex);

        const vk::raii::PhysicalDevice& getPhysicalDevice() const { return physicalDevice; }
        const vk::raii::Device& getDevice() const { return device; }
        const vk::raii::Queue& getQueue() const { return queue; }

    private:
        vk::raii::PhysicalDevice physicalDevice = nullptr;
        vk::raii::Device device = nullptr;
        vk::raii::Queue queue = nullptr;

        bool isDeviceSuitable(const vk::raii::PhysicalDevice& physicalDevice, const vk::raii::SurfaceKHR& surface);
};

/*********** APP ***********/

class Sirius
{
    public:
        void run();
    
    private:
        GLFWwindow* window = nullptr;
        uint32_t queueIndex = ~0;
        uint32_t frameIndex = 0;
        std::optional<RenderingContext> renderingContext;

        VulkanInstance instance;
        VulkanDevice device;
        VulkanSurface surface;
        VulkanSwapChain swapchain;
        VulkanImageView imageview;
        GraphicsPipeline graphicspipline;
        CommandPool commandpool;
        SynchObject synchobj;
    
        void initWindow();
        void initVulkan();
        void mainLoop(const vk::raii::Device& dev);
        void drawFrame(const vk::raii::Device& dev, const vk::raii::SwapchainKHR& swapChain, const std::vector<vk::raii::CommandBuffer>& commandBuffers, const vk::raii::Queue& queue);
        void cleanup();

        void recreateSwapChain(const vk::raii::PhysicalDevice& physDevice, const vk::raii::Device& dev, const vk::raii::SurfaceKHR& surf, vk::SurfaceFormatKHR swapChainSurfaceFormat, std::vector<vk::Image> swapChainImages, GLFWwindow* win);
        void cleanupSwapChain();
};