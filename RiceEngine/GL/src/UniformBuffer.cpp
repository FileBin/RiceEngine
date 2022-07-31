/*
 * UniformBuffer.cpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "pch.h"

#include "Vulkan_API_code/api_Shader.hpp"
#include "Vulkan_API_code/api_UniformBuffer.hpp"
#include "Vulkan_API_code/api_UniformBuffer_impl.hpp"
#include <Rice/GL/UniformBuffer.hpp>

NSP_GL_BEGIN

UniformBuffer::UniformBuffer(ptr<GraphicsManager> g_mgr, uint n)
    : GraphicsComponentBase(g_mgr) {
    api_data =
        std::move(uptr<UniformBuffer_API_Data>(new UniformBuffer_API_Data(
            get_api_data(), get_api_data().device, get_api_data().GPU)));
    api_data->allocate(n);
}

void UniformBuffer::setShader(ptr<Shader> shader) {
    api_data->allocateDescriptorSets(shader->api_data->descriptorSetLayout,
                                     shader->api_data->layout);
}

void UniformBuffer::setBinding(uint b, uint n) { api_data->setBinding(b, n); }

void UniformBuffer::cleanup() {
    if (isAllocated())
        api_data->free();
}

bool UniformBuffer::isAllocated() { return api_data->buffer_size != 0; }

void UniformBuffer::reset(uint size) {
    cleanup();

    if (size != 0) {
        api_data->allocate(size);
    }
}

void UniformBuffer::updateData(void *pData, uint nData) {
    api_data->setData(get_api_data().swapchainImageIndex, pData, nData, 0);
}

void UniformBuffer::updateDataAll(void *pData, uint nData) {
    for (uint i = 0; i < api_data->buffer_count; i++)
        api_data->setData(i, pData, nData, 0);
}

void UniformBuffer::get_data(void *d, uint n) { api_data->getData(0, d, n, 0); }

UniformBuffer::~UniformBuffer() { cleanup(); }

NSP_GL_END
