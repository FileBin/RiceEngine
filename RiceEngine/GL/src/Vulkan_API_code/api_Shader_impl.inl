#pragma once
#include "Rice/GL/namespaces.h"
#include "Rice/Math/Vectors/Vector2i.hpp"
#include "VulkanException.hpp"
#include "api_GraphicsManager.hpp"
#include "api_Shader.hpp"
#include <vulkan/vulkan_enums.hpp>

NSP_GL_BEGIN

void Shader_API_Data::buildDescriptorSetLayout(
    GraphicsManager_API_data &api_data) {
    using namespace vk;

    constexpr auto maxCount = 0x10;

    vec<DescriptorPoolSize> sizes = {
        {DescriptorType::eUniformBuffer, maxCount}};

    DescriptorPoolCreateInfo pool_info = {};
    pool_info.maxSets = maxCount;
    pool_info.poolSizeCount = (uint32_t)sizes.size();
    pool_info.pPoolSizes = sizes.data();

    auto res = api_data.device.createDescriptorPool(&pool_info, nullptr, &descriptorPool);
    THROW_VK_EX_IF_BAD(res);

    if (bindings.size() == 0)
        THROW_EXCEPTION(
            "Cannot build descriptor set layout! Bindings are not set!");
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();
    descriptorSetLayout = api_data.device.createDescriptorSetLayout(layoutInfo);

    DescriptorSetAllocateInfo allocInfo = {};
    // using the pool we just set
    allocInfo.descriptorPool = descriptorPool;
    // only 1 descriptor
    allocInfo.descriptorSetCount = 1;
    // using the global data layout
    allocInfo.pSetLayouts = &descriptorSetLayout;

    res = api_data.device.allocateDescriptorSets(&allocInfo, &descriptorSet);
    THROW_VK_EX_IF_BAD(res);
}

void Shader_API_Data::buildPipeline(GraphicsManager_API_data &api_data,
                                    Vector2i windowExcent,
                                    VertexLayout &vertexLayout,
                                    uint vertexStride) {
    using help = VulkanHelper;
    using namespace vk;

    vk::PipelineLayoutCreateInfo pipeline_layout_info;

    PushConstantRange range;
    range.setOffset(0).setSize(128).setStageFlags(ShaderStageFlagBits::eVertex);

    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &range;

    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &descriptorSetLayout;

    layout = api_data.device.createPipelineLayout(pipeline_layout_info);

    // build the stage-create-info for both vertex and fragment stages. This
    // lets the pipeline know the shader modules per stage
    help::PipelineBuilder pipelineBuilder;

    if (vertexShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eVertex, vertexShader));

    if (fragmentShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eFragment, fragmentShader));

    if (geometryShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eGeometry, geometryShader));

    VertexInputBindingDescription bindingDesc =
        VertexInputBindingDescription()
            .setBinding(0)
            .setStride(vertexStride)
            .setInputRate(VertexInputRate::eVertex);

    uint n = vertexLayout.size();

    vec<VertexInputAttributeDescription> attributeDescs(n);

    for (uint i = 0; i < n; ++i) {
        auto &input = vertexLayout[i];
        auto &desc = attributeDescs[i];
        desc.setBinding(input.binding).setOffset(input.offset).setLocation(i);

        switch (input.format) {

        case VertexInput::float1:
            desc.setFormat(Format::eR32Sfloat);
            break;
        case VertexInput::float2:
            desc.setFormat(Format::eR32G32Sfloat);
            break;
        case VertexInput::float3:
            desc.setFormat(Format::eR32G32B32Sfloat);
            break;
        case VertexInput::float4:
            desc.setFormat(Format::eR32G32B32A32Sfloat);
            break;

        case VertexInput::int1:
            desc.setFormat(Format::eR32Sint);
            break;
        case VertexInput::int2:
            desc.setFormat(Format::eR32G32Sint);
            break;
        case VertexInput::int3:
            desc.setFormat(Format::eR32G32B32Sint);
            break;
        case VertexInput::int4:
            desc.setFormat(Format::eR32G32B32A32Sint);
            break;

        default:
            THROW_EXCEPTION("Unknown vertex format!");
            break;
        }
    }

    // vertex input controls how to read vertices from vertex buffers. We aren't
    // using it yet
    pipelineBuilder.vk_vertexInputInfo =
        PipelineVertexInputStateCreateInfo()
            .setVertexBindingDescriptionCount(1)
            .setPVertexBindingDescriptions(&bindingDesc)
            .setVertexAttributeDescriptionCount(n)
            .setPVertexAttributeDescriptions(attributeDescs.data());

    // input assembly is the configuration for drawing triangle lists, strips,
    // or individual points. we are just going to draw triangle list
    pipelineBuilder.vk_inputAssembly =
        PipelineInputAssemblyStateCreateInfo().setTopology(
            PrimitiveTopology::eTriangleList);

    // build viewport and scissor from the swapchain extents
    pipelineBuilder.vk_viewport.x = 0.0f;
    pipelineBuilder.vk_viewport.y = 0.0f;
    pipelineBuilder.vk_viewport.width = (float)windowExcent.x;
    pipelineBuilder.vk_viewport.height = (float)windowExcent.y;
    pipelineBuilder.vk_viewport.minDepth = 0.0f;
    pipelineBuilder.vk_viewport.maxDepth = 1.0f;

    pipelineBuilder.vk_scissor.offset = vk::Offset2D(0, 0);
    pipelineBuilder.vk_scissor.extent =
        vk::Extent2D(windowExcent.x, windowExcent.y);

    // configure the rasterizer to draw filled triangles
    pipelineBuilder.vk_rasterizer = PipelineRasterizationStateCreateInfo()
                                        .setLineWidth(1)
                                        .setCullMode(CullModeFlagBits::eBack)
                                        .setFrontFace(FrontFace::eClockwise)
                                        .setPolygonMode(PolygonMode::eFill)
                                        .setDepthClampEnable(false)
                                        .setDepthBiasEnable(false);

    // we don't use multisampling, so just run the default one
    pipelineBuilder.vk_multisampling =
        PipelineMultisampleStateCreateInfo().setRasterizationSamples(
            SampleCountFlagBits::e1);

    // a single blend attachment with no blending and writing to RGBA
    pipelineBuilder.vk_colorBlendAttachment =
        PipelineColorBlendAttachmentState()
            .setColorWriteMask(
                ColorComponentFlagBits::eR | ColorComponentFlagBits::eG |
                ColorComponentFlagBits::eB | ColorComponentFlagBits::eA)
            .setBlendEnable(false);

    pipelineBuilder.vk_depthStencil = PipelineDepthStencilStateCreateInfo()
                                          .setDepthTestEnable(true)
                                          .setDepthWriteEnable(true)
                                          .setDepthCompareOp(CompareOp::eLess);

    // use the triangle layout we created
    pipelineBuilder.vk_pipelineLayout = layout;

    // finally build the pipeline
    pipeline = pipelineBuilder.build_pipeline(api_data.device,
                                              api_data.def_renderPass);
}

void Shader_API_Data::cleanupPipeline(GraphicsManager_API_data &api_data) {
    if (pipeline)
        api_data.device.destroy(pipeline);

    if (layout)
        api_data.device.destroy(layout);
}
void Shader_API_Data::cleanup(GraphicsManager_API_data &api_data) {
    if (vertexShader)
        api_data.device.destroy(vertexShader);
    if (fragmentShader)
        api_data.device.destroy(fragmentShader);
    if (geometryShader)
        api_data.device.destroy(geometryShader);

    if (descriptorSetLayout)
        api_data.device.destroy(descriptorSetLayout);
}

NSP_GL_END
