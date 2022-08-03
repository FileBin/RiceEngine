/*
 * api_UniformBuffer_impl.hpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

/*
 * api_Buffer_impl.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "VulkanHelper.hpp"

#include "api_GraphicsManager.hpp"
#include "api_UniformBuffer.hpp"

NSP_GL_BEGIN

UniformBuffer_API_Data::UniformBuffer_API_Data(
    GraphicsManager_API_data &api_data)
    : device(api_data.device), GPU(api_data.GPU) {
    buffer_count = api_data.swapchainImages.size();

    buffers.resize(buffer_count);
    memories.resize(buffer_count);

    vk::DescriptorPoolSize poolSize{};
    poolSize.type = vk::DescriptorType::eUniformBuffer;
    poolSize.descriptorCount = buffer_count;

    vk::DescriptorPoolCreateInfo poolInfo{};
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = buffer_count;
    poolInfo.setFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);

    pool = device.createDescriptorPool(poolInfo);
}

void UniformBuffer_API_Data::allocateDescriptorSets(
    vk::DescriptorSetLayout layout, vk::PipelineLayout pipeline_layout) {
    pip_layout = pipeline_layout;
    std::vector<vk::DescriptorSetLayout> layouts(buffer_count, layout);
    vk::DescriptorSetAllocateInfo allocInfo{};
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = static_cast<uint>(buffer_count);
    allocInfo.pSetLayouts = layouts.data();

    sets = device.allocateDescriptorSets(allocInfo);

    built = true;
}

void UniformBuffer_API_Data::freeDescriptorSets() {
    if (built)
        device.freeDescriptorSets(pool, sets);
    built = false;
}

void UniformBuffer_API_Data::setBinding(uint binding, uint size) {
    for (uint i = 0; i < buffer_count; ++i) {
        using namespace vk;
        DescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = size;

        WriteDescriptorSet descriptorWrite{};
        descriptorWrite.dstSet = sets[i];
        descriptorWrite.dstBinding = binding;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = DescriptorType::eUniformBuffer;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;

        device.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);
    }
}

UniformBuffer_API_Data &UniformBuffer_API_Data::allocate(size_t size) {
    using namespace vk;
    if (!allocated) {
        for (uint i = 0; i < buffer_count; i++) {
            buffer_size = size;

            BufferCreateInfo bufferInfo{};
            bufferInfo.size = size;
            bufferInfo.usage = BufferUsageFlagBits::eUniformBuffer;
            bufferInfo.sharingMode = SharingMode::eExclusive;

            buffers[i] = device.createBuffer(bufferInfo);

            MemoryRequirements memReq =
                device.getBufferMemoryRequirements(buffers[i]);

            MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memReq.size;
            allocInfo.memoryTypeIndex =
                findMemoryType(memReq.memoryTypeBits,
                               MemoryPropertyFlagBits::eHostVisible |
                                   MemoryPropertyFlagBits::eHostCoherent);

            memories[i] = device.allocateMemory(allocInfo);
            device.bindBufferMemory(buffers[i], memories[i], 0);
        }
    }
    allocated = true;

    return *this;
}

UniformBuffer_API_Data &UniformBuffer_API_Data::setData(uint i, void *pData,
                                                        size_t nData,
                                                        size_t offset) {
    void *mappedData;

    mappedData = device.mapMemory(memories[i], offset, nData);
    memcpy(mappedData, pData, nData);
    device.unmapMemory(memories[i]);

    return *this;
}

UniformBuffer_API_Data &UniformBuffer_API_Data::getData(uint i, void *pData,
                                                        size_t nData,
                                                        size_t offset) {
    void *mappedData;

    mappedData = device.mapMemory(memories[i], offset, nData);
    memcpy(pData, mappedData, nData);
    device.unmapMemory(memories[i]);

    return *this;
}

UniformBuffer_API_Data &UniformBuffer_API_Data::free() {
    if (allocated) {
        for (uint i = 0; i < buffer_count; i++) {
            device.destroy(buffers[i]);
            device.freeMemory(memories[i]);
        }
    }

    allocated = false;
    freeDescriptorSets();
    return *this;
}

UniformBuffer_API_Data::~UniformBuffer_API_Data() { free(); }

uint UniformBuffer_API_Data::findMemoryType(
    uint typeFilter, vk::MemoryPropertyFlags properties) {
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

NSP_GL_END
