/*
 * api_ShadowMap_impl.inl
 */

#pragma once

#include "api_GraphicsManager.hpp"
#include "api_ShadowMap.hpp"

#include <Rice/Engine/Log.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

ShadowMap_API_Data::ShadowMap_API_Data(GraphicsManager_API_data &data, uint size)
    : api_data(data), size(size) {}

ShadowMap_API_Data::~ShadowMap_API_Data() { cleanup(); }

void ShadowMap_API_Data::create() {
    using namespace vk;

    if (created)
        return;

    // Depth attachment AND sampled texture, plus transfer source for
    // ShadowMap::readFilledTexelCount() (the smoke test reads the map back to
    // prove it contains geometry). D32_SFLOAT (no stencil) is the format the
    // shadow render pass declares.
    api_data.createImage(size, size, Format::eD32Sfloat, ImageTiling::eOptimal,
                         ImageUsageFlagBits::eDepthStencilAttachment |
                             ImageUsageFlagBits::eSampled |
                             ImageUsageFlagBits::eTransferSrc,
                         MemoryPropertyFlagBits::eDeviceLocal, ImageLayout::eUndefined, image,
                         memory);

    view = api_data.createImageView(image, Format::eD32Sfloat, ImageAspectFlagBits::eDepth);

    // Compare is OFF on purpose: the shader samples raw depth values with
    // texelFetch and does the comparison itself (that is what the original
    // DirectX shader did, and it is what makes the 2x2 interpolated PCF and the
    // "bias - lightSpaceZ" term possible at all).
    SamplerCreateInfo samplerInfo{};
    samplerInfo.magFilter = Filter::eNearest;
    samplerInfo.minFilter = Filter::eNearest;
    samplerInfo.mipmapMode = SamplerMipmapMode::eNearest;
    samplerInfo.addressModeU = SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = SamplerAddressMode::eClampToEdge;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = CompareOp::eAlways;
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = 0.f;
    samplerInfo.borderColor = BorderColor::eFloatOpaqueWhite;
    sampler = api_data.device.createSampler(samplerInfo);

    FramebufferCreateInfo fbInfo{};
    fbInfo.renderPass = api_data.shadow_renderPass;
    fbInfo.attachmentCount = 1;
    fbInfo.pAttachments = &view;
    fbInfo.width = size;
    fbInfo.height = size;
    fbInfo.layers = 1;
    framebuffer = api_data.device.createFramebuffer(fbInfo);

    created = true;

    Log::log(Log::Info, "Shadow map created: {}x{} D32_SFLOAT, {} bytes", size, size,
             (unsigned long long)size * size * 4);
}

void ShadowMap_API_Data::cleanup() {
    if (!created)
        return;

    if (framebuffer)
        api_data.device.destroy(framebuffer);
    if (sampler)
        api_data.device.destroy(sampler);
    if (view)
        api_data.device.destroy(view);
    if (image)
        api_data.device.destroy(image);
    if (memory)
        api_data.device.free(memory);

    framebuffer = nullptr;
    sampler = nullptr;
    view = nullptr;
    image = nullptr;
    memory = nullptr;
    created = false;
}

NSP_GL_END
