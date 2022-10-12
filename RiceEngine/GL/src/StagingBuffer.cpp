/*
 * file: StagingBuffer.cpp
 * author: filebin
 * date: 2022-08-13 19:37:11
 */
#include "Rice/GL/GraphicsManager.hpp"
#include "pch.h"

#include "Rice/GL/Buffer.hpp"
#include "Rice/GL/GraphicsComponentBase.hpp"
#include "Rice/GL/StagingBuffer.hpp"

#include "Vulkan_API_code/api_Buffer.hpp"
#include "Vulkan_API_code/api_GraphicsManager.hpp"

NSP_GL_BEGIN

StagingBuffer::StagingBuffer(ptr<GraphicsManager> g_mgr, ptr<Buffer> dst, uint n, void *data)
    : GraphicsComponentBase(g_mgr) {
    api_data.reset(new Buffer_API_Data(get_api_data().device, get_api_data().GPU));
    // TODO
}

StagingBuffer::~StagingBuffer() {}

NSP_GL_END