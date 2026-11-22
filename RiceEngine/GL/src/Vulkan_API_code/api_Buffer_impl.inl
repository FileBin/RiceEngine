/*
 * api_Buffer_impl.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "Rice/GL/GraphicsManager.hpp"
#include "api_Buffer.hpp"
#include "api_GraphicsManager.hpp"
#include <vulkan/vulkan_enums.hpp>

#include "MemoryManager.hpp"

NSP_GL_BEGIN

Buffer_API_Data::Buffer_API_Data(GraphicsManager_API_data &api_data) : api_data(api_data) {}

Buffer_API_Data &Buffer_API_Data::allocate(size_t size, BufferUsage usage) {
    using namespace vk;
    vk::BufferUsageFlags usageFlags;
    switch (usage) {
    case BufferUsage::Index:
        usageFlags = BufferUsageFlagBits::eIndexBuffer;
        break;
    case BufferUsage::Vertex:
        usageFlags = BufferUsageFlagBits::eVertexBuffer;
        break;
    case BufferUsage::Uniform:
        usageFlags = BufferUsageFlagBits::eUniformBuffer;
        break;
    case BufferUsage::Storage:
        usageFlags = BufferUsageFlagBits::eStorageBuffer;
        break;
    }

    return allocate(size, usageFlags, MemoryPropertyFlagBits::eHostVisible);
}

Buffer_API_Data &Buffer_API_Data::allocate(size_t size, vk::BufferUsageFlags usageFlags,
                                           vk::MemoryPropertyFlags properties) {
    using namespace vk;
    if (!allocated) {
        buffer_size = size;

        // Every buffer this engine allocates is written from the host at least
        // once (initial geometry, a uniform block, a compute input).
        usageFlags |= BufferUsageFlagBits::eTransferDst;

        uint memory_type = 0;
        api_data.createBuffer(size, usageFlags, properties, buffer, memory, &memory_type);
        host_coherent =
            !!(api_data.memoryTypeProperties(memory_type) & vk::MemoryPropertyFlagBits::eHostCoherent);
    }
    allocated = true;

    return *this;
}

Buffer_API_Data &Buffer_API_Data::setData(void *pData, size_t nData, size_t offset) {

    // api_data.copyDataToBuffer(pData, nData, offset, buffer);
    api_data.memoryManager->copyDataToBuffer(pData, nData, offset, buffer);
    return *this;
}

Buffer_API_Data &Buffer_API_Data::free() {
    if (allocated) {
        // A staging copy for this buffer may still be queued (uploads are
        // submitted at the next draw); recording it against a destroyed handle
        // is invalid, so forget those copies first. The manager is already gone
        // during shutdown cleanup, when nothing is queued anymore.
        if (api_data.memoryManager)
            api_data.memoryManager->forgetCopyDestination(buffer);
        api_data.device.destroy(buffer);
        api_data.device.freeMemory(memory);
    }
    allocated = false;
    return *this;
}

Buffer_API_Data::~Buffer_API_Data() { free(); }

NSP_GL_END
