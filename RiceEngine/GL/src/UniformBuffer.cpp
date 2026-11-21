/*
 * UniformBuffer.cpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "Rice/GL/Buffer.hpp"
#include "Rice/Util/Exceptions/Exception.hpp"
#include "pch.h"

#include <algorithm>
#include <cstring>

#include "Vulkan_API_code/api_Buffer.hpp"
#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_Shader.hpp"
#include <Rice/GL/UniformBuffer.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

UniformBuffer::UniformBuffer(ptr<GraphicsManager> g_mgr, uint n)
    : Buffer(g_mgr, BufferUsage::Uniform, n) {}

void UniformBuffer::updateData(const void *data, size_t size) {
    if (data == nullptr || size == 0 || !isAllocated())
        return;

    auto &g_api = get_api_data();
    const size_t n = std::min<size_t>(size, api_data->buffer_size);

    // Mapped once and kept: on a software driver every vkMapMemory waits for the
    // device, and this is called once per frame per animated material.
    if (mapped == nullptr)
        mapped = g_api.device.mapMemory(api_data->memory, 0, api_data->buffer_size);

    std::memcpy(mapped, data, n);

    // eHostVisible does not imply eHostCoherent: without the flush the write is
    // not guaranteed to be visible to the GPU. Coherent memory (what every
    // desktop driver hands out for uniform buffers) needs no flush at all, and
    // skipping it matters: a flush per frame per animated material is a device
    // synchronisation on some drivers, which showed up as a large per-frame cost
    // in the headless (lavapipe) runs.
    if (!api_data->host_coherent) {
        vk::MappedMemoryRange range;
        range.memory = api_data->memory;
        range.offset = 0;
        range.size = VK_WHOLE_SIZE;
        g_api.device.flushMappedMemoryRanges(1, &range);
    }
}

void UniformBuffer::unmap() {
    if (mapped != nullptr) {
        get_api_data().device.unmapMemory(api_data->memory);
        mapped = nullptr;
    }
}

void UniformBuffer::cleanup() {
    // Unmap before the memory is freed: a mapping is a property of the
    // VkDeviceMemory, so freeing it while mapped is invalid.
    if (isAllocated())
        unmap();
    if (isAllocated())
        api_data->free();
}

bool UniformBuffer::isAllocated() { return api_data->buffer_size != 0; }

void UniformBuffer::reset(uint size) {
    cleanup();

    if (size != 0) {
        api_data->allocate(size, BufferUsage::Uniform);
    }
}

NSP_GL_END
