#include "vk_present.hpp"

void VulkanImageView::createImageViews(vk::SurfaceFormatKHR swapChainSurfaceFormat, std::vector<vk::Image> swapChainImages, const vk::raii::Device& device)
{
    assert(swapChainImageViews.empty());

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .viewType = vk::ImageViewType::e2D,
        .format = swapChainSurfaceFormat.format,
        .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
    };

    // iterate over all swap chain images and add them to the structure
    for (auto &image : swapChainImages)
    {
        imageViewCreateInfo.image = image;
        swapChainImageViews.emplace_back(device, imageViewCreateInfo);
    }
}