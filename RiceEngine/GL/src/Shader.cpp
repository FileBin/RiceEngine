
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
    graphics_mgr->resizePipelines->subscribe(resizeReg, [this](Vector2i win) {
        onResize(win);
    }); // @suppress("Invalid arguments")
}
Shader::~Shader() { cleanup(); }

void Shader::build() {
    if (vertexStride == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex stride is not set!");
    if (vertexLayout.size() == 0)
        THROW_EXCEPTION("Cannot build pipeline! Vertex layout is not set!");

    Vector2i s = {get_api_data().windowExcent.width,
                  get_api_data().windowExcent.height};
    api_data->buildDescriptorSetLayout(get_api_data());
    api_data->buildPipeline(get_api_data(), s, vertexLayout, vertexStride);
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

void Shader::onResize(Vector2i extent) {
    api_data->cleanupPipeline(get_api_data());
    api_data->buildPipeline(get_api_data(), extent, vertexLayout, vertexStride);
}

NSP_GL_END
