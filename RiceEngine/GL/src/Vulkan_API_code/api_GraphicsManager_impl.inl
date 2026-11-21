/*
 * VulkanAPI_data_impl.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VkBootstrap.h"
#include "VulkanException.hpp"
#include "api_GraphicsManager.hpp"

#include <Rice/Engine/Log.hpp>
#include <Rice/GL/GraphicsManager.hpp>

#include <Rice/Math/Math.hpp>

#include <Rice/Engine/Window.hpp>
#include <cstddef>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "SDL2/SDL_vulkan.h"

#include "Rice/Engine/Log.hpp"

#include "MemoryManager.inl"

NSP_GL_BEGIN

short deviceTypeOrder(VkPhysicalDeviceType type) {
    switch (type) {
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return 0;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return 1;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
    case VK_PHYSICAL_DEVICE_TYPE_MAX_ENUM:
        return 0x7fff;
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    String message = pCallbackData->pMessage;

    switch (messageType) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        Log::log(Log::Info, "[Vulkan] {}", message);
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        Log::log(Log::Error, "[Vulkan] {}", message);
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        Log::log(Log::Warning, "[Vulkan] {}", message);
        break;
    default:
        Log::log(Log::Info, "[Vulkan] {}", message);
        break;
    }

    return VK_FALSE;
}

GraphicsManager_API_data::GraphicsManager_API_data(ptr<GraphicsManager> mgr) {
    vkb::InstanceBuilder builder;

    g_mgr = mgr;

    // make the Vulkan instance
    auto inst_ret = builder.set_app_name("Riced Field")
                        .require_api_version(1, 1, 0)
#ifdef GL_DEBUG_MODE
                        .request_validation_layers(true)
                        .use_default_debug_messenger()
                        .set_debug_callback(debugCallback)
#endif
                        .build();

    if (!inst_ret.has_value())
        THROW_VK_EX(inst_ret.vk_result());

    vkb::Instance vkb_inst = inst_ret.value();

    // store the instance
    instance = vkb_inst.instance;

    // store the debug messenger
#ifdef _DEBUG
    debug_messenger = vkb_inst.debug_messenger;
#endif

    if (!SDL_Vulkan_CreateSurface(
            mgr->window->getHandle().get(), instance,
            (VkSurfaceKHR *)&surface)) { // @suppress("Invalid arguments") //
                                         // @suppress("Method cannot be resolved")
        THROW_EXCEPTION("Failed to create vulkan surface! (Maybe window is not created)");
    }

    if (!surface)
        THROW_NULL_PTR_EXCEPTION(nullptr);

    // use vkbootstrap to select a GPU.
    // We want a GPU that can write to the SDL surface and supports Vulkan 1.1
    vkb::PhysicalDeviceSelector selector{vkb_inst};

    auto dev_result = selector.set_minimum_version(1, 1).set_surface(surface).select_devices();

    if (!dev_result.has_value()) {
        String str =
            fmt::format("Error: {}!",
                        dev_result.error().message()); // @suppress("Invalid arguments")
        THROW_EXCEPTION(str.c_str());
    }

    vec<vkb::PhysicalDevice> physicalDevices = dev_result.value();

    std::sort(physicalDevices.begin(), physicalDevices.end(),
              [](vkb::PhysicalDevice a, vkb::PhysicalDevice b) {
                  return deviceTypeOrder(a.properties.deviceType) <
                         deviceTypeOrder(b.properties.deviceType);
              });

    vkb::PhysicalDevice physicalDevice = physicalDevices.at(0);

    // create the final Vulkan device
    vkb::DeviceBuilder deviceBuilder{physicalDevice};

    vkb::Device vkbDevice = deviceBuilder.build().value();

    // Get the VkDevice handle used in the rest of a Vulkan application
    device = vkbDevice.device;

    GPU = physicalDevice.physical_device;
    Log::log(Log::Info, "GPU INFO: {}", String(GPU.getProperties().deviceName.data()));

    // use vkbootstrap to get a Graphics queue
    graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    init_swapchain();
    init_commands();
    init_descriptor_pool();
    init_depth_image();
    init_def_renderpass();
    init_shadow_renderpass();
    init_framebuffers();
    init_sync_structures();

    memoryManager = new MemoryManager(*this);
}

void GraphicsManager_API_data::init_swapchain() {

    auto graphicsManager = g_mgr.lock();
    if (!graphicsManager)
        THROW_NULL_PTR_EXCEPTION(graphicsManager.get());

    Log::debug("Swapchain building...");
    vkb::SwapchainBuilder swapchainBuilder{GPU, device, surface};

    if (use_hdr) {
        swapchainBuilder.set_desired_format((VkSurfaceFormatKHR)vk::SurfaceFormatKHR(
            vk::Format::eR16G16B16A16Sfloat, vk::ColorSpaceKHR::eHdr10HlgEXT));
    } else {
        swapchainBuilder.use_default_format_selection();
    }

    // The swapchain lives in pixels, so the drawable size is what matters - not
    // the logical window size reported by SDL_GetWindowSize (they differ on
    // scaled displays, and that is what made the image smaller than the window).
    int w = 0, h = 0;
    SDL_Vulkan_GetDrawableSize(graphicsManager->window->getHandle().get(), &w, &h);
    if (w <= 0 || h <= 0) {
        w = graphicsManager->window->getWidth();
        h = graphicsManager->window->getHeight();
    }

    // A zero-sized or tiny drawable (minimized window) must not produce a
    // swapchain; keep the old one and wait for the next resize.
    if (w < 64 || h < 64) {
        Log::debug("Swapchain rebuild skipped, drawable is {}x{}", w, h);
        return;
    }

    for (size_t i = 0; i < swapchainImageViews.size(); i++)
        device.destroy(swapchainImageViews[i]);
    swapchainImageViews.clear();

    auto old_swapchain = swapchain;

    vkb::Swapchain vkbSwapchain =
        swapchainBuilder
            .set_desired_present_mode(
                (VkPresentModeKHR)vk::PresentModeKHR::eImmediate) // use v-sync present
                                                                  // mode
            .set_desired_extent((uint32_t)w, (uint32_t)h)
            .set_old_swapchain((VkSwapchainKHR)old_swapchain)
            .build()
            .value();

    if (old_swapchain)
        device.destroy(old_swapchain);

    // store swapchain and its related images
    swapchain = vkbSwapchain.swapchain;
    // vk-bootstrap hands out raw Vulkan handles; vulkan-hpp wraps them, so
    // copy instead of casting the vector type.
    const auto &vkbImages = vkbSwapchain.get_images().value();
    swapchainImages.assign(vkbImages.begin(), vkbImages.end());
    const auto &vkbImageViews = vkbSwapchain.get_image_views().value();
    swapchainImageViews.assign(vkbImageViews.begin(), vkbImageViews.end());

    swapchainImageFormat = static_cast<vk::Format>(vkbSwapchain.image_format);

    // Everything that follows (depth image, framebuffers, pipelines, render
    // areas) must use the size the driver actually gave us.
    windowExcent = vkbSwapchain.extent;

    // One presentation semaphore per image, recreated together with the images.
    for (auto &sem : imageSemaphores)
        if (sem)
            device.destroy(sem);
    imageSemaphores.clear();
    vk::SemaphoreCreateInfo semInfo{};
    for (size_t i = 0; i < swapchainImages.size(); i++)
        imageSemaphores.push_back(device.createSemaphore(semInfo));

    Log::debug("Swapchain built! {}x{}, {} images (window {}x{}, drawable {}x{})",
               windowExcent.width, windowExcent.height, swapchainImages.size(),
               graphicsManager->window->getWidth(), graphicsManager->window->getHeight(), w, h);
}

void GraphicsManager_API_data::prepareSwapchainImage(vk::CommandBuffer cmd, uint imageIndex) {
    if (imageIndex >= swapchainImages.size())
        return;

    using namespace vk;

    ImageMemoryBarrier barrier{};
    barrier.oldLayout = ImageLayout::eUndefined; // discard, whatever it was
    barrier.newLayout = ImageLayout::eColorAttachmentOptimal;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = swapchainImages[imageIndex];
    barrier.subresourceRange = ImageSubresourceRange(ImageAspectFlagBits::eColor, 0, 1, 0, 1);
    barrier.srcAccessMask = AccessFlags{};
    barrier.dstAccessMask = AccessFlagBits::eColorAttachmentWrite;

    cmd.pipelineBarrier(PipelineStageFlagBits::eTopOfPipe,
                        PipelineStageFlagBits::eColorAttachmentOutput, DependencyFlags{},
                        nullptr, nullptr, barrier);
}

void GraphicsManager_API_data::init_commands() {
    // create a command pool for commands submitted to the graphics queue.
    // we also want the pool to allow for resetting of individual command
    // buffers
    auto commandPoolInfo = vk::CommandPoolCreateInfo()
                               .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
                               .setQueueFamilyIndex(graphicsQueueFamily);

    commandPool = device.createCommandPool(commandPoolInfo);

    // allocate the default command buffer that we will use for rendering
}

void GraphicsManager_API_data::init_descriptor_pool() {
    auto n = swapchainImages.size();
    // Every uniform buffer allocates one set per swapchain image and draws
    // allocate their own sets too, so the pool has to be much larger than the
    // swapchain itself - sizing it as "n descriptors" ran it dry immediately.
    // Two descriptor types: uniform buffers (ModelData / material / light) and
    // combined image samplers (the shadow map).
    std::array<vk::DescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = vk::DescriptorType::eUniformBuffer;
    poolSizes[0].descriptorCount = 0x10000;
    poolSizes[1].type = vk::DescriptorType::eCombinedImageSampler;
    poolSizes[1].descriptorCount = 0x1000;

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 0x10000;
    poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    descriptorPool = device.createDescriptorPool(poolInfo);
    (void)n;
}

vk::Format GraphicsManager_API_data::findSupportedFormat(const vec<vk::Format> &candidates,
                                                         vk::ImageTiling tiling,
                                                         vk::FormatFeatureFlags features) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = GPU.getFormatProperties(format);

        if (tiling == vk::ImageTiling::eLinear &&
            (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal &&
                   (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    THROW_EXCEPTION("Failed to find supported format!");
}

uint GraphicsManager_API_data::findMemoryType(uint typeFilter,
                                              vk::MemoryPropertyFlags properties) {
    using namespace vk;
    PhysicalDeviceMemoryProperties memprop = GPU.getMemoryProperties();

    for (uint i = 0; i < memprop.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) &&
            (memprop.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0xffffffff;
}

void GraphicsManager_API_data::createImage(uint w, uint h, vk::Format format,
                                           vk::ImageTiling tiling, vk::ImageUsageFlags usage,
                                           vk::MemoryPropertyFlags properties,
                                           vk::ImageLayout layout, vk::Image &image,
                                           vk::DeviceMemory &imageMemory) {
    using namespace vk;
    ImageCreateInfo imageInfo{};
    imageInfo.imageType = ImageType::e2D;
    imageInfo.extent.width = w;
    imageInfo.extent.height = h;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = layout;
    imageInfo.usage = usage;
    imageInfo.samples = SampleCountFlagBits::e1;
    imageInfo.sharingMode = SharingMode::eExclusive;

    auto res = device.createImage(&imageInfo, nullptr, &image);
    THROW_VK_EX_IF_BAD(res);

    MemoryRequirements memRequirements = device.getImageMemoryRequirements(image);

    MemoryAllocateInfo allocInfo{};
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    res = device.allocateMemory(&allocInfo, nullptr, &imageMemory);
    THROW_VK_EX_IF_BAD(res);

    device.bindImageMemory(image, imageMemory, 0);
}

vk::ImageView GraphicsManager_API_data::createImageView(vk::Image image, vk::Format format,
                                                        vk::ImageAspectFlags aspectFlags) {
    using namespace vk;
    ImageViewCreateInfo viewInfo{};
    viewInfo.image = image;
    viewInfo.viewType = ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    ImageView imageView;
    auto res = device.createImageView(&viewInfo, nullptr, &imageView);
    THROW_VK_EX_IF_BAD(res);

    return imageView;
}
void GraphicsManager_API_data::init_depth_image() {
    using namespace vk;
    // create a depth image
    depthFormat = findSupportedFormat(
        {Format::eD32SfloatS8Uint, Format::eD32Sfloat, Format::eD24UnormS8Uint},
        ImageTiling::eOptimal, FormatFeatureFlagBits::eDepthStencilAttachment);

    ImageAspectFlags aspectFlags = ImageAspectFlagBits::eDepth;

    if (depthFormat == vk::Format::eD16UnormS8Uint ||
        depthFormat == vk::Format::eD24UnormS8Uint ||
        depthFormat == vk::Format::eD32SfloatS8Uint) {
        aspectFlags |= vk::ImageAspectFlagBits::eStencil;
    }

    createImage(windowExcent.width, windowExcent.height, depthFormat, ImageTiling::eOptimal,
                ImageUsageFlagBits::eDepthStencilAttachment,
                MemoryPropertyFlagBits::eDeviceLocal, ImageLayout::eUndefined, depthImage,
                depthImageMemory);
    depthImageView = createImageView(depthImage, depthFormat, aspectFlags);
}

void GraphicsManager_API_data::init_def_renderpass() {
    vk::AttachmentDescription color_attachment;
    // the attachment will have the format needed by the swapchain
    color_attachment.format = swapchainImageFormat;
    // 1 sample, we won't be doing MSAA
    color_attachment.samples = vk::SampleCountFlagBits::e1;
    // Contents are discarded: the initial layout is Undefined (a LOAD would be
    // illegal there, VUID-VkAttachmentDescription-format-06410) and the scene
    // clears the target explicitly with ClearRenderTarget.
    color_attachment.loadOp = vk::AttachmentLoadOp::eDontCare;
    // we keep the attachment stored when the renderpass ends
    color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    // we don't care about stencil
    color_attachment.stencilLoadOp =
        vk::AttachmentLoadOp::eDontCare; // vk::AttachmentLoadOp::eClear;
    color_attachment.stencilStoreOp =
        vk::AttachmentStoreOp::eDontCare; // vk::AttachmentStoreOp::eStore;

    // A fresh swapchain image is Undefined; the render pass declares that as its
    // initial layout and hands the image to the presentation engine itself
    // (finalLayout = PresentSrc), so the recorded command buffer stays valid on
    // every replay instead of tracking layouts on the CPU.
    color_attachment.initialLayout = vk::ImageLayout::eUndefined;
    color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

    vk::AttachmentDescription depth_attachment{};
    depth_attachment.format = depthFormat;
    depth_attachment.samples = vk::SampleCountFlagBits::e1;
    depth_attachment.loadOp = vk::AttachmentLoadOp::eDontCare;
    depth_attachment.storeOp = vk::AttachmentStoreOp::eStore;
    // the depth format has a stencil aspect, so stencil ops must be valid too
    depth_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    // freshly created depth images are Undefined; contents are discarded anyway
    // (the scene clears depth every frame)
    depth_attachment.initialLayout = vk::ImageLayout::eUndefined;
    depth_attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference color_attachment_ref = {};
    // attachment number will index into the pAttachments array in the parent
    // renderpass itself
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    // we are going to create 1 subpass, which is the minimum you can do
    vk::SubpassDescription subpass = {};
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    vk::SubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = dependency.srcStageMask =
        vk::PipelineStageFlagBits::eAllGraphics;
    dependency.dstAccessMask = dependency.srcAccessMask =
        vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite |
        vk::AccessFlagBits::eDepthStencilAttachmentRead |
        vk::AccessFlagBits::eDepthStencilAttachmentWrite;

    std::array<vk::AttachmentDescription, 2> attachments = {color_attachment,
                                                            depth_attachment};

    vk::RenderPassCreateInfo render_pass_info = {};
    // connect the color attachment to the info
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    // connect the subpass to the info
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    // connect the dependency to the info
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    auto res = device.createRenderPass(&render_pass_info, nullptr, &def_renderPass);
    THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager_API_data::init_shadow_renderpass() {
    using namespace vk;

    // One depth attachment, no colour attachment: this pass exists only to fill
    // the shadow map. Contents are cleared to 1.0 (far) at the start of every
    // pass and handed to the scene shaders as a read-only depth texture.
    AttachmentDescription depth{};
    depth.format = Format::eD32Sfloat;
    depth.samples = SampleCountFlagBits::e1;
    depth.loadOp = AttachmentLoadOp::eClear;
    depth.storeOp = AttachmentStoreOp::eStore;
    depth.stencilLoadOp = AttachmentLoadOp::eDontCare;
    depth.stencilStoreOp = AttachmentStoreOp::eDontCare;
    // Undefined discards the previous frame's contents (the pass clears anyway)
    // and keeps an already recorded command buffer valid on every replay, the
    // same trick the swapchain attachment uses.
    depth.initialLayout = ImageLayout::eUndefined;
    depth.finalLayout = ImageLayout::eShaderReadOnlyOptimal;

    AttachmentReference depth_ref{};
    depth_ref.attachment = 0;
    depth_ref.layout = ImageLayout::eDepthStencilAttachmentOptimal;

    SubpassDescription subpass{};
    subpass.pipelineBindPoint = PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 0;
    subpass.pDepthStencilAttachment = &depth_ref;

    // The map is written at the end of this pass and read by the fragment
    // shaders of the main pass, in the same command buffer: the second
    // dependency makes the depth writes available and visible to them.
    std::array<SubpassDependency, 2> deps{};
    deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    deps[0].dstSubpass = 0;
    deps[0].srcStageMask = PipelineStageFlagBits::eFragmentShader |
                           PipelineStageFlagBits::eLateFragmentTests;
    deps[0].srcAccessMask = AccessFlagBits::eShaderRead;
    deps[0].dstStageMask = PipelineStageFlagBits::eEarlyFragmentTests;
    deps[0].dstAccessMask = AccessFlagBits::eDepthStencilAttachmentWrite;

    deps[1].srcSubpass = 0;
    deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    deps[1].srcStageMask = PipelineStageFlagBits::eLateFragmentTests;
    deps[1].srcAccessMask = AccessFlagBits::eDepthStencilAttachmentWrite;
    deps[1].dstStageMask = PipelineStageFlagBits::eFragmentShader;
    deps[1].dstAccessMask = AccessFlagBits::eShaderRead;

    RenderPassCreateInfo info{};
    info.attachmentCount = 1;
    info.pAttachments = &depth;
    info.subpassCount = 1;
    info.pSubpasses = &subpass;
    info.dependencyCount = deps.size();
    info.pDependencies = deps.data();

    auto res = device.createRenderPass(&info, nullptr, &shadow_renderPass);
    THROW_VK_EX_IF_BAD(res);
}

void GraphicsManager_API_data::init_framebuffers() {
    // create the framebuffers for the swapchain images. This will connect the
    // render-pass to the images for rendering
    vk::FramebufferCreateInfo fb_info = {};

    fb_info.renderPass = def_renderPass;
    fb_info.width = windowExcent.width;
    fb_info.height = windowExcent.height;
    fb_info.layers = 1;

    // grab how many images we have in the swapchain
    const uint swapchain_imagecount = swapchainImages.size();
    framebuffers = vec<vk::Framebuffer>(swapchain_imagecount);

    // create framebuffers for each of the swapchain image views
    for (int i = 0; i < swapchain_imagecount; i++) {
        std::array<vk::ImageView, 2> attachments = {swapchainImageViews[i], depthImageView};
        fb_info.attachmentCount = attachments.size();
        fb_info.pAttachments = attachments.data();
        framebuffers[i] = device.createFramebuffer(fb_info);
    }
}

void GraphicsManager_API_data::init_sync_structures() {
    // create synchronization structures
    vk::FenceCreateInfo fenceCreateInfo = {};

    // we want to create the fence with the Create Signaled flag, so we can wait
    // on it before using it on a GPU command (for the first frame)
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    renderFence = device.createFence(fenceCreateInfo);

    // for the semaphores we don't need any flags
    vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

    presentSemaphore = device.createSemaphore(semaphoreCreateInfo);
    // per-image semaphores are created together with the swapchain images
}

void GraphicsManager_API_data::recreateSwapchain() {
    auto graphicsManager = g_mgr.lock();
    if (!graphicsManager)
        THROW_NULL_PTR_EXCEPTION(graphicsManager.get());

    if (graphicsManager->window->isMinimized()) {
        Log::debug("Window is minimized!");
        return;
    }
    Log::debug("Swapchain recreating...");
    device.waitIdle();
    cleanupSwapchain(false);

    init_swapchain();
    init_depth_image();

    Vector2i window_size;

    SDL_Vulkan_GetDrawableSize(graphicsManager->window->getHandle().get(),
                               (int *)&window_size.x, (int *)&window_size.y);

    graphicsManager->resizePipelines->invoke(window_size);
    init_framebuffers();
    graphicsManager->resizeGraphicsComponents->invoke(window_size);
    graphicsManager->resizeSubCommandBuffers->invoke(window_size);
    graphicsManager->resizeCommandBuffers->invoke(window_size);
}

inline void GraphicsManager_API_data::cleanupSwapchain(bool destroy) {

    uint n = framebuffers.size();
    for (size_t i = 0; i < n; i++) {
        device.destroy(framebuffers[i]);
    }

    device.destroy(depthImageView);
    device.destroy(depthImage);
    device.free(depthImageMemory);

    if (destroy) {
        n = swapchainImages.size();
        for (size_t i = 0; i < n; i++) {
            device.destroy(swapchainImageViews[i]);
        }
        swapchainImageViews.clear();

        device.destroy(swapchain);
    }
}

void GraphicsManager_API_data::sync() {
    auto graphicsManager = g_mgr.lock();
    if (!graphicsManager)
        THROW_NULL_PTR_EXCEPTION(graphicsManager.get());
    // wait until the GPU has finished rendering the last frame. Timeout of 0.5
    // second (fast)
    auto res = device.waitForFences({renderFence}, true, 0.5 VK_SEC);
    if (res == vk::Result::eTimeout && !graphicsManager->window->isMinimized()) {
        // wait until the GPU has finished rendering the last frame. Timeout of
        // 1 second (slow)
        res = device.waitForFences({renderFence}, true, 1 VK_SEC);
    }
    THROW_VK_EX_IF_BAD(res);
}

bool GraphicsManager_API_data::acquireNextImage() {
    if (resizing)
        return false;

    // wait until the GPU has finished the previous frame, so the acquire
    // semaphore can be reused
    sync();

    // request image from the swapchain, 1 second timeout
    auto res = device.acquireNextImageKHR(swapchain, 1000000000, presentSemaphore, nullptr,
                                          &swapchainImageIndex);
    if (res == vk::Result::eErrorOutOfDateKHR) {
        resizing = true;
        return false;
    }
    if (res != vk::Result::eSuccess && res != vk::Result::eSuboptimalKHR) {
        THROW_VK_EX(res);
    }
    return true;
}

void GraphicsManager_API_data::drawFrame(vec<vk::CommandBuffer> cmd) {
    if (resizing)
        return;

    device.resetFences({renderFence});

    memoryManager->update();

    // prepare the submission to the queue.
    // we want to wait on the _presentSemaphore, as that semaphore is signaled
    // when the swapchain is ready we will signal the _renderSemaphore, to
    // signal that rendering has finished

    vk::SubmitInfo submit = {};
    submit.sType = vk::StructureType::eSubmitInfo;
    submit.pNext = nullptr;

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    submit.pWaitDstStageMask = &waitStage;

    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &presentSemaphore;

    // Signal the semaphore that belongs to this swapchain image: the previous
    // frame may still be presenting another image with the shared one.
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &imageSemaphores[swapchainImageIndex];

    submit.commandBufferCount = cmd.size();
    submit.pCommandBuffers = cmd.data();

    // submit command buffer to the queue and execute it.
    //  _renderFence will now block until the graphic commands finish execution
    auto result = graphicsQueue.submit(1, &submit, renderFence);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        resizing = true;
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        THROW_VK_EX(result);
    }
    // this will put the image we just rendered into the visible window.
    // we want to wait on the _renderSemaphore for that,
    // as it's necessary that drawing commands have finished before the image is
    // displayed to the user
    vk::PresentInfoKHR presentInfo = {};
    presentInfo.sType = vk::StructureType::ePresentInfoKHR;
    presentInfo.pNext = nullptr;

    presentInfo.pSwapchains = &swapchain;
    presentInfo.swapchainCount = 1;

    presentInfo.pWaitSemaphores = &imageSemaphores[swapchainImageIndex];
    presentInfo.waitSemaphoreCount = 1;

    presentInfo.pImageIndices = &swapchainImageIndex;
    result = (vk::Result)vkQueuePresentKHR((VkQueue)graphicsQueue,
                                           (const VkPresentInfoKHR *)&presentInfo);

    if (result == vk::Result::eErrorOutOfDateKHR) {
        resizing = true;
        return;
    } else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR) {
        THROW_VK_EX(result);
    }

    if (result == vk::Result::eSuboptimalKHR)
        resizing = true;
}

GraphicsManager_API_data::~GraphicsManager_API_data() { cleanup(); }

void GraphicsManager_API_data::cleanup() {
    if (!instance)
        return;

    // nothing may be in flight before any of the objects below are destroyed
    device.waitIdle();

    if (memoryManager) {
        delete memoryManager;
        memoryManager = nullptr;
    }
    auto graphicsManager = g_mgr.lock();

    if (graphicsManager) {
        graphicsManager->destroyCommandBuffers->invoke();
        graphicsManager->destroyEvent->invoke();
    }
    device.destroyDescriptorPool(descriptorPool);
    cleanupSwapchain();

    device.destroy(def_renderPass);
    device.destroy(shadow_renderPass);

    device.destroy(presentSemaphore);
    for (auto &sem : imageSemaphores)
        if (sem)
            device.destroy(sem);
    imageSemaphores.clear();
    device.destroy(renderFence);

    device.destroy(commandPool);

    vkDestroyDevice(device, nullptr);
    device = nullptr;

    vkDestroySurfaceKHR(instance, surface, nullptr);
    surface = nullptr;

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    debug_messenger = nullptr;

    instance.destroy();
    instance = nullptr;
}

vk::MemoryPropertyFlags GraphicsManager_API_data::memoryTypeProperties(uint index) {
    auto memprop = GPU.getMemoryProperties();
    if (index >= memprop.memoryTypeCount)
        return {};
    return memprop.memoryTypes[index].propertyFlags;
}

void GraphicsManager_API_data::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage,
                                            vk::MemoryPropertyFlags properties,
                                            vk::Buffer &buffer, vk::DeviceMemory &bufferMemory,
                                            uint *memoryTypeIndex) {
    using namespace vk;
    BufferCreateInfo bufferInfo{};
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = SharingMode::eExclusive;

    buffer = device.createBuffer(bufferInfo);

    auto memReq = device.getBufferMemoryRequirements(buffer);

    MemoryAllocateInfo allocInfo;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, properties);
    if (memoryTypeIndex != nullptr)
        *memoryTypeIndex = allocInfo.memoryTypeIndex;

    bufferMemory = device.allocateMemory(allocInfo);
    device.bindBufferMemory(buffer, bufferMemory, 0);
}

void GraphicsManager_API_data::copyDataToBuffer(void *pData, size_t nData, size_t offset,
                                                vk::Buffer dstBuffer) {
    /*void *mappedData;
    mappedData = device.mapMemory(mem, offset, nData);
    memcpy(mappedData, pData, (size_t)nData);
    device.unmapMemory(mem);*/
    // FIXME: not working
    using namespace vk;
    using vk::Buffer;
    Buffer stagingBuffer;
    DeviceMemory stagingBufferMemory;

    createBuffer(nData, BufferUsageFlagBits::eTransferSrc,
                 MemoryPropertyFlagBits::eHostVisible | MemoryPropertyFlagBits::eHostCoherent,
                 stagingBuffer, stagingBufferMemory);

    void *mappedData = device.mapMemory(stagingBufferMemory, 0, nData);
    memcpy(mappedData, pData, (size_t)nData);
    device.unmapMemory(stagingBufferMemory);

    copyBuffer(stagingBuffer, dstBuffer, BufferCopy().setDstOffset(offset).setSize(nData));

    device.destroy(stagingBuffer);
    device.free(stagingBufferMemory);
}

void GraphicsManager_API_data::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer,
                                          vk::BufferCopy copyRegion) {
    using namespace vk;
    CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = CommandBufferLevel::ePrimary;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    auto commandBuffer = device.allocateCommandBuffers(allocInfo)[0];

    CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    commandBuffer.copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);

    commandBuffer.end();

    SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    auto res = graphicsQueue.submit(1, &submitInfo, nullptr);
    THROW_VK_EX_IF_BAD(res);
    graphicsQueue.waitIdle();

    device.freeCommandBuffers(commandPool, 1, &commandBuffer);
}

NSP_GL_END
