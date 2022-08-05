/*
 * api_CommandBuffer_impl.hpp
 *
 *  Created on: 27 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "Rice/GL/IndexBuffer.hpp"
#include "Rice/GL/VertexBuffer.hpp"
#include "api_Buffer.hpp"
#include "api_CommandBuffer.hpp"
#include "api_Shader.hpp"
#include "api_UniformBuffer.hpp"
#include <array>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

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
CommandBuffer_API_data::doCommand(ptr<CommandBuffer::Command> command, uint i) {
    using namespace vk;

    uint n = cmd.size();

    switch (command->cmd) {
    case CommandBuffer::Command::Draw: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto count = *(uint *)(it++).current->getData();
        auto instCount = *(uint *)(it++).current->getData();
        auto vert_begin = *(uint *)(it++).current->getData();
        auto inst_begin = *(uint *)(it++).current->getData();
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
        cmd[i].drawIndexed(count, instCount, index_offset, vert_begin,
                           inst_begin);
    } break;

    case CommandBuffer::Command::SetShader: {
        auto& sh_api_data =
            (*(ptr<Shader> *)command->arg_chain->getData())->api_data;
        cmd[i].bindPipeline(vk::PipelineBindPoint::eGraphics,
                            sh_api_data->pipeline);
        cmd[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                  sh_api_data->layout, 0, 1,
                                  &sh_api_data->descriptorSet, 0, nullptr);
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

    default:
        THROW_EXCEPTION("Unknown command occured!");
        break;
    }
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
