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
    if (!allocated) {
        buffer_size = size;

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
        }
        usageFlags |= BufferUsageFlagBits::eTransferDst;

        api_data.createBuffer(size, usageFlags, vk::MemoryPropertyFlagBits::eHostVisible,
                              buffer, memory);
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
        api_data.device.destroy(buffer);
        api_data.device.freeMemory(memory);
    }
    allocated = false;
    return *this;
}

Buffer_API_Data::~Buffer_API_Data() { free(); }

NSP_GL_END
