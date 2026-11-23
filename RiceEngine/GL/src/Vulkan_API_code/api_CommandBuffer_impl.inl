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
#include "Rice/Util/Interfaces.hpp"
#include "api_Buffer.hpp"
#include "api_CommandBuffer.hpp"
#include "api_GraphicsManager.hpp"
#include "api_Shader.hpp"
#include "api_ShadowMap.hpp"
#include "api_UniformBuffer.hpp"
#include <Rice/Math/Vectors.hpp>
#include <array>
#include <cstdint>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

/* BarrierStage / BarrierAccess (engine enums) -> vk stage and access masks. */
inline vk::PipelineStageFlags to_vk_stage(BarrierStage stage) {
    using namespace vk;
    const uint bits = static_cast<uint>(stage);
    PipelineStageFlags flags;

    if (bits & static_cast<uint>(BarrierStage::TopOfPipe))
        flags |= PipelineStageFlagBits::eTopOfPipe;
    if (bits & static_cast<uint>(BarrierStage::ComputeShader))
        flags |= PipelineStageFlagBits::eComputeShader;
    if (bits & static_cast<uint>(BarrierStage::VertexInput))
        flags |= PipelineStageFlagBits::eVertexInput;
    if (bits & static_cast<uint>(BarrierStage::DrawIndirect))
        flags |= PipelineStageFlagBits::eDrawIndirect;
    if (bits & static_cast<uint>(BarrierStage::Transfer))
        flags |= PipelineStageFlagBits::eTransfer;
    if (bits & static_cast<uint>(BarrierStage::FragmentShader))
        flags |= PipelineStageFlagBits::eFragmentShader;
    if (bits & static_cast<uint>(BarrierStage::AllGraphics))
        flags |= PipelineStageFlagBits::eAllGraphics;
    if (bits & static_cast<uint>(BarrierStage::AllCommands))
        flags |= PipelineStageFlagBits::eAllCommands;

    return flags;
}

inline vk::AccessFlags to_vk_access(BarrierAccess access) {
    using namespace vk;
    const uint bits = static_cast<uint>(access);
    AccessFlags flags;

    if (bits & static_cast<uint>(BarrierAccess::ShaderWrite))
        flags |= AccessFlagBits::eShaderWrite;
    if (bits & static_cast<uint>(BarrierAccess::ShaderRead))
        flags |= AccessFlagBits::eShaderRead;
    if (bits & static_cast<uint>(BarrierAccess::VertexAttributeRead))
        flags |= AccessFlagBits::eVertexAttributeRead;
    if (bits & static_cast<uint>(BarrierAccess::IndexRead))
        flags |= AccessFlagBits::eIndexRead;
    if (bits & static_cast<uint>(BarrierAccess::IndirectCommandRead))
        flags |= AccessFlagBits::eIndirectCommandRead;
    if (bits & static_cast<uint>(BarrierAccess::TransferRead))
        flags |= AccessFlagBits::eTransferRead;
    if (bits & static_cast<uint>(BarrierAccess::TransferWrite))
        flags |= AccessFlagBits::eTransferWrite;

    return flags;
}

struct DrawAdditionData : public ICleanable {
    vk::DescriptorSet descriptorSet;
    vk::DescriptorPool descriptorPool;
    GraphicsManager_API_data &api_data;
    DrawAdditionData(GraphicsManager_API_data &api_data) : api_data(api_data) {}

    void cleanup() override;
    void createDescriptorSet(DescriptorSetCreator &creator);
    void bindDescriptosSets(DescriptorSetCreator &creator, vk::CommandBuffer cmd,
                            vk::PipelineBindPoint bindPoint = vk::PipelineBindPoint::eGraphics);
};

struct DescriptorSetCreator {
    vk::PipelineLayout layout;
    vk::DescriptorSetLayout descriptorSetLayout;

    /* Binding numbers of the descriptor set layout of the shader that is
     * currently bound. The write map below is shared by the whole command
     * buffer, but a set may only contain the bindings its layout declares: a
     * shader built without Shader::addLighting() must not receive the scene
     * light block (2) or the shadow map (3), and a pipeline whose layout has no
     * such binding rejects a write to it. Filled in by the SetShader command. */
    std::set<uint> declared;

    struct WriteDescriptor {
        vk::WriteDescriptorSet write;
        vk::DescriptorBufferInfo buffer_info;
        vk::DescriptorImageInfo image_info;
    };

    map<uint, WriteDescriptor> writes;

    void addWrite(ptr<UniformBuffer> buffer, uint binding) {
        using namespace vk;
        auto &api_data = buffer->api_data;

        DescriptorBufferInfo info;
        info.buffer = api_data->buffer;
        info.offset = 0;
        info.range = api_data->buffer_size;

        WriteDescriptorSet write;
        write.descriptorType = DescriptorType::eUniformBuffer;
        write.dstArrayElement = 0;
        write.dstBinding = binding;
        write.descriptorCount = 1;

        writes[binding] = {write, info, {}};
    }

    /* A shader storage buffer (SSBO): the set 0 binding a compute pass writes
     * its output through, and the buffer a draw may also consume as vertex or
     * index data. Same DescriptorBufferInfo path as a uniform buffer, only the
     * descriptor type differs. */
    void addWrite(ptr<StorageBuffer> buffer, uint binding) {
        using namespace vk;
        auto &api_data = buffer->api_data;

        DescriptorBufferInfo info;
        info.buffer = api_data->buffer;
        info.offset = 0;
        info.range = api_data->buffer_size;

        WriteDescriptorSet write;
        write.descriptorType = DescriptorType::eStorageBuffer;
        write.dstArrayElement = 0;
        write.dstBinding = binding;
        write.descriptorCount = 1;

        writes[binding] = {write, info, {}};
    }

    /* The shadow map as a combined image sampler. The sampler has comparison
     * off, so the shader reads raw depth (texelFetch) and compares itself. */
    void addWrite(ptr<ShadowMap> map, uint binding) {
        using namespace vk;
        auto &api_data = map->api_data;

        DescriptorImageInfo info;
        info.sampler = api_data->getSampler();
        info.imageView = api_data->getView();
        info.imageLayout = ImageLayout::eShaderReadOnlyOptimal;

        WriteDescriptorSet write;
        write.descriptorType = DescriptorType::eCombinedImageSampler;
        write.dstArrayElement = 0;
        write.dstBinding = binding;
        write.descriptorCount = 1;

        writes[binding] = {write, {}, info};
    }
};

inline void CommandBuffer_API_data::build(GraphicsManager_API_data &api_data, bool secondary) {
    // allocate the default command buffer that we will use for rendering
    is_secondary = secondary;
    vk::CommandBufferAllocateInfo info = {};

    info.commandPool = api_data.commandPool;
    info.commandBufferCount = api_data.swapchainImages.size();
    info.level =
        secondary ? vk::CommandBufferLevel::eSecondary : vk::CommandBufferLevel::ePrimary;

    cmd = api_data.device.allocateCommandBuffers(info);
}

inline void CommandBuffer_API_data::begin(GraphicsManager_API_data &api_data,
                                          vk::Extent2D window, uint i) {
    uint n = cmd.size();

    vk::CommandBufferInheritanceInfo info;
    info.renderPass = api_data.def_renderPass;
    info.framebuffer = api_data.framebuffers[i];

    vk::CommandBufferBeginInfo cmdBeginInfo;
    if (is_secondary) {
        cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                             vk::CommandBufferUsageFlagBits::eSimultaneousUse;
        cmdBeginInfo.pInheritanceInfo = &info;
    }
    cmd[i].begin(cmdBeginInfo);
}

inline void CommandBuffer_API_data::doCommand(ptr<CommandBuffer::Command> command, uint i,
                                              GraphicsManager_API_data &api_data,
                                              DescriptorSetCreator &creator) {
    using namespace vk;

    uint n = cmd.size();

    switch (command->cmd) {
    case CommandBuffer::Command::BeginRenderPass: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto rect = *(Util::Rect *)(*it++).getData();
        bool secondary = *(bool *)(*it++).getData();

        rect.x *= api_data.windowExcent.width;
        rect.w *= api_data.windowExcent.width;
        rect.y *= api_data.windowExcent.height;
        rect.h *= api_data.windowExcent.height;

        // The pass expects the image in ColorAttachmentOptimal.
        api_data.prepareSwapchainImage(cmd[i], i);

        vk::RenderPassBeginInfo rpInfo = {};
        rpInfo.renderPass = api_data.def_renderPass;
        rpInfo.renderArea.offset.x = rect.x;
        rpInfo.renderArea.offset.y = rect.y;
        rpInfo.renderArea.extent = vk::Extent2D(rect.w, rect.h);
        rpInfo.framebuffer = api_data.framebuffers[i];

        using enum vk::SubpassContents;

        cmd[i].beginRenderPass(&rpInfo, secondary ? eSecondaryCommandBuffers : eInline);
    } break;

    case CommandBuffer::Command::ClearRenderTarget: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto color = *(Vector3f *)(*it++).getData();
        auto depth = *(float *)(*it++).getData();

        std::array<vk::ClearAttachment, 2> clearValues;

        clearValues[0].setAspectMask(vk::ImageAspectFlagBits::eColor);
        clearValues[0].setClearValue(
            vk::ClearColorValue(std::array<float, 4>{color.x, color.y, color.z, 1.0f}));
        clearValues[0].setColorAttachment(0);
        clearValues[1].setAspectMask(vk::ImageAspectFlagBits::eDepth);
        clearValues[1].setClearValue(vk::ClearDepthStencilValue(depth, 0));
        clearValues[1].setColorAttachment(1);

        vk::ClearRect rect = {};
        rect.rect.extent = api_data.windowExcent;
        rect.layerCount = 1;

        cmd[i].clearAttachments(clearValues.size(), clearValues.data(), 1, &rect);
    } break;

    case CommandBuffer::Command::ExecuteCommandBuffer: {
        auto sub_cmd = *(ptr<CommandBuffer> *)command->arg_chain->getData();
        cmd[i].executeCommands(1, &sub_cmd->api_data->cmd[i]);
    } break;

    case CommandBuffer::Command::EndRenderPass: {
        cmd[i].endRenderPass();
    } break;

    case CommandBuffer::Command::Draw: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto count = *(uint *)(it++).current->getData();
        auto instCount = *(uint *)(it++).current->getData();
        auto vert_begin = *(uint *)(it++).current->getData();
        auto inst_begin = *(uint *)(it++).current->getData();

        if (!command->additional_data) {
            auto draw_data = new DrawAdditionData(api_data);
            command->additional_data = static_cast<ICleanable *>(draw_data);
        }
        auto draw_data = dynamic_cast<DrawAdditionData *>(command->additional_data);
        draw_data->bindDescriptosSets(creator, cmd[i]);

        // creator.bufferInfos.clear();
        // creator.writes.clear();
        // creator.maxCount = 0;

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

        if (!command->additional_data) {
            auto draw_data = new DrawAdditionData(api_data);
            command->additional_data = static_cast<ICleanable *>(draw_data);
        }
        auto draw_data = dynamic_cast<DrawAdditionData *>(command->additional_data);
        draw_data->bindDescriptosSets(creator, cmd[i]);

        // creator.bufferInfos.clear();
        // creator.writes.clear();
        // creator.maxCount = 0;

        cmd[i].drawIndexed(count, instCount, index_offset, vert_begin, inst_begin);
    } break;

    case CommandBuffer::Command::SetShader: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto &sh_api_data = (*(ptr<Shader> *)(*it++).getData())->api_data;
        bool ghost = *(bool *)(*it++).getData();
        if (!ghost) {
            cmd[i].bindPipeline(vk::PipelineBindPoint::eGraphics, sh_api_data->pipeline);
        }
        creator.layout = sh_api_data->layout;
        creator.descriptorSetLayout = sh_api_data->descriptorSetLayout;
        creator.declared.clear();
        for (auto &b : sh_api_data->bindings)
            creator.declared.insert(b.binding);
    } break;

    case CommandBuffer::Command::BindVertexBuffer: {
        // Any Buffer: a VertexBuffer's storage or a StorageBuffer that compute
        // wrote and the vertex fetch now reads (§6.3).
        auto buf = *(ptr<Buffer> *)command->arg_chain->getData();
        vk::Buffer vk_buf = buf->api_data->buffer;
        DeviceSize offset = 0;
        cmd[i].bindVertexBuffers(0, 1, &vk_buf,
                                 &offset); // @suppress("Ambiguous problem")
    } break;

    case CommandBuffer::Command::BindIndexBuffer: {
        auto buf = *(ptr<Buffer> *)command->arg_chain->getData();
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

        else
            flags = vk::ShaderStageFlagBits::eCompute;

        cmd[i].pushConstants(shader->api_data->layout, flags, 0, nData, pData);
    } break;

    case CommandBuffer::Command::BindUniformBuffer: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto ubo = *(ptr<UniformBuffer> *)(it++).current->getData();
        auto binding = *(uint *)(it++).current->getData();
        creator.addWrite(ubo, binding);
    } break;

    case CommandBuffer::Command::BeginShadowPass: {
        auto shadow_map = *(ptr<ShadowMap> *)command->arg_chain->getData();
        auto &sdata = *shadow_map->api_data;

        vk::RenderPassBeginInfo rpInfo = {};
        rpInfo.renderPass = api_data.shadow_renderPass;
        rpInfo.framebuffer = sdata.getFramebuffer();
        rpInfo.renderArea.offset.x = 0;
        rpInfo.renderArea.offset.y = 0;
        rpInfo.renderArea.extent = vk::Extent2D(sdata.size, sdata.size);

        // The pass clears the depth attachment itself; 1.0 = far = "nothing
        // here", which is what the shader's PCF treats as fully lit.
        vk::ClearValue clear{};
        clear.depthStencil = vk::ClearDepthStencilValue(1.f, 0);
        rpInfo.clearValueCount = 1;
        rpInfo.pClearValues = &clear;

        cmd[i].beginRenderPass(&rpInfo, vk::SubpassContents::eInline);
    } break;

    case CommandBuffer::Command::EndShadowPass: {
        cmd[i].endRenderPass();
    } break;

    case CommandBuffer::Command::BindShadowMap: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto shadow_map = *(ptr<ShadowMap> *)(it++).current->getData();
        auto binding = *(uint *)(it++).current->getData();
        creator.addWrite(shadow_map, binding);
    } break;

    case CommandBuffer::Command::BindStorageBuffer: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto ssbo = *(ptr<StorageBuffer> *)(it++).current->getData();
        auto binding = *(uint *)(it++).current->getData();
        creator.addWrite(ssbo, binding);
    } break;

    case CommandBuffer::Command::Dispatch: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto shader = *(ptr<Shader> *)(it++).current->getData();
        auto x = *(uint *)(it++).current->getData();
        auto y = *(uint *)(it++).current->getData();
        auto z = *(uint *)(it++).current->getData();

        auto &sh = *shader->api_data;
        cmd[i].bindPipeline(PipelineBindPoint::eCompute, sh.pipeline);

        // The compute pass has its own set 0: rebuilt from the SSBOs bound since
        // the last draw/dispatch, and only published to the bindings this
        // shader declared - the same rule DrawAdditionData applies to graphics.
        creator.layout = sh.layout;
        creator.descriptorSetLayout = sh.descriptorSetLayout;
        creator.declared.clear();
        for (auto &b : sh.bindings)
            creator.declared.insert(b.binding);

        if (!command->additional_data)
            command->additional_data = new DrawAdditionData(api_data);
        auto draw_data = dynamic_cast<DrawAdditionData *>(command->additional_data);
        draw_data->bindDescriptosSets(creator, cmd[i], PipelineBindPoint::eCompute);

        cmd[i].dispatch(x, y, z);
    } break;

    case CommandBuffer::Command::PipelineBarrier: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto src_stage = *(BarrierStage *)(it++).current->getData();
        auto src_access = *(BarrierAccess *)(it++).current->getData();
        auto dst_stage = *(BarrierStage *)(it++).current->getData();
        auto dst_access = *(BarrierAccess *)(it++).current->getData();

        vk::MemoryBarrier barrier{};
        barrier.srcAccessMask = to_vk_access(src_access);
        barrier.dstAccessMask = to_vk_access(dst_access);

        cmd[i].pipelineBarrier(to_vk_stage(src_stage), to_vk_stage(dst_stage),
                               vk::DependencyFlags{}, 1, &barrier, 0, nullptr, 0, nullptr);
    } break;

    case CommandBuffer::Command::DrawIndexedIndirect: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto draws = *(ptr<Buffer> *)(it++).current->getData();
        auto first_draw = *(uint *)(it++).current->getData();
        auto draw_count = *(uint *)(it++).current->getData();

        if (!command->additional_data)
            command->additional_data = new DrawAdditionData(api_data);
        auto draw_data = dynamic_cast<DrawAdditionData *>(command->additional_data);
        draw_data->bindDescriptosSets(creator, cmd[i]);

        const DeviceSize stride = sizeof(vk::DrawIndexedIndirectCommand);
        cmd[i].drawIndexedIndirect(draws->api_data->buffer, first_draw * stride, draw_count,
                                   stride);
    } break;

    case CommandBuffer::Command::DrawIndexedRange: {
        CommandBuffer::Command::ArgIterator it = command->arg_chain;
        auto count = *(uint *)(it++).current->getData();
        auto first_index = *(uint *)(it++).current->getData();
        auto vertex_offset = *(int32_t *)(it++).current->getData();

        if (!command->additional_data)
            command->additional_data = new DrawAdditionData(api_data);
        auto draw_data = dynamic_cast<DrawAdditionData *>(command->additional_data);
        draw_data->bindDescriptosSets(creator, cmd[i]);

        cmd[i].drawIndexed(count, 1, first_index, vertex_offset, 0);
    } break;

    case CommandBuffer::Command::CaptureColor: {
        auto dst = *(ptr<Buffer> *)command->arg_chain->getData();
        if (i >= api_data.swapchainImages.size())
            break;

        const vk::Image image = api_data.swapchainImages[i];
        const vk::ImageSubresourceRange range(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);

        const vk::DeviceSize needed = vk::DeviceSize(api_data.windowExcent.width) *
                                      api_data.windowExcent.height * 4;
        if (dst->api_data->buffer_size < needed)
            // A resize after the buffer was sized: skip rather than record an
            // out-of-bounds copy. The test that wanted the pixels then sees an
            // empty image and fails, which is the loud outcome.
            break;

        // The render pass hands the image over in PresentSrcKHR. Take it to
        // TransferSrcOptimal, copy, and hand it back: the last layout of the
        // command buffer is the one the presentation engine expects.
        vk::ImageMemoryBarrier to_src{};
        to_src.oldLayout = vk::ImageLayout::ePresentSrcKHR;
        to_src.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        to_src.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        to_src.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        to_src.image = image;
        to_src.subresourceRange = range;
        to_src.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
        to_src.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        cmd[i].pipelineBarrier(vk::PipelineStageFlagBits::eColorAttachmentOutput,
                               vk::PipelineStageFlagBits::eTransfer, vk::DependencyFlags{}, 0,
                               nullptr, 0, nullptr, 1, &to_src);

        vk::BufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource =
            vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, 0, 0, 1);
        region.imageOffset = vk::Offset3D(0, 0, 0);
        region.imageExtent =
            vk::Extent3D(api_data.windowExcent.width, api_data.windowExcent.height, 1);

        cmd[i].copyImageToBuffer(image, vk::ImageLayout::eTransferSrcOptimal,
                                 dst->api_data->buffer, 1, &region);

        vk::ImageMemoryBarrier back = to_src;
        back.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        back.newLayout = vk::ImageLayout::ePresentSrcKHR;
        back.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        back.dstAccessMask = vk::AccessFlags{};

        cmd[i].pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                               vk::PipelineStageFlagBits::eBottomOfPipe, vk::DependencyFlags{},
                               0, nullptr, 0, nullptr, 1, &back);
    } break;

    case CommandBuffer::Command::CopyBuffer: {
        const auto &copy = *(CommandBuffer::BufferCopy *)command->arg_chain->getData();
        auto src = copy.src;
        auto dst = copy.dst;
        auto src_offset = copy.srcOffset;
        auto dst_offset = copy.dstOffset;
        auto size = copy.size;

        if (!src || !dst || size == 0)
            break;

        // Clamp to both buffers: a region that no longer fits (a page was
        // reserved smaller than the one it is copied from) is skipped rather
        // than recorded as an out-of-bounds copy.
        const vk::DeviceSize src_size = src->api_data->buffer_size;
        const vk::DeviceSize dst_size = dst->api_data->buffer_size;
        const vk::DeviceSize src_left = src_size > src_offset ? src_size - src_offset : 0;
        const vk::DeviceSize dst_left = dst_size > dst_offset ? dst_size - dst_offset : 0;
        const vk::DeviceSize n = std::min<vk::DeviceSize>(size, std::min(src_left, dst_left));
        if (n == 0)
            break;

        vk::BufferCopy region{};
        region.srcOffset = src_offset;
        region.dstOffset = dst_offset;
        region.size = n;
        cmd[i].copyBuffer(src->api_data->buffer, dst->api_data->buffer, 1, &region);
    } break;

    default:
        THROW_EXCEPTION("Unknown command occured!");
        break;
    }
}

// NEEDIMPROVE
inline void DrawAdditionData::bindDescriptosSets(DescriptorSetCreator &creator,
                                                 vk::CommandBuffer cmd,
                                                 vk::PipelineBindPoint bindPoint) {
    if (!creator.writes.empty()) {
        if (!descriptorSet)
            createDescriptorSet(creator);
    }
    if (descriptorSet)
        cmd.bindDescriptorSets(bindPoint, creator.layout, 0, 1, &descriptorSet, 0, nullptr);
}

inline void DrawAdditionData::createDescriptorSet(DescriptorSetCreator &creator) {
    using namespace vk;

    // Sets come from the manager-wide pool: creating a pool per draw meant a
    // vkCreateDescriptorPool for every command on every rebuild.
    DescriptorSetAllocateInfo allocInfo = {};
    allocInfo.descriptorPool = api_data.descriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &creator.descriptorSetLayout;

    auto res = api_data.device.allocateDescriptorSets(&allocInfo, &descriptorSet);
    THROW_VK_EX_IF_BAD(res);

    // Only the bindings the bound shader's layout declares may be written: the
    // write map accumulates every BindUniformBuffer/BindShadowMap of the whole
    // command buffer, and a set whose layout has no binding N rejects a write
    // to N (different shaders of one frame declare different bindings).
    vec<WriteDescriptorSet> writes;
    writes.reserve(creator.writes.size());
    for (auto &w : creator.writes) {
        if (!creator.declared.empty() && creator.declared.count(w.first) == 0)
            continue;
        auto write = w.second.write;
        write.dstSet = descriptorSet;
        if (write.descriptorType == DescriptorType::eCombinedImageSampler)
            write.pImageInfo = &w.second.image_info;
        else
            write.pBufferInfo = &w.second.buffer_info;
        writes.push_back(write);
    }
    api_data.device.updateDescriptorSets(writes.size(), writes.data(), 0, nullptr);
}

inline void DrawAdditionData::cleanup() {
    if (descriptorSet) {
        api_data.device.freeDescriptorSets(api_data.descriptorPool, descriptorSet);
        descriptorSet = nullptr;
    }
    descriptorPool = nullptr;
}

inline void CommandBuffer_API_data::end(uint i) { cmd[i].end(); }

inline void CommandBuffer_API_data::reset(uint i) { cmd[i].reset(); }

inline uint CommandBuffer_API_data::bufCount() { return cmd.size(); }

inline void CommandBuffer_API_data::cleanup(GraphicsManager_API_data &api_data) {
    api_data.device.free(api_data.commandPool, cmd);
}

NSP_GL_END
