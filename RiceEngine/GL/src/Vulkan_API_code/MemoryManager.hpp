#pragma once

#include "Rice/GL/namespaces.h"
#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <list>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

struct MemoryChunk {
    typedef uint16_t ptr_t;
    static constexpr size_t chunk_size = 0x10000; // 64KB
    static constexpr ptr_t npos = chunk_size - 1;

    size_t allocEnd = 0;
    vk::Device device;
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vec<ptr_t> allocations;

    MemoryChunk(GraphicsManager_API_data &api_data);
    MemoryChunk(const MemoryChunk &) = delete;
    MemoryChunk(MemoryChunk &&) = delete;
    ~MemoryChunk();
    bool allocateData(size_t nData, ptr_t &offset);
    bool freeData(ptr_t offset);
};

struct MemoryAllocation {
    MemoryChunk::ptr_t chunkOffset;
    MemoryChunk &chunk;
    MemoryAllocation(MemoryChunk &chunk, MemoryChunk::ptr_t chunkOffset);
    MemoryAllocation(const MemoryAllocation &) = delete;
    MemoryAllocation(MemoryAllocation &&) = delete;
    ~MemoryAllocation();
};

struct MemoryRegion {
    vk::Buffer src;
    vk::Buffer dst;
    vk::BufferCopy copy;
};

struct MemoryManager {
    typedef uptr<MemoryAllocation> Timer;
    typedef std::list<Timer> Timers;
    static constexpr auto maxLiveFrames = 8;
    friend struct GraphicsManager_API_data;

  private:
    GraphicsManager_API_data &api_data;
    vec<uptr<MemoryChunk>> chunks;
    uptr<Timers> timers[maxLiveFrames];
    std::list<MemoryRegion> copyRegions;
    vk::CommandBuffer cmd;
    ~MemoryManager();

  public:
    void copyDataToBuffer(void *pData, size_t nData, size_t dstOffset, vk::Buffer dstBuffer);

  private:
    MemoryManager(GraphicsManager_API_data &api_data);
    void update();
};

NSP_GL_END