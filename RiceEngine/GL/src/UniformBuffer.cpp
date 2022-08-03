/*
 * UniformBuffer.cpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "pch.h"

#include "Vulkan_API_code/api_Shader.hpp"
#include "Vulkan_API_code/api_UniformBuffer.hpp"
#include "Vulkan_API_code/api_UniformBuffer_impl.inl"
#include <Rice/GL/UniformBuffer.hpp>

NSP_GL_BEGIN

UniformBuffer::UniformBuffer(ptr<GraphicsManager> g_mgr, uint n)
    : GraphicsComponentBase(g_mgr) {
    api_data.reset(new UniformBuffer_API_Data(get_api_data()));
    api_data->allocate(n);
    g_mgr->resizeGraphicsComponents->subscribe(resizeReg, [this](Vector2i win) {
        onResize();
    }); // @suppress("Invalid arguments")
}

void UniformBuffer::onResize() {
    // FIXME do resize correctly
    api_data->freeDescriptorSets();
    build();
}

void UniformBuffer::setShader(ptr<Shader> sh) { shader = sh; }

void UniformBuffer::setBinding(uint b, uint n) {
    binding.binding = b;
    binding.size = n;
}

void UniformBuffer::build() {
    auto sh = shader.lock();
    if (sh) {
        api_data->allocateDescriptorSets(sh->api_data->descriptorSetLayout,
                                         sh->api_data->layout);
        api_data->setBinding(binding.binding, binding.size);
    }
}

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
