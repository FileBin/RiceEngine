
#include "MemoryManager.hpp"
#include "Rice/GL/namespaces.h"
#include "Rice/Util/Exceptions/Exception.hpp"
#include <algorithm>
#include <cstddef>
#include <memory>
#include <sys/types.h>
#include <vector>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>

using namespace vk;

constexpr auto maxLiveFrames = 10;

NSP_GL_BEGIN

MemoryManager::MemoryManager(GraphicsManager_API_data &api_data) : api_data(api_data) {
    chunks.push_back(std::make_unique<MemoryChunk>(api_data));
}

void MemoryManager::copyDataToBuffer(void *pData, size_t nData, size_t dstOffset,
                                     vk::Buffer dstBuffer) {
    MemoryChunk::ptr_t chunk_offset;
    if (!chunks.back()->allocateData(nData, chunk_offset)) {
        chunks.push_back(std::make_unique<MemoryChunk>(api_data));

        if (!chunks.back()->allocateData(nData, chunk_offset)) {
            THROW_EXCEPTION("Allocation is too big!");
        }
    }

    auto &chunk = *chunks.back();

    void *mappedData = api_data.device.mapMemory(chunk.memory, chunk_offset, nData);
    memcpy(mappedData, pData, nData);
    api_data.device.unmapMemory(chunk.memory);

    CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = CommandBufferLevel::ePrimary;
    allocInfo.commandPool = api_data.commandPool;
    allocInfo.commandBufferCount = 1;

    auto commandBuffer = api_data.device.allocateCommandBuffers(allocInfo)[0];

    CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = CommandBufferUsageFlagBits::eOneTimeSubmit;

    commandBuffer.begin(beginInfo);

    vk::BufferCopy copyRegion;
    copyRegion.dstOffset = dstOffset;
    copyRegion.srcOffset = chunk_offset;
    copyRegion.size = nData;
    commandBuffer.copyBuffer(chunk.buffer, dstBuffer, 1, &copyRegion);

    commandBuffer.end();

    SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    auto res = api_data.graphicsQueue.submit(1, &submitInfo, nullptr);

    timers.push_back(std::make_unique<MemoryTimer>(api_data, commandBuffer, chunk,
                                                   chunk_offset, maxLiveFrames));
}

void MemoryManager::update() {
    auto removeQueue = vec<vec<uptr<MemoryTimer>>::iterator>{};
    for (auto it = timers.begin(); it < timers.end(); it++) {
        auto &timer = **it;
        timer.framesToDestroy--;
        if (timer.framesToDestroy <= 0) {
            removeQueue.push_back(it);
        }
    }
    for (auto it : removeQueue) {
        timers.erase(it);
    }
}

MemoryTimer::~MemoryTimer() {
    api_data.device.freeCommandBuffers(api_data.commandPool, 1, &copyCmdBuffer);
    chunk.freeData(chunkOffset);
}

MemoryChunk::MemoryChunk(GraphicsManager_API_data &api_data) : device(api_data.device) {
    api_data.createBuffer(chunk_size, BufferUsageFlagBits::eTransferSrc,
                          MemoryPropertyFlagBits::eHostVisible |
                              MemoryPropertyFlagBits::eHostCoherent,
                          buffer, memory);
}

MemoryChunk::~MemoryChunk() {
    device.destroy(buffer);
    device.free(memory);
}

bool MemoryChunk::allocateData(ptr_t nData, ptr_t &offset) {
    // find place in the end of buffer
    if (nData <= npos - allocEnd) {
        offset = allocEnd;
        allocations.insert(allocations.end(), offset);
        allocEnd += nData;
        return true;
    }
    // find place in the front of buffer
    if (nData <= allocations.front()) {
        offset = allocations.front() - nData;
        allocations.insert(allocations.begin(), offset);
        return true;
    }
    return false;
}

bool MemoryChunk::freeData(ptr_t offset) {
    if (allocations.empty())
        return false;
    auto end = allocations.end() - 1;
    for (auto it = allocations.begin(); it < end; it++) {
        if (*it == offset) {
            allocations.erase(it);
            return true;
        }
    }
    if (*end == offset) {
        allocations.erase(end);
        if (allocations.empty()) {
            allocEnd = 0;
        } else {
            allocEnd = offset;
        }
        return true;
    }
    return false;
}

MemoryTimer::MemoryTimer(GraphicsManager_API_data &api_data, vk::CommandBuffer copyCmdBuffer,
                         MemoryChunk &chunk, MemoryChunk::ptr_t chunkOffset, int frames)
    : api_data(api_data), copyCmdBuffer(copyCmdBuffer), chunk(chunk), framesToDestroy(frames),
      chunkOffset(chunkOffset) {}

NSP_GL_END
