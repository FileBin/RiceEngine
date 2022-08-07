/*
 * CommandBuffer.cpp
 *
 *  Created on: 27 April 2022
 *      Author: FileBinsLapTop
 */

#include "Rice/GL/IndexBuffer.hpp"
#include "pch.h"

#include <Rice/GL/CommandBuffer.hpp>

#include <Rice/GL/GraphicsComponentBase.hpp>
#include <sys/types.h>

#include "Vulkan_API_code/api_CommandBuffer_impl.inl"
#include "Vulkan_API_code/api_Shader.hpp"

NSP_GL_BEGIN

void _build(uint i, CommandBuffer_API_data *api_data,
            vec<ptr<CommandBuffer::Command>> &commands,
            GraphicsManager_API_data &data, vk::Extent2D window) {
    DescriptorSetCreator creator;
    data.sync();
    api_data->reset(i);
    api_data->begin(data, window, i);
    for (ptr<CommandBuffer::Command> cmd : commands)
        api_data->doCommand(cmd, i, data, creator);
    api_data->end(i);
}

CommandBuffer::CommandBuffer(ptr<GraphicsManager> g_mgr, bool begin_pass)
    : GraphicsComponentBase(g_mgr, false),
      api_data(new CommandBuffer_API_data) {
    api_data->build(get_api_data());

    api_data->begin_pass = begin_pass;

    uint n = api_data->bufCount();
    need_recreate.resize(n);

    g_mgr->resizeCommandBuffers->subscribe(resizeReg, [this](Vector2i size) {
        uint n = api_data->bufCount();
        need_recreate.resize(n);
        for (uint i = 0; i < n; ++i)
            _build(i, api_data.get(), commands, get_api_data(),
                   vk::Extent2D(size.x, size.y));
    });
    g_mgr->destroyCommandBuffers->subscribe(deleteReg, [this]() { cleanup(); });
}

void CommandBuffer::clear() {
    commands.clear();
    auto n = api_data->bufCount();
    for (uint i = 0; i < n; ++i)
        api_data->reset(i);
}

void CommandBuffer::needRecreate(bool v) {
    uint n = api_data->bufCount();
    for (uint i = 0; i < n; ++i)
        need_recreate[i] = v;
}

using pCmd = ptr<CommandBuffer::Command>;

void CommandBuffer::drawVertices(uint count) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::Draw, count, 1, 0, 0));
    // api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::drawIndexed(ptr<IndexBuffer> indexBuffer) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::DrawIndexed, indexBuffer));
    // api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::setActiveShader(ptr<Shader> shader) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::SetShader, shader));
    // api_data->doCommand({ Command::SetShader, shader->api_data->pipeline });
}

void CommandBuffer::bindVertexBuffer(ptr<Buffer> buffer) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::bindVertexBuffer(ptr<VertexBuffer> buffer) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::bindIndexBuffer(ptr<IndexBuffer> buffer) {
    needRecreate();
    commands.push_back(new_ptr<Command>(Command::BindIndexBuffer, buffer));
}

void CommandBuffer::bindUniformBuffer(ptr<UniformBuffer> uniformBuffer,
                                      uint binding) {
    needRecreate();
    commands.push_back(
        new_ptr<Command>(Command::BindUniformBuffer, uniformBuffer, binding));
}

void CommandBuffer::update() {
    GraphicsManager_API_data &d = get_api_data();
    if (need_recreate[d.swapchainImageIndex]) {
        build();
    }
}

void CommandBuffer::build() {
    GraphicsManager_API_data &d = get_api_data();
    _build(d.swapchainImageIndex, api_data.get(), commands, d, d.windowExcent);
    need_recreate[d.swapchainImageIndex] = false;
}

void CommandBuffer::buildAll() {
    GraphicsManager_API_data &d = get_api_data();
    uint n = api_data->bufCount();
    for (uint i = 0; i < n; ++i) {
        _build(i, api_data.get(), commands, d, d.windowExcent);
        need_recreate[i] = false;
    }
}

void CommandBuffer::cleanup() {
    if (!api_data)
        return;
    clear();
    GraphicsManager_API_data *data = &get_api_data();
    api_data->cleanup(*data);
    delete api_data.release();
}

CommandBuffer::~CommandBuffer() { cleanup(); }
NSP_GL_END
