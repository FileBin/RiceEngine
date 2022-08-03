
#include "pch.h"
#include <Rice/GL/GraphicsComponentBase.hpp>
#include <Rice/GL/Shader.hpp>

#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_Shader.hpp"

#include <Rice/GL/Mesh.hpp>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

Shader::Shader(ptr<GraphicsManager> g_mgr)
    : GraphicsComponentBase(g_mgr), api_data{new Shader_API_Data()} {
    graphics_mgr->resizePipelines->subscribe(resizeReg, [this](Vector2i win) {
        onResize(win);
    }); // @suppress("Invalid arguments")
}
Shader::~Shader() { cleanup(); }

void Shader::build() {
    Vector2i s = {get_api_data().windowExcent.width,
                  get_api_data().windowExcent.height};
    buildDescriptorSet();
    buildPipeline(s);
}

void Shader::cleanup() {
    if (init) {
        cleanupShaders();
        cleanupPipeline();
        cleanupDescriptorSetLayout();
        delete api_data.release();
    }
    init = false;
}

void Shader::cleanupShaders() {
    if (api_data->vertexShader)
        get_api_data().device.destroy(api_data->vertexShader);
    if (api_data->fragmentShader)
        get_api_data().device.destroy(api_data->fragmentShader);
    if (api_data->geometryShader)
        get_api_data().device.destroy(api_data->geometryShader);
}

void Shader::cleanupPipeline() {
    if (api_data->pipeline)
        get_api_data().device.destroy(api_data->pipeline);
    if (api_data->layout)
        get_api_data().device.destroy(api_data->layout);
}

void Shader::cleanupDescriptorSetLayout() {
    if (api_data->descriptorSetLayout)
        get_api_data().device.destroy(api_data->descriptorSetLayout);
}

void Shader::loadShader(String path, Type type) {
    data_t buffer = Util::readFile(path.toUTF8String());

    // create a new shader module, using the buffer we loaded
    vk::ShaderModuleCreateInfo createInfo = {};

    // codeSize has to be in bytes, so multiply the ints in the buffer by size
    // of int to know the real size of the buffer
    createInfo.codeSize = buffer.size();
    createInfo.pCode = (uint *)buffer.data();

    // check that the creation goes well.
    vk::ShaderModule shaderModule =
        get_api_data().device.createShaderModule(createInfo);

    switch (type) {
    case Type::Vertex:
        api_data->vertexShader = shaderModule;
        break;
    case Type::Fragment:
        api_data->fragmentShader = shaderModule;
        break;
    case Type::Geometry:
        api_data->geometryShader = shaderModule;
    }
}

void Shader::addUniformBuffer(uint binding, Type stage) {
    using namespace vk;
    DescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;

    switch (stage) {
    case Type::Vertex:
        uboLayoutBinding.stageFlags = ShaderStageFlagBits::eVertex;
        break;
    case Type::Fragment:
        uboLayoutBinding.stageFlags = ShaderStageFlagBits::eFragment;
        break;
    case Type::Geometry:
        uboLayoutBinding.stageFlags = ShaderStageFlagBits::eGeometry;
        break;
    }

    api_data->bindings.push_back(uboLayoutBinding);
}

void Shader::buildDescriptorSet() {
    using namespace vk;
    DescriptorSetLayoutCreateInfo info;
    info.bindingCount = api_data->bindings.size();
    info.pBindings = api_data->bindings.data();

    api_data->descriptorSetLayout =
        get_api_data().device.createDescriptorSetLayout(info);
}

void Shader::buildPipeline(Vector2i extent) {
    if (vertexStride == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex stride is not set!");
    if (vertexLayout.size() == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex layout is not set!");

    using help = VulkanHelper;
    using namespace vk;

    vk::PipelineLayoutCreateInfo pipeline_layout_info;

    PushConstantRange range;
    range.setOffset(0).setSize(128).setStageFlags(ShaderStageFlagBits::eVertex);

    pipeline_layout_info.pushConstantRangeCount = 1;
    pipeline_layout_info.pPushConstantRanges = &range;

    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &api_data->descriptorSetLayout;

    api_data->layout =
        get_api_data().device.createPipelineLayout(pipeline_layout_info);

    // build the stage-create-info for both vertex and fragment stages. This
    // lets the pipeline know the shader modules per stage
    help::PipelineBuilder pipelineBuilder;

    if (api_data->vertexShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eVertex, api_data->vertexShader));

    if (api_data->fragmentShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eFragment, api_data->fragmentShader));

    if (api_data->geometryShader)
        pipelineBuilder.vk_shaderStages.push_back(
            help::pipeline_shader_stage_create_info(
                vk::ShaderStageFlagBits::eGeometry, api_data->geometryShader));

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
    pipelineBuilder.vk_viewport.width = (float)extent.x;
    pipelineBuilder.vk_viewport.height = (float)extent.y;
    pipelineBuilder.vk_viewport.minDepth = 0.0f;
    pipelineBuilder.vk_viewport.maxDepth = 1.0f;

    pipelineBuilder.vk_scissor.offset = vk::Offset2D(0, 0);
    pipelineBuilder.vk_scissor.extent = vk::Extent2D(extent.x, extent.y);

    // configure the rasterizer to draw filled triangles
    pipelineBuilder.vk_rasterizer =
        PipelineRasterizationStateCreateInfo()
            .setLineWidth(1)
            .setCullMode(CullModeFlagBits::eBack)
            .setFrontFace(FrontFace::eClockwise)
            .setPolygonMode(PolygonMode::eFill)
            .setDepthClampEnable(false);

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

    // use the triangle layout we created
    pipelineBuilder.vk_pipelineLayout = api_data->layout;

    // finally build the pipeline
    api_data->pipeline = pipelineBuilder.build_pipeline(
        get_api_data().device, get_api_data().def_renderPass);
    init = true;
}

void Shader::onResize(Vector2i extent) {
    cleanupPipeline();
    buildPipeline(extent);
}

NSP_GL_END
