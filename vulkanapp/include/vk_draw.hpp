#pragma once

#include "vk_settings.hpp"

struct RenderingContext {
    const vk::raii::Device& device;
    const std::vector<vk::Image>& swapChainImages;
    const std::vector<vk::raii::ImageView>& swapChainImageViews;
    vk::Extent2D swapChainExtent;
    const vk::raii::Pipeline& graphicsPipeline;
};

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class CommandPool
{
    public:
        void createCommandPool(uint32_t& queueIndex, const vk::raii::Device& device);
        void createCommandBuffer(const vk::raii::Device& device);
        void recordCommandBuffer(uint32_t frameIndex, uint32_t imageIndex);
        void setRenderingContext(const RenderingContext& context) { renderingContext = &context; }

        const std::vector<vk::raii::CommandBuffer>& getCommandBuffer() { return commandBuffers; }

    private:
        vk::raii::CommandPool commandPool = nullptr;
        std::vector<vk::raii::CommandBuffer> commandBuffers;
        const RenderingContext* renderingContext = nullptr;

        void transition_image_layout(uint32_t frameIndex, uint32_t imageIndex, vk::ImageLayout old_layout, vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask, vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask);
};

class SynchObject
{
    public:
        void createSyncObjects(const vk::raii::Device& device, std::vector<vk::Image> swapChainImages);
    
        std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
        std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
        std::vector<vk::raii::Fence> inFlightFences;
        vk::raii::Fence drawFence = nullptr;
};