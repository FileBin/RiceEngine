
#include "pch.h"
#include <Rice/GL/GraphicsComponentBase.hpp>
#include <Rice/GL/Shader.hpp>

#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_Shader_impl.inl"

#include <Rice/GL/Mesh.hpp>
#include <cstdint>
#include <sys/types.h>
#include <vulkan/vulkan_structs.hpp>

NSP_GL_BEGIN

Shader::Shader(ptr<GraphicsManager> g_mgr)
    : GraphicsComponentBase(g_mgr), api_data{new Shader_API_Data()} {
    graphics_mgr->resizePipelines->subscribe(
        resizeReg, [this](Vector2i win) { onResize(win); }); // @suppress("Invalid arguments")
}
Shader::~Shader() { cleanup(); }

void Shader::build() {
    if (vertexStride == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex stride is not set!");
    if (vertexLayout.size() == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex layout is not set!");

    Vector2i s = {get_api_data().windowExcent.width, get_api_data().windowExcent.height};
    api_data->buildDescriptorSetLayout(get_api_data());
    api_data->buildPipeline(get_api_data(), s, vertexLayout, vertexStride, blend_enabled,
                            topology == Topology::Lines, depth_test_enabled,
                            wireframe);
    init = true;
}

void Shader::cleanup() {
    if (init) {
        api_data->cleanup(get_api_data());
        delete api_data.release();
    }
    init = false;
}

void Shader::loadShader(String path, Type type) {
    data_t buffer = Util::readFile(path);

    // create a new shader module, using the buffer we loaded
    vk::ShaderModuleCreateInfo createInfo = {};

    // codeSize has to be in bytes, so multiply the ints in the buffer by size
    // of int to know the real size of the buffer
    createInfo.codeSize = buffer.size();
    createInfo.pCode = (uint *)buffer.data();

    // check that the creation goes well.
    vk::ShaderModule shaderModule = get_api_data().device.createShaderModule(createInfo);

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
    uboLayoutBinding.binding = binding;
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

void Shader::addUniformBuffer(uint binding, Type firstStage, Type secondStage) {
    using namespace vk;

    // One descriptor set layout binding covers all stages, so the two stages are
    // merged into a single vk::ShaderStageFlags instead of two bindings.
    constexpr auto stageFlag = [](Type stage) {
        switch (stage) {
        case Type::Vertex:
            return ShaderStageFlagBits::eVertex;
        case Type::Fragment:
            return ShaderStageFlagBits::eFragment;
        default:
            return ShaderStageFlagBits::eGeometry;
        }
    };

    DescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = binding;
    uboLayoutBinding.descriptorType = DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = stageFlag(firstStage) | stageFlag(secondStage);

    api_data->bindings.push_back(uboLayoutBinding);
}

void Shader::addLighting(uint light_binding, uint shadow_binding) {
    using namespace vk;

    // The scene light block is read by BOTH stages: the vertex shader needs
    // light_wvp to compute the light-space position, the fragment shader the
    // ambient / diffuse / direction terms.
    DescriptorSetLayoutBinding light;
    light.binding = light_binding;
    light.descriptorType = DescriptorType::eUniformBuffer;
    light.descriptorCount = 1;
    light.stageFlags = ShaderStageFlagBits::eVertex | ShaderStageFlagBits::eFragment;
    api_data->bindings.push_back(light);

    // The shadow map itself is only sampled by the fragment stage.
    DescriptorSetLayoutBinding shadow;
    shadow.binding = shadow_binding;
    shadow.descriptorType = DescriptorType::eCombinedImageSampler;
    shadow.descriptorCount = 1;
    shadow.stageFlags = ShaderStageFlagBits::eFragment;
    api_data->bindings.push_back(shadow);

    lighting = true;
    this->light_binding = light_binding;
    this->shadow_binding = shadow_binding;
}

void Shader::buildShadowPipeline(uint map_size) {
    if (vertexStride == 0)
        THROW_EXCEPTION("Cannot build shadow pipeline! Vertex stride is not set!");
    if (vertexLayout.size() == 0)
        THROW_EXCEPTION("Cannot build shadow pipeline! Vertex layout is not set!");
    if (api_data->bindings.size() == 0)
        THROW_EXCEPTION("Cannot build shadow pipeline! Bindings are not set!");

    api_data->buildDescriptorSetLayout(get_api_data());
    api_data->buildDepthOnlyPipeline(get_api_data(), get_api_data().shadow_renderPass,
                                     {(int)map_size, (int)map_size}, vertexLayout, vertexStride);
    shadow_pipeline = true;
    init = true;
}

void Shader::onResize(Vector2i extent) {
    // A depth-only (shadow) pipeline is not tied to the swapchain extent.
    if (shadow_pipeline)
        return;

    api_data->cleanupPipeline(get_api_data());
    api_data->buildPipeline(get_api_data(), extent, vertexLayout, vertexStride, blend_enabled,
                            topology == Topology::Lines, depth_test_enabled,
                            wireframe);
}

NSP_GL_END
