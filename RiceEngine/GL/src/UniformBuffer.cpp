/*
 * UniformBuffer.cpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

#include "Rice/GL/Buffer.hpp"
#include "Rice/Util/Exceptions/Exception.hpp"
#include "pch.h"

#include "Vulkan_API_code/api_Buffer.hpp"
#include "Vulkan_API_code/api_Shader.hpp"
#include <Rice/GL/UniformBuffer.hpp>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

UniformBuffer::UniformBuffer(ptr<GraphicsManager> g_mgr, uint n)
    : Buffer(g_mgr, BufferUsage::Uniform, n) {}

void UniformBuffer::setShader(ptr<Shader> sh, uint binding) {
    shader = sh;
    using namespace vk;

    DescriptorBufferInfo info;
    info.buffer = api_data->buffer;
    info.offset = 0;
    info.range = api_data->buffer_size;

    auto write = WriteDescriptorSet()
                     .setDescriptorType(DescriptorType::eUniformBuffer)
                     .setDescriptorCount(1)
                     .setDstSet(sh->api_data->descriptorSet)
                     .setDstBinding(binding)
                     .setPBufferInfo(&info);

    get_api_data().device.updateDescriptorSets(1, &write, 0, nullptr);
}

void UniformBuffer::cleanup() {
    if (isAllocated())
        api_data->free();
}

bool UniformBuffer::isAllocated() { return api_data->buffer_size != 0; }

void UniformBuffer::reset(uint size) {
    cleanup();

    if (size != 0) {
        api_data->allocate(get_api_data(), size, BufferUsage::Uniform);
    }
}

NSP_GL_END
