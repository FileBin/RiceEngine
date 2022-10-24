/*
 * VulkanHelper.hpp
 *
 *  Created on: 2 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#include <Rice/stdafx.hpp>

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include "VulkanException.hpp"

#pragma once
NSP_GL_BEGIN

#define VK_SEC *1000000000ull

#define TO_VK_BOOL(b) ((b) ? VK_TRUE : VK_FALSE)

class VulkanHelper {
  public:
    class PipelineBuilder {
      public:
        vec<vk::PipelineShaderStageCreateInfo> vk_shaderStages;
        vk::PipelineVertexInputStateCreateInfo vk_vertexInputInfo;
        vk::PipelineInputAssemblyStateCreateInfo vk_inputAssembly;
        vk::Viewport vk_viewport;
        vk::Rect2D vk_scissor;
        vk::PipelineRasterizationStateCreateInfo vk_rasterizer;
        vk::PipelineColorBlendAttachmentState vk_colorBlendAttachment;
        vk::PipelineMultisampleStateCreateInfo vk_multisampling;
        vk::PipelineLayout vk_pipelineLayout;
        vk::PipelineDepthStencilStateCreateInfo vk_depthStencil;

        vk::Pipeline build_pipeline(vk::Device device, vk::RenderPass pass);
    };

    static vk::CommandPoolCreateInfo command_pool_create_info(uint queueFamilyIndex, vk::CommandPoolCreateFlags flags);

    static vk::CommandBufferAllocateInfo
    command_buffer_allocate_info(vk::CommandPool pool, uint count = 1,
                                 vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

    static vk::PipelineShaderStageCreateInfo pipeline_shader_stage_create_info(vk::ShaderStageFlagBits stage_flags,
                                                                               vk::ShaderModule shaderModule,
                                                                               const char *entrypoint = "main");

    static vk::PipelineVertexInputStateCreateInfo
    vertex_input_state_create_info(vec<vk::VertexInputBindingDescription> *bindings = nullptr,
                                   vec<vk::VertexInputAttributeDescription> *attributes = nullptr);

    static vk::PipelineInputAssemblyStateCreateInfo
    input_assembly_create_info(vk::PrimitiveTopology topology = vk::PrimitiveTopology::eTriangleList);

    static vk::PipelineRasterizationStateCreateInfo
    rasterization_state_create_info(vk::PolygonMode polygonMode = vk::PolygonMode::eFill, float line_width = 1.f,
                                    vk::CullModeFlags cull_mode_flags = vk::CullModeFlagBits::eNone,
                                    vk::FrontFace front_face = vk::FrontFace::eClockwise);

    static vk::PipelineMultisampleStateCreateInfo
    multisampling_state_create_info(vk::SampleCountFlagBits nSamples = vk::SampleCountFlagBits::e1);

    static vk::PipelineColorBlendAttachmentState color_blend_attachment_state(
        vk::ColorComponentFlags colorMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
        bool blendEnable = false);
};

NSP_GL_END
