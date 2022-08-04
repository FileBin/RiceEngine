/*
 * VulkanAPI_data.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VulkanHelper.hpp"
#include <Rice/GL/GraphicsManager.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

NSP_GL_BEGIN

struct GraphicsManager_API_data {
  public:
    vk::Instance instance;                      // Vulkan library handle
    vk::DebugUtilsMessengerEXT debug_messenger; // Vulkan debug output handle
    vk::PhysicalDevice GPU; // GPU chosen as the default device
    vk::Device device;      // Vulkan device for commands
    vk::SurfaceKHR surface; // Vulkan window surface

    vk::SwapchainKHR swapchain = nullptr;
    vk::Format swapchainImageFormat =
        vk::Format::eUndefined; // image format expected by the windowing system
    vk::Format depthFormat = vk::Format::eUndefined; // depth format expected by
                                                     // the windowing system
    vec<vk::Image> swapchainImages = {}; // array of images from the swapchain
    vec<vk::ImageView> swapchainImageViews =
        {}; // array of image-views from the swapchain
    vec<vk::Framebuffer> framebuffers = {};

    // TODO make this three as one texture
    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;

    vk::Queue graphicsQueue;      // queue we will submit to
    uint graphicsQueueFamily = 0; // family of that queue

    vk::CommandPool commandPool; // the command pool for our commands

    vk::RenderPass def_renderPass; // the default render pass

    vk::Semaphore presentSemaphore, renderSemaphore;
    vk::Fence renderFence;

    vk::Extent2D windowExcent;

    wptr<GraphicsManager> g_mgr;

    uint swapchainImageIndex = 0;

    bool resizing = false;

    static constexpr bool use_hdr =
#ifdef USE_HDR
        true;
#else
        false;
#endif

    ~GraphicsManager_API_data();

    vk::Framebuffer getCurrentFrambuffer() {
        return framebuffers[swapchainImageIndex];
    }
    friend class GraphicsManager;

    GraphicsManager_API_data(ptr<GraphicsManager> g_mgr);
    void sync();

    void createImage(uint w, uint h, vk::Format format, vk::ImageTiling tiling,
                     vk::ImageUsageFlags usage,
                     vk::MemoryPropertyFlags properties, vk::ImageLayout layout,
                     vk::Image &image, vk::DeviceMemory &imageMemory);

    vk::ImageView createImageView(vk::Image image, vk::Format format,
                                  vk::ImageAspectFlags aspectFlags);

    uint findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties);

    vk::Format findSupportedFormat(const vec<vk::Format> &candidates,
                                   vk::ImageTiling tiling,
                                   vk::FormatFeatureFlags features);

  private:
    void recreateSwapchain();
    void init_swapchain();
    void init_commands();
    void init_def_renderpass();
    void init_framebuffers();
    void init_sync_structures();
    void init_depth_image();

    void cleanupSwapchain(bool destroy = true);

    void executeCmd(vec<vk::CommandBuffer> cmd);
    void cleanup();
};

NSP_GL_END
