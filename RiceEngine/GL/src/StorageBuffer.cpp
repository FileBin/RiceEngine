/*
 * StorageBuffer.cpp
 */

#include "pch.h"

#include <Rice/GL/StorageBuffer.hpp>

#include <algorithm>
#include <cstring>

#include "Vulkan_API_code/api_Buffer.hpp"
#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

namespace {
vk::BufferUsageFlags storage_usage_flags(StorageUsage usage) {
    using namespace vk;
    const uint bits = static_cast<uint>(usage);

    BufferUsageFlags flags = BufferUsageFlagBits::eStorageBuffer;
    if (bits & static_cast<uint>(StorageUsage::Vertex))
        flags |= BufferUsageFlagBits::eVertexBuffer;
    if (bits & static_cast<uint>(StorageUsage::Index))
        flags |= BufferUsageFlagBits::eIndexBuffer;
    if (bits & static_cast<uint>(StorageUsage::Indirect))
        flags |= BufferUsageFlagBits::eIndirectBuffer;

    // The CPU writes and reads this buffer directly, and a capture/readback
    // destination is copied into from an image, so transfer both ways.
    flags |= BufferUsageFlagBits::eTransferDst | BufferUsageFlagBits::eTransferSrc;
    return flags;
}
} // namespace

StorageBuffer::StorageBuffer(ptr<GraphicsManager> g_mgr, size_t size, StorageUsage usage)
    : Buffer(g_mgr) {
    if (size == 0)
        THROW_EXCEPTION("StorageBuffer: size must be non-zero");
    api_data->allocate(size, storage_usage_flags(usage),
                       vk::MemoryPropertyFlagBits::eHostVisible);
}

StorageBuffer::~StorageBuffer() { cleanup(); }

size_t StorageBuffer::size() const { return api_data ? api_data->buffer_size : 0; }

void *StorageBuffer::map() {
    if (mapped != nullptr)
        return mapped;
    if (!api_data || api_data->buffer_size == 0)
        return nullptr;

    mapped = get_api_data().device.mapMemory(api_data->memory, 0, api_data->buffer_size);
    return mapped;
}

void StorageBuffer::unmap() {
    if (mapped == nullptr)
        return;

    get_api_data().device.unmapMemory(api_data->memory);
    mapped = nullptr;
}

void StorageBuffer::write(const void *src, size_t bytes, size_t offset) {
    if (src == nullptr || bytes == 0)
        return;

    void *dst = map();
    if (dst == nullptr)
        return;

    const size_t n = std::min(bytes, api_data->buffer_size > offset
                                         ? api_data->buffer_size - offset
                                         : size_t(0));
    if (n == 0)
        return;

    std::memcpy((char *)dst + offset, src, n);

    // eHostVisible does not imply eHostCoherent: without the flush the write is
    // not guaranteed to be visible to the GPU. Coherent memory (what desktop
    // drivers hand out) skips it, like UniformBuffer::updateData does.
    if (!api_data->host_coherent) {
        vk::MappedMemoryRange range;
        range.memory = api_data->memory;
        range.offset = 0;
        range.size = VK_WHOLE_SIZE;
        get_api_data().device.flushMappedMemoryRanges(1, &range);
    }
}

void StorageBuffer::read(void *dst, size_t bytes, size_t offset) {
    if (dst == nullptr || bytes == 0)
        return;

    const void *src = map();
    if (src == nullptr)
        return;

    const size_t n = std::min(bytes, api_data->buffer_size > offset
                                         ? api_data->buffer_size - offset
                                         : size_t(0));
    if (n == 0)
        return;

    std::memcpy(dst, (const char *)src + offset, n);
}

void StorageBuffer::cleanup() {
    if (!api_data)
        return;
    // A mapping is a property of the VkDeviceMemory: it has to go before the
    // memory is freed.
    unmap();
    api_data->free();
}

NSP_GL_END
