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

NSP_GL_BEGIN

Buffer_API_Data::Buffer_API_Data(GraphicsManager_API_data &api_data) : api_data(api_data) {}

Buffer_API_Data &Buffer_API_Data::allocate(size_t size, BufferUsage usage) {
    using namespace vk;
    if (!allocated) {
        buffer_size = size;

        BufferCreateInfo bufferInfo{};
        bufferInfo.size = size;
        switch (usage) {
        case BufferUsage::Index:
            bufferInfo.usage = BufferUsageFlagBits::eIndexBuffer;
            break;
        case BufferUsage::Vertex:
            bufferInfo.usage = BufferUsageFlagBits::eVertexBuffer;
            break;
        case BufferUsage::Uniform:
            bufferInfo.usage = BufferUsageFlagBits::eUniformBuffer;
            break;
        }

        bufferInfo.sharingMode = SharingMode::eExclusive;

        buffer = api_data.device.createBuffer(bufferInfo);

        MemoryRequirements memReq = api_data.device.getBufferMemoryRequirements(buffer);

        MemoryAllocateInfo allocInfo;
        allocInfo.allocationSize = memReq.size;
        allocInfo.memoryTypeIndex =
            api_data.findMemoryType(memReq.memoryTypeBits, MemoryPropertyFlagBits::eDeviceLocal);

        memory = api_data.device.allocateMemory(allocInfo);
        api_data.device.bindBufferMemory(buffer, memory, 0);
    }
    allocated = true;

    return *this;
}

Buffer_API_Data &Buffer_API_Data::setData(void *pData, size_t nData, size_t offset) {

    void *mappedData;
    mappedData = api_data.device.mapMemory(memory, offset, nData);
    memcpy(mappedData, pData, nData);
    api_data.device.unmapMemory(memory);

    return *this;
}

Buffer_API_Data &Buffer_API_Data::getData(void *pData, size_t nData, size_t offset) {
    void *mappedData;
    mappedData = api_data.device.mapMemory(memory, offset, nData);
    memcpy(pData, mappedData, nData);
    api_data.device.unmapMemory(memory);

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
