
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

    CommandBufferAllocateInfo allocInfo{};
    allocInfo.level = CommandBufferLevel::ePrimary;
    allocInfo.commandPool = api_data.commandPool;
    allocInfo.commandBufferCount = 1;

    cmd = api_data.device.allocateCommandBuffers(allocInfo)[0];
}

MemoryManager::~MemoryManager() {
    api_data.device.freeCommandBuffers(api_data.commandPool, 1, &cmd);
}

void MemoryManager::copyDataToBuffer(void *pData, size_t nData, size_t dstOffset,
                                     vk::Buffer dstBuffer) {
    MemoryChunk::ptr_t chunk_offset;
    if (nData > MemoryChunk::chunk_size) {
        //THROW_EXCEPTION("Copy chunk is too big");
        // FIXME does not working properly
        for (size_t offset = 0; offset < nData; offset += MemoryChunk::chunk_size) {
            size_t rest = nData - offset;
            rest = rest > MemoryChunk::chunk_size ? MemoryChunk::chunk_size : rest;
            copyDataToBuffer((char *)pData + offset, rest, dstOffset + offset, dstBuffer);
        }
        return;
    }
    auto it = chunks.end() - 1;

    if (!(*it)->allocateData(nData, chunk_offset)) {
        bool allocated = false;
        for (it = chunks.end() - 2; it >= chunks.begin(); it--) {
            if ((*it)->allocateData(nData, chunk_offset)) {
                allocated = true;
                break;
            }
        }
        if (!allocated) {
            chunks.push_back(std::make_unique<MemoryChunk>(api_data));
            it = chunks.end() - 1;
            if (!(*it)->allocateData(nData, chunk_offset)) {
                THROW_EXCEPTION("Allocation is too big!");
            }
        }
    }

    auto &chunk = **it;

    void *mappedData = api_data.device.mapMemory(chunk.memory, chunk_offset, nData);
    memcpy(mappedData, pData, nData);
    api_data.device.unmapMemory(chunk.memory);

    MemoryRegion region;
    region.src = chunk.buffer;
    region.dst = dstBuffer;
    region.copy =
        vk::BufferCopy().setSrcOffset(chunk_offset).setDstOffset(dstOffset).setSize(nData);

    copyRegions.push_back(region);

    timers.push_back(std::make_unique<MemoryTimer>(chunk, chunk_offset, maxLiveFrames));
}

void MemoryManager::update() {
    cmd.reset();
    CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmd.begin(beginInfo);

    for (auto region : copyRegions)
        cmd.copyBuffer(region.src, region.dst, 1, &region.copy);
    copyRegions.clear();

    cmd.end();

    SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;

    auto res = api_data.graphicsQueue.submit(1, &submitInfo, nullptr);

    auto removeQueue = vec<std::list<uptr<MemoryTimer>>::iterator>{};
    for (auto it = timers.begin(); it != timers.end(); it++) {
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

MemoryTimer::~MemoryTimer() { chunk.freeData(chunkOffset); }

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

bool MemoryChunk::allocateData(size_t nData, ptr_t &offset) {
    // find place in the end of buffer
    if (nData <= chunk_size - allocEnd) {
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

MemoryTimer::MemoryTimer(MemoryChunk &chunk, MemoryChunk::ptr_t chunkOffset, int frames)
    : chunk(chunk), framesToDestroy(frames), chunkOffset(chunkOffset) {}

NSP_GL_END
