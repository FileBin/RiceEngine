/*
 * VulkanAPI_data.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VulkanHelper.hpp"
#include <Rice/GL/GraphicsManager.hpp>
#include <cstddef>
#include <sys/types.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

NSP_GL_BEGIN

struct MemoryManager;

struct GraphicsManager_API_data {
  public:
    MemoryManager *memoryManager;

    vk::Instance instance;                      // Vulkan library handle
    vk::DebugUtilsMessengerEXT debug_messenger; // Vulkan debug output handle
    vk::PhysicalDevice GPU;                     // GPU chosen as the default device
    vk::Device device;                          // Vulkan device for commands
    vk::SurfaceKHR surface;                     // Vulkan window surface

    vk::SwapchainKHR swapchain = nullptr;
    vk::Format swapchainImageFormat =
        vk::Format::eUndefined; // image format expected by the windowing system
    vk::Format depthFormat = vk::Format::eUndefined; // depth format expected by
                                                     // the windowing system
    vec<vk::Image> swapchainImages = {};             // array of images from the swapchain
    vec<vk::ImageView> swapchainImageViews = {};     // array of image-views from the swapchain
    // One binary semaphore per swapchain image: a semaphore signalled for
    // presentation may still be in use by the presentation engine, so it cannot
    // be reused for another image (VUID-vkQueueSubmit-pSignalSemaphores-00067).
    vec<vk::Semaphore> imageSemaphores = {};
    vec<vk::Framebuffer> framebuffers = {};

    // TODO make this three as one texture
    vk::Image depthImage;
    vk::DeviceMemory depthImageMemory;
    vk::ImageView depthImageView;

    vk::Queue graphicsQueue;      // queue we will submit to
    uint graphicsQueueFamily = 0; // family of that queue

    vk::CommandPool commandPool;       // the command pool for our commands
    vk::DescriptorPool descriptorPool; // the descriptor pool for our descriptors

    vk::RenderPass def_renderPass; // the default render pass

    vk::Semaphore presentSemaphore; // signalled by vkAcquireNextImageKHR
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

    vk::Framebuffer getCurrentFrambuffer() { return framebuffers[swapchainImageIndex]; }

    // Records the transition into ColorAttachmentOptimal before a render pass.
    // The source layout is always declared Undefined: that is legal whatever the
    // image really is, discards the previous contents (the scene clears the
    // target anyway) and keeps an already recorded command buffer valid when it
    // is replayed on later frames.
    void prepareSwapchainImage(vk::CommandBuffer cmd, uint imageIndex);

    // Acquires the next swapchain image and stores its index. Returns false when
    // the swapchain is out of date (resizing is set) so nothing is submitted.
    bool acquireNextImage();

    friend class GraphicsManager;

    GraphicsManager_API_data(ptr<GraphicsManager> g_mgr);
    void sync();

    void createImage(uint w, uint h, vk::Format format, vk::ImageTiling tiling,
                     vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties,
                     vk::ImageLayout layout, vk::Image &image, vk::DeviceMemory &imageMemory);

    vk::ImageView createImageView(vk::Image image, vk::Format format,
                                  vk::ImageAspectFlags aspectFlags);

    uint findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties);

    vk::Format findSupportedFormat(const vec<vk::Format> &candidates, vk::ImageTiling tiling,
                                   vk::FormatFeatureFlags features);

    void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                      vk::MemoryPropertyFlags properties, vk::Buffer &buffer,
                      vk::DeviceMemory &bufferMemory);

    void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::BufferCopy copyRegion);

    void copyDataToBuffer(void *pData, size_t nData, size_t offset, vk::Buffer dstBuffer);

  private:
    void recreateSwapchain();
    void init_swapchain();
    void init_commands();
    void init_descriptor_pool();
    void init_def_renderpass();
    void init_framebuffers();
    void init_sync_structures();
    void init_depth_image();

    void cleanupSwapchain(bool destroy = true);

    void drawFrame(vec<vk::CommandBuffer> cmd);
    void cleanup();
};

NSP_GL_END
