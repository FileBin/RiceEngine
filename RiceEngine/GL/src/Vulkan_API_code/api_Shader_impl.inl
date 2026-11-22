#pragma once
#include "Rice/GL/namespaces.h"
#include "VulkanException.hpp"
#include "api_GraphicsManager.hpp"
#include "api_Shader.hpp"
#include <Rice/Math/Vectors.hpp>
#include <vulkan/vulkan_enums.hpp>

NSP_GL_BEGIN

/* Vertex input state shared by the scene pipeline and the depth-only shadow
 * pipeline: binding 0, stride = vertexStride, one attribute per VertexLayout
 * entry with location = declaration order. */
static vk::PipelineVertexInputStateCreateInfo
make_vertex_input_state(VertexLayout &vertexLayout, uint vertexStride,
                        vk::VertexInputBindingDescription &bindingDesc,
                        vec<vk::VertexInputAttributeDescription> &attributeDescs) {
    using namespace vk;

    bindingDesc = VertexInputBindingDescription()
                      .setBinding(0)
                      .setStride(vertexStride)
                      .setInputRate(VertexInputRate::eVertex);

    uint n = vertexLayout.size();
    attributeDescs.resize(n);

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

    return PipelineVertexInputStateCreateInfo()
        .setVertexBindingDescriptionCount(1)
        .setPVertexBindingDescriptions(&bindingDesc)
        .setVertexAttributeDescriptionCount(n)
        .setPVertexAttributeDescriptions(attributeDescs.data());
}

void Shader_API_Data::buildDescriptorSetLayout(GraphicsManager_API_data &api_data) {
    using namespace vk;

    if (bindings.size() == 0)
        THROW_EXCEPTION("Cannot build descriptor set layout! Bindings are not set!");
    vk::DescriptorSetLayoutCreateInfo layoutInfo;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data();
    descriptorSetLayout = api_data.device.createDescriptorSetLayout(layoutInfo);
}

void Shader_API_Data::buildPipeline(GraphicsManager_API_data &api_data, Vector2i windowExcent,
                                    VertexLayout &vertexLayout, uint vertexStride,
                                    bool blendEnable, bool lineTopology, bool depthTest) {
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
        pipelineBuilder.vk_shaderStages.push_back(help::pipeline_shader_stage_create_info(
            vk::ShaderStageFlagBits::eVertex, vertexShader));

    if (fragmentShader)
        pipelineBuilder.vk_shaderStages.push_back(help::pipeline_shader_stage_create_info(
            vk::ShaderStageFlagBits::eFragment, fragmentShader));

    if (geometryShader)
        pipelineBuilder.vk_shaderStages.push_back(help::pipeline_shader_stage_create_info(
            vk::ShaderStageFlagBits::eGeometry, geometryShader));

    VertexInputBindingDescription bindingDesc;
    vec<VertexInputAttributeDescription> attributeDescs;

    pipelineBuilder.vk_vertexInputInfo =
        make_vertex_input_state(vertexLayout, vertexStride, bindingDesc, attributeDescs);

    // input assembly is the configuration for drawing triangle lists, strips,
    // or individual points. we are just going to draw triangle list
    pipelineBuilder.vk_inputAssembly = PipelineInputAssemblyStateCreateInfo().setTopology(
        lineTopology ? PrimitiveTopology::eLineList : PrimitiveTopology::eTriangleList);

    // build viewport and scissor from the swapchain extents
    pipelineBuilder.vk_viewport.x = 0.0f;
    pipelineBuilder.vk_viewport.y = 0.0f;
    pipelineBuilder.vk_viewport.width = (float)windowExcent.x;
    pipelineBuilder.vk_viewport.height = (float)windowExcent.y;
    pipelineBuilder.vk_viewport.minDepth = 0.0f;
    pipelineBuilder.vk_viewport.maxDepth = 1.0f;

    pipelineBuilder.vk_scissor.offset = vk::Offset2D(0, 0);
    pipelineBuilder.vk_scissor.extent = vk::Extent2D(windowExcent.x, windowExcent.y);

    // configure the rasterizer to draw filled triangles
    // Overlay wireframes have no front face and no depth relationship with the scene,
    // so they are drawn with culling off.
    pipelineBuilder.vk_rasterizer = PipelineRasterizationStateCreateInfo()
                                        .setLineWidth(1)
                                        .setCullMode((lineTopology || !depthTest)
                                                         ? CullModeFlagBits::eNone
                                                         : CullModeFlagBits::eBack)
                                        .setFrontFace(FrontFace::eClockwise)
                                        .setPolygonMode(PolygonMode::eFill)
                                        .setDepthClampEnable(false)
                                        .setDepthBiasEnable(false);

    // we don't use multisampling, so just run the default one
    pipelineBuilder.vk_multisampling =
        PipelineMultisampleStateCreateInfo().setRasterizationSamples(SampleCountFlagBits::e1);

    // A single blend attachment. Opaque by default, exactly as before;
    // Shader::setBlendEnabled(true) switches it to straight (non-premultiplied)
    // alpha: the fragment shader returns an opaque colour with alpha as the
    // coverage, which is what the old water shader did.
    vk::PipelineColorBlendAttachmentState colorBlendAttachment =
        PipelineColorBlendAttachmentState()
            .setColorWriteMask(ColorComponentFlagBits::eR | ColorComponentFlagBits::eG |
                               ColorComponentFlagBits::eB | ColorComponentFlagBits::eA)
            .setBlendEnable(blendEnable);

    if (blendEnable) {
        colorBlendAttachment
            .setSrcColorBlendFactor(BlendFactor::eSrcAlpha)
            .setDstColorBlendFactor(BlendFactor::eOneMinusSrcAlpha)
            .setColorBlendOp(BlendOp::eAdd)
            .setSrcAlphaBlendFactor(BlendFactor::eOne)
            .setDstAlphaBlendFactor(BlendFactor::eZero)
            .setAlphaBlendOp(BlendOp::eAdd);
    }
    pipelineBuilder.vk_colorBlendAttachment = colorBlendAttachment;

    // Depth test stays on (blended geometry is still hidden by opaque things in
    // front of it) but depth writes are off, so a transparent surface never
    // occludes anything drawn after it.
    pipelineBuilder.vk_depthStencil = PipelineDepthStencilStateCreateInfo()
                                          .setDepthTestEnable(depthTest)
                                          .setDepthWriteEnable(depthTest && !blendEnable)
                                          .setDepthCompareOp(CompareOp::eLess);

    // use the triangle layout we created
    pipelineBuilder.vk_pipelineLayout = layout;

    // finally build the pipeline
    pipeline = pipelineBuilder.build_pipeline(api_data.device, api_data.def_renderPass);
}

void Shader_API_Data::buildDepthOnlyPipeline(GraphicsManager_API_data &api_data,
                                            vk::RenderPass pass, Vector2i extent,
                                            VertexLayout &vertexLayout, uint vertexStride) {
    using namespace vk;

    if (!vertexShader)
        THROW_EXCEPTION("Cannot build a shadow pipeline without a vertex shader!");

    // Both descriptors are required: the casters pass reads the per-mesh
    // ModelData (binding 0) and the scene LightData (binding 2), both declared
    // through addUniformBuffer() before this call. No push constants.
    PipelineLayoutCreateInfo plInfo{};
    plInfo.setLayoutCount = 1;
    plInfo.pSetLayouts = &descriptorSetLayout;
    layout = api_data.device.createPipelineLayout(plInfo);

    // Vertex stage only: no fragment work at all. The pass writes depth, which
    // is the whole point, and a pipeline without a fragment stage is legal.
    PipelineShaderStageCreateInfo stage = VulkanHelper::pipeline_shader_stage_create_info(
        ShaderStageFlagBits::eVertex, vertexShader);

    VertexInputBindingDescription bindingDesc;
    vec<VertexInputAttributeDescription> attributeDescs;
    PipelineVertexInputStateCreateInfo vertexInput =
        make_vertex_input_state(vertexLayout, vertexStride, bindingDesc, attributeDescs);

    PipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.topology = PrimitiveTopology::eTriangleList;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // The viewport is the shadow map, not the window: this pipeline is never
    // used for the swapchain.
    Viewport viewport{};
    viewport.x = 0.f;
    viewport.y = 0.f;
    viewport.width = (float)extent.x;
    viewport.height = (float)extent.y;
    viewport.minDepth = 0.f;
    viewport.maxDepth = 1.f;

    Rect2D scissor{};
    scissor.offset = Offset2D(0, 0);
    scissor.extent = Extent2D(extent.x, extent.y);

    PipelineViewportStateCreateInfo viewportState{};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Both faces are casters: a closed box must not punch a hole in the map
    // because of the winding it happens to have.
    PipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.polygonMode = PolygonMode::eFill;
    rasterizer.cullMode = CullModeFlagBits::eNone;
    rasterizer.frontFace = FrontFace::eClockwise;
    rasterizer.lineWidth = 1.f;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.depthBiasEnable = VK_FALSE;

    PipelineMultisampleStateCreateInfo multisampling{};
    multisampling.rasterizationSamples = SampleCountFlagBits::e1;
    multisampling.sampleShadingEnable = VK_FALSE;

    // The depth state the scene pipeline does NOT set explicitly (it relies on
    // the defaults, i.e. depth testing off); here it is the entire purpose.
    PipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = CompareOp::eLess;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    // The shadow render pass has no colour attachment, so there is nothing to
    // blend into.
    PipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 0;
    colorBlending.pAttachments = nullptr;

    GraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.stageCount = 1;
    pipelineInfo.pStages = &stage;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = layout;
    pipelineInfo.renderPass = pass;
    pipelineInfo.subpass = 0;

    vk::Pipeline newPipeline;
    auto res = api_data.device.createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr,
                                                       &newPipeline);
    THROW_VK_EX_IF_BAD(res);
    pipeline = newPipeline;
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

    cleanupPipeline(api_data);
}

NSP_GL_END
