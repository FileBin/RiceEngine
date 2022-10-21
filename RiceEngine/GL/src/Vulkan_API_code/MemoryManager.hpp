#pragma once

#include "Rice/GL/namespaces.h"
#include "Rice/defines.h"
#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

struct MemoryChunk {
    typedef uint16_t ptr_t;
    static constexpr uint chunk_size = 0x10000; // 64KB
    static constexpr ptr_t npos = chunk_size - 1;

    ptr_t allocEnd = 0;
    vk::Device device;
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vec<ptr_t> allocations;

    MemoryChunk(GraphicsManager_API_data &api_data);
    MemoryChunk(const MemoryChunk &) = delete;
    MemoryChunk(MemoryChunk &&) = delete;
    ~MemoryChunk();
    bool allocateData(ptr_t nData, ptr_t &offset);
    bool freeData(ptr_t offset);
};

struct MemoryTimer {
    int framesToDestroy;
    GraphicsManager_API_data &api_data;
    vk::CommandBuffer copyCmdBuffer;
    MemoryChunk &chunk;
    MemoryChunk::ptr_t chunkOffset;
    MemoryTimer(GraphicsManager_API_data &api_data, vk::CommandBuffer copyCmdBuffer,
                MemoryChunk &chunk, MemoryChunk::ptr_t chunkOffset, int framesToDestroy);
    MemoryTimer(const MemoryTimer &) = delete;
    MemoryTimer(MemoryTimer &&) = delete;
    ~MemoryTimer();
};

struct MemoryManager {
    friend struct GraphicsManager_API_data;

  private:
    GraphicsManager_API_data &api_data;
    vec<uptr<MemoryChunk>> chunks;
    vec<uptr<MemoryTimer>> timers;

  public:
    void copyDataToBuffer(void *pData, size_t nData, size_t dstOffset, vk::Buffer dstBuffer);

  private:
    MemoryManager(GraphicsManager_API_data &api_data);
    void update();
};

NSP_GL_END