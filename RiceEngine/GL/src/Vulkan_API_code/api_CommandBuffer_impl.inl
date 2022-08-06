/*
 * api_CommandBuffer_impl.hpp
 *
 *  Created on: 27 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "Rice/GL/CommandBuffer.hpp"
#include "Rice/GL/IndexBuffer.hpp"
#include "Rice/GL/UniformBuffer.hpp"
#include "Rice/GL/VertexBuffer.hpp"
#include "api_Buffer.hpp"
#include "api_CommandBuffer.hpp"
#include "api_GraphicsManager.hpp"
#include "api_Shader.hpp"
#include "api_UniformBuffer.hpp"
#include <array>
#include <cstdint>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

struct DescriptorSetCreator {
    uint maxCount = 0;
    vk::PipelineLayout layout;
    vk::DescriptorSetLayout descriptorSetLayout;
    vec<vk::WriteDescriptorSet> writes;
    vec<vk::DescriptorBufferInfo> bufferInfos;

    void addWrite(ptr<UniformBuffer> buffer, uint binding) {
        using namespace vk;
        auto &api_data = buffer->api_data;

        DescriptorBufferInfo info;
        info.buffer = api_data->buffer;
        info.offset = 0;
        info.range = api_data->buffer_size;

        bufferInfos.push_back(info);

        WriteDescriptorSet write;
        write.descriptorType = DescriptorType::eUniformBuffer;
        write.dstArrayElement = 0;
        write.dstBinding = binding;
        write.descriptorCount = 1;

        writes.push_back(write);
        maxCount++;
    }
};

inline void CommandBuffer_API_data::build(GraphicsManager_API_data &api_data) {
    // allocate the default command buffer that we will use for rendering
    vk::CommandBufferAllocateInfo info = {};

    info.commandPool = api_data.commandPool;
    info.commandBufferCount = api_data.swapchainImages.size();
    info.level = vk::CommandBufferLevel::ePrimary;

    cmd = api_data.device.allocateCommandBuffers(info);
}

inline void CommandBuffer_API_data::begin(GraphicsManager_API_data &api_data,
                                          vk::Extent2D window, uint i) {
    uint n = cmd.size();
    vk::CommandBufferBeginInfo cmdBeginInfo;

    cmd[i].begin(cmdBeginInfo);

    std::vector<vk::ClearAttachment> clearValues;
    clearValues.resize(2);

    clearValues[0].setAspectMask(vk::ImageAspectFlagBits::eColor);
    clearValues[0].setClearValue(
        vk::ClearColorValue(std::array<float, 4>{0.1f, 0.2f, 0.8f, 1.0f}));
    clearValues[0].setColorAttachment(0);
    clearValues[1].setAspectMask(vk::ImageAspectFlagBits::eDepth);
    clearValues[1].setClearValue(vk::ClearDepthStencilValue(1.0f, 0));
    clearValues[1].setColorAttachment(1);

    /*clearValues[0].clearValue.color.float32[0] = .1f;
    clearValues[0].clearValue.color.float32[1] = .2f;
    clearValues[0].clearValue.color.float32[2] = .8f;
    clearValues[0].clearValue.color.float32[3] = 1.f;

    clearValues[1].clearValue.depthStencil.depth = 1.01f;
    clearValues[1].clearValue.depthStencil.stencil = 0;*/
    // start the main renderpass.
    // We will use the clear color from above, and the framebuffer of the
    // index the swapchain gave us
    vk::RenderPassBeginInfo rpInfo = {};

    // windowExcent.width = window->getWidth();
    // windowExcent.height = window->getHeight();

    // if (begin_pass) {
    //     rpInfo.renderPass = api_data.begin_renderPass;
    // } else {
    rpInfo.renderPass = api_data.def_renderPass;
    //}
    rpInfo.renderArea.offset.x = 0;
    rpInfo.renderArea.offset.y = 0;
    rpInfo.renderArea.extent = window;
    rpInfo.framebuffer = api_data.framebuffers[i];

    vk::ClearRect rect = {};
    rect.rect.extent = window;
    rect.layerCount = 1;

    cmd[i].beginRenderPass(&rpInfo, vk::SubpassContents::eInline);

    if (begin_pass) {
        cmd[i].clearAttachments(clearValues.size(), clearValues.data(), 1,
                                &rect);
    }
}

inline void
CommandBuffer_API_data::doCommand(ptr<CommandBuffer::Command> command, uint i,
                                  GraphicsManager_API_data &api_data,
                                  DescriptorSetCreator &creator) {
    using namespace vk;

    uint n = cmd.size();

    switch (command->cmd) {
    case CommandBuffer::Command::Draw: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto count = *(uint *)(it++).current->getData();
        auto instCount = *(uint *)(it++).current->getData();
        auto vert_begin = *(uint *)(it++).current->getData();
        auto inst_begin = *(uint *)(it++).current->getData();
        bindDescriptosSets(api_data, creator, i);
        cmd[i].draw(count, instCount, vert_begin, inst_begin);
    } break;

    case CommandBuffer::Command::DrawIndexed: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto ib = *(ptr<IndexBuffer> *)(it++).current->getData();
        auto count = ib->indexCount();
        auto instCount = 1;
        auto index_offset = 0;
        auto vert_begin = 0;
        auto inst_begin = 0;
        bindDescriptosSets(api_data, creator, i);
        cmd[i].drawIndexed(count, instCount, index_offset, vert_begin,
                           inst_begin);
    } break;

    case CommandBuffer::Command::SetShader: {
        auto &sh_api_data =
            (*(ptr<Shader> *)command->arg_chain->getData())->api_data;
        cmd[i].bindPipeline(vk::PipelineBindPoint::eGraphics,
                            sh_api_data->pipeline);
        creator.layout = sh_api_data->layout;
        creator.descriptorSetLayout = sh_api_data->descriptorSetLayout;
    } break;

    case CommandBuffer::Command::BindVertexBuffer: {
        auto buf = *(ptr<VertexBuffer> *)command->arg_chain->getData();
        vk::Buffer vk_buf = buf->api_data->buffer;
        DeviceSize offset = 0;
        cmd[i].bindVertexBuffers(0, 1, &vk_buf,
                                 &offset); // @suppress("Ambiguous problem")
    } break;

    case CommandBuffer::Command::BindIndexBuffer: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto buf = *(ptr<IndexBuffer> *)(it++).current->getData();
        IndexType idx_ty;

        if (sizeof(index_t) == sizeof(uint16_t))
            idx_ty = IndexType::eUint16;
        else if (sizeof(index_t) == sizeof(uint))
            idx_ty = IndexType::eUint32;
        else
            THROW_EXCEPTION("Unknown index buffer type");

        vk::Buffer vk_buf = buf->api_data->buffer;
        DeviceSize offset = 0;

        cmd[i].bindIndexBuffer(vk_buf, offset, idx_ty);
    } break;

    case CommandBuffer::Command::PushConstants: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto shader = *(ptr<Shader> *)(it++).current->getData();

        auto stage = *(Shader::Type *)(it++).current->getData();

        auto nData = *(uint *)(it++).current->getData();
        void *pData = 0;
        pData = (it++).current->getData();

        ShaderStageFlags flags;

        if (stage == Shader::Vertex)
            flags = vk::ShaderStageFlagBits::eVertex;

        else if (stage == Shader::Fragment)
            flags = vk::ShaderStageFlagBits::eFragment;

        else if (stage == Shader::Geometry)
            flags = vk::ShaderStageFlagBits::eGeometry;

        cmd[i].pushConstants(shader->api_data->layout, flags, 0, nData, pData);
    } break;

    case CommandBuffer::Command::BindUniformBuffer: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto ubo = *(ptr<UniformBuffer> *)(it++).current->getData();
        auto binding = *(uint *)(it++).current->getData();
        creator.addWrite(ubo, binding);
    } break;

    default:
        THROW_EXCEPTION("Unknown command occured!");
        break;
    }
}

//NEEDIMPROVE 
inline void CommandBuffer_API_data::bindDescriptosSets(
    GraphicsManager_API_data &api_data, DescriptorSetCreator &creator, uint i) {
    if (!creator.writes.empty()) {
        if (!descriptorSet)
            createDescriptorSet(api_data, creator);
    }
    if (descriptorSet)
        cmd[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                  creator.layout, 0, 1, &descriptorSet, 0,
                                  nullptr);
}

inline void
CommandBuffer_API_data::createDescriptorSet(GraphicsManager_API_data &api_data,
                                            DescriptorSetCreator &creator) {
    using namespace vk;
    uint maxCount = creator.maxCount;
    vec<DescriptorPoolSize> sizes = {
        {DescriptorType::eUniformBuffer, maxCount}};

    DescriptorPoolCreateInfo pool_info = {};
    pool_info.maxSets = maxCount;
    pool_info.poolSizeCount = (uint32_t)sizes.size();
    pool_info.pPoolSizes = sizes.data();

    auto res = api_data.device.createDescriptorPool(&pool_info, nullptr,
                                                    &descriptorPool);
    THROW_VK_EX_IF_BAD(res);

    DescriptorSetAllocateInfo allocInfo = {};
    // using the pool we just set
    allocInfo.descriptorPool = descriptorPool;
    // only 1 descriptor
    allocInfo.descriptorSetCount = 1;
    // using the global data layout
    allocInfo.pSetLayouts = &creator.descriptorSetLayout;

    res = api_data.device.allocateDescriptorSets(&allocInfo, &descriptorSet);
    THROW_VK_EX_IF_BAD(res);
    auto n = creator.writes.size();
    for (uint i = 0; i < n; ++i) {
        auto& write = creator.writes[i];
        write.dstSet = descriptorSet;
        write.pBufferInfo = &creator.bufferInfos[i];
    }
    api_data.device.updateDescriptorSets(creator.writes.size(),
                                         creator.writes.data(), 0, nullptr);
}

inline void CommandBuffer_API_data::end(uint i) {
    cmd[i].endRenderPass();
    cmd[i].end();
}

inline void CommandBuffer_API_data::reset(uint i) { cmd[i].reset(); }

inline uint CommandBuffer_API_data::bufCount() { return cmd.size(); }

inline void
CommandBuffer_API_data::cleanup(GraphicsManager_API_data &api_data) {
    api_data.device.free(api_data.commandPool, cmd);
}

NSP_GL_END
