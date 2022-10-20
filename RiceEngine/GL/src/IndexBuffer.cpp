/*
 * IndexBuffer.cpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "Vulkan_API_code/VulkanHelper.hpp"
#include "Vulkan_API_code/api_Buffer.hpp"
#include "pch.h"
#include <Rice/GL/IndexBuffer.hpp>

constexpr auto stride = sizeof(index_t);

NSP_GL_BEGIN

IndexBuffer::IndexBuffer(ptr<GraphicsManager> g_mgr, vec<index_t> data) : Buffer(g_mgr) {
    reset(data);
}

void IndexBuffer::cleanup() {
    if (is_allocated) {
        api_data->free();
    }
}

void IndexBuffer::reset(vec<index_t> data) {
    cleanup();
    if (!data.empty()) {
        uint n = data.size();

        current_count = n;

        allocate(n * stride, BufferUsage::Index);
        is_allocated = true;

        setData(data.data(), n * stride, 0);
    }
}

void IndexBuffer::updateIndices(vec<index_t> indices, uint pos) {
    setData(indices.data(), indices.size() * stride, pos * stride);
}

uint IndexBuffer::indexCount() { return current_count; }

NSP_GL_END
