#pragma once

#include "vk_settings.hpp"

class VulkanSurface
{
    public:
        void createSurface(const vk::raii::Instance& instance, GLFWwindow* window);

        const vk::raii::SurfaceKHR& getSurface() const { return surface; }

    private:
        vk::raii::SurfaceKHR surface = nullptr;
};


class VulkanSwapChain
{
    public:
        void createSwapChain(const vk::raii::PhysicalDevice& physDevice, const vk::raii::Device& device, const vk::raii::SurfaceKHR& surface, GLFWwindow* win);

        const vk::raii::SwapchainKHR& getSwapChain() { return swapChain; }
        const std::vector<vk::Image>& getSwapChainImage() { return swapChainImages; }
        const vk::SurfaceFormatKHR getSwapChainSurfaceFormat() { return swapChainSurfaceFormat; }
        const vk::Extent2D getSwapChainExtent() { return swapChainExtent; }

    private:
        GLFWwindow* window = nullptr;

        vk::raii::SwapchainKHR swapChain = nullptr;
        std::vector<vk::Image> swapChainImages;
        vk::SurfaceFormatKHR swapChainSurfaceFormat;
        vk::Extent2D swapChainExtent;
        std::vector<vk::raii::ImageView> swapChainImageViews;

        vk::SurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<vk::SurfaceFormatKHR> const &availableFormats);
        vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const &availablePresentModes);
        vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const &capabilities);
        uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const & surfaceCapabilities);
};


class VulkanImageView
{
    public:
        void createImageViews(vk::SurfaceFormatKHR swapChainSurfaceFormat, std::vector<vk::Image> swapChainImages, const vk::raii::Device& device);

        const std::vector<vk::raii::ImageView>& getSwapChainImageViews() { return swapChainImageViews; }
        
        private:
        std::vector<vk::raii::ImageView> swapChainImageViews;
};