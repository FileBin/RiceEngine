/*
 * api_ShadowMap.hpp
 *
 * Vulkan resources of a ShadowMap: the D32_SFLOAT depth image, its view, the
 * non-comparison sampler, and the framebuffer of the depth-only shadow pass.
 */

#pragma once

#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"
#include <Rice/GL/ShadowMap.hpp>

NSP_GL_BEGIN

struct ShadowMap_API_Data {
    GraphicsManager_API_data &api_data;

    uint size = 0;
    uint caster_count = 0;

    vk::Image image;
    vk::DeviceMemory memory;
    vk::ImageView view;
    vk::Sampler sampler;
    vk::Framebuffer framebuffer;

    bool created = false;

    ShadowMap_API_Data(GraphicsManager_API_data &data, uint size);
    ~ShadowMap_API_Data();

    void create();
    void cleanup();

    vk::ImageView getView() const { return view; }
    vk::Sampler getSampler() const { return sampler; }
    vk::Framebuffer getFramebuffer() const { return framebuffer; }
};

NSP_GL_END
