#pragma once

#include "vk_settings.hpp"


// function to load the binary data from files
static std::vector<char> readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<char> buffer(file.tellg());

    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));

    file.close();
    return buffer;
}

class GraphicsPipeline
{
    public:
        void createGraphicsPipeline(const vk::raii::Device& device, vk::Extent2D swapChainExtent, vk::SurfaceFormatKHR swapChainSurfaceFormat);

        const vk::raii::Pipeline& getGraphicsPipeline() { return graphicsPipeline; }

    private:
        vk::raii::PipelineLayout pipelineLayout = nullptr;
        vk::raii::Pipeline graphicsPipeline = nullptr;
    
        [[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code, const vk::raii::Device& device) const;
};