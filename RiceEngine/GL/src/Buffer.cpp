/*
 * Buffer.cpp
 *
 *  Created on: 26 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#include "Rice/defines.h"
#include "Vulkan_API_code/api_Buffer.hpp"
#include "pch.h"

#include <Rice/GL/Buffer.hpp>
#include <algorithm>

#include "Vulkan_API_code/api_Buffer_impl.inl"
#include "Vulkan_API_code/api_GraphicsManager.hpp"

NSP_GL_BEGIN

Buffer::Buffer(ptr<GraphicsManager> g_mgr) : GraphicsComponentBase(g_mgr) {
    build_api();
}

Buffer::Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage, size_t size)
    : GraphicsComponentBase(g_mgr) {
    build_api();
    allocate(size, usage);
}

Buffer::Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage,
               data_t initialData)
    : GraphicsComponentBase(g_mgr) {
    build_api();
    allocate(initialData.size(), usage);
    setData(initialData.data(), initialData.size());
}

void Buffer::build_api() {
    api_data = std::move(uptr<Buffer_API_Data>{
        new Buffer_API_Data(get_api_data().device, get_api_data().GPU)});
}

void Buffer::allocate(size_t size, BufferUsage usage) {
    api_data->allocate(get_api_data() ,size, usage);
}

void Buffer::setData(void *pData, size_t nData, size_t offset) {
    graphics_mgr->sync();
    api_data->setData(pData, nData, offset);
}

void Buffer::getData(void *pData, size_t nData, size_t offset) {
    graphics_mgr->sync();
    api_data->getData(pData, nData, offset);
}

Buffer::~Buffer() { cleanup(); }

void Buffer::cleanup() { delete api_data.release(); }

NSP_GL_END
