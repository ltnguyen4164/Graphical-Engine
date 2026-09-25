#include "vk_config.hpp"

void Sirius::run()
{
    initWindow();
    initVulkan();
    mainLoop(device.getDevice());
    cleanup();
}

void Sirius::initWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Sirius", nullptr, nullptr);
}

void Sirius::initVulkan()
{
    instance.createInstance();
    instance.setupDebugMessenger();
    
    surface.createSurface(instance.getInstance(), window); 
    
    device.pickPhysicalDevice(instance.getInstance(), surface.getSurface());
    device.createLogicalDevice(surface.getSurface(), queueIndex);
    
    swapchain.createSwapChain(device.getPhysicalDevice(), device.getDevice(), surface.getSurface(), window);
    
    imageview.createImageViews(swapchain.getSwapChainSurfaceFormat(), swapchain.getSwapChainImage(), device.getDevice());
    
    graphicspipline.createGraphicsPipeline(device.getDevice(), swapchain.getSwapChainExtent(), swapchain.getSwapChainSurfaceFormat());
    
    renderingContext.emplace(RenderingContext{
        device.getDevice(),
        swapchain.getSwapChainImage(),
        imageview.getSwapChainImageViews(),
        swapchain.getSwapChainExtent(),
        graphicspipline.getGraphicsPipeline()
    });
    commandpool.setRenderingContext(*renderingContext);
    
    commandpool.createCommandPool(queueIndex, device.getDevice());
    commandpool.createCommandBuffer(device.getDevice());

    synchobj.createSyncObjects(device.getDevice(), swapchain.getSwapChainImage());
}

void Sirius::mainLoop(const vk::raii::Device& dev)
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        drawFrame(device.getDevice(), swapchain.getSwapChain(), commandpool.getCommandBuffer(), device.getQueue());
    }

    dev.waitIdle();
}

void Sirius::drawFrame(const vk::raii::Device& dev, const vk::raii::SwapchainKHR& swapChain, const std::vector<vk::raii::CommandBuffer>& commandBuffers, const vk::raii::Queue& queue)
{   
    auto fenceResult = dev.waitForFences(*synchobj.inFlightFences[frameIndex], vk::True, UINT64_MAX);
    if (fenceResult != vk::Result::eSuccess)
    {
        throw std::runtime_error("Failed to wait for fence");
    }
    dev.resetFences(*synchobj.inFlightFences[frameIndex]);
    
    // acquire an image from the swap chain after the previous frame has finished
    auto [result, imageIndex] = swapChain.acquireNextImage(UINT64_MAX, *synchobj.presentCompleteSemaphores[frameIndex], nullptr);
    commandpool.recordCommandBuffer(frameIndex, imageIndex);

    // submitting the command buffer
    vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    const vk::SubmitInfo submitInfo{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*synchobj.presentCompleteSemaphores[frameIndex],
        .pWaitDstStageMask = &waitDestinationStageMask,
        .commandBufferCount = 1,
        .pCommandBuffers = &*commandBuffers[frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &*synchobj.renderFinishedSemaphores[imageIndex]
    };
    queue.submit(submitInfo, *synchobj.inFlightFences[frameIndex]);

    const vk::PresentInfoKHR presentInfoKHR{
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &*synchobj.renderFinishedSemaphores[imageIndex],
        .swapchainCount = 1,
        .pSwapchains = &*swapChain,
        .pImageIndices = &imageIndex
    };
    result = queue.presentKHR(presentInfoKHR);
    switch (result)
	{
		case vk::Result::eSuccess:
			break;
		case vk::Result::eSuboptimalKHR:
			std::cout << "vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n";
			break;
		default:
			break;        // an unexpected result is returned!
	}

    frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Sirius::recreateSwapChain(const vk::raii::PhysicalDevice& physDevice, const vk::raii::Device& dev, const vk::raii::SurfaceKHR& surf, vk::SurfaceFormatKHR swapChainSurfaceFormat, std::vector<vk::Image> swapChainImages, GLFWwindow* win)
{
    dev.waitIdle();

    cleanupSwapChain();

    swapchain.createSwapChain(physDevice, dev, surf, win);
    imageview.createImageViews(swapChainSurfaceFormat, swapChainImages, dev);
}

void Sirius::cleanupSwapChain()
{
    
}

void Sirius::cleanup()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}