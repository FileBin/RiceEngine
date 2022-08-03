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

#include "Vulkan_API_code/api_CommandBuffer_impl.inl"
#include  "Vulkan_API_code/api_Shader.hpp"

NSP_GL_BEGIN

void _build(uint i, CommandBuffer_API_data* api_data, vec<ptr<CommandBuffer::Command>>& commands, GraphicsManager_API_data& data, vk::Extent2D window) {
	data.sync();
	api_data->reset(i);
	api_data->begin(data, window, i);
	for(ptr<CommandBuffer::Command> cmd : commands)
		api_data->doCommand(cmd, i);
	api_data->end(i);
}

CommandBuffer::CommandBuffer(ptr<GraphicsManager> g_mgr) : GraphicsComponentBase(g_mgr), api_data(new CommandBuffer_API_data) {
	api_data->build(get_api_data());

	g_mgr->resizeCommandBuffers->subscribe(resizeReg, [this](Vector2i size) { // @suppress("Invalid arguments")
		uint n = api_data->bufCount();
		for (uint i = 0; i < n; ++i)
			_build(i, api_data.get(), commands, get_api_data(), vk::Extent2D(size.x, size.y));
	});
}

void CommandBuffer::clear() {
	commands.clear();
	auto n = api_data->bufCount();
	for (uint i = 0; i < n; ++i)
		api_data->reset(i);
}

using pCmd = ptr<CommandBuffer::Command>;

void CommandBuffer::drawVertices(uint count) {
	commands.push_back(new_ptr<Command>(Command::Draw, count, 1, 0, 0));
	//api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::drawIndexed(ptr<IndexBuffer> indexBuffer) {

	commands.push_back(new_ptr<Command>(Command::DrawIndexed, indexBuffer));
	//api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::setActiveShader(ptr<Shader> shader) {
	commands.push_back(new_ptr<Command>(Command::SetShader, shader));
	//api_data->doCommand({ Command::SetShader, shader->api_data->pipeline });
}

void CommandBuffer::bindVertexBuffer(ptr<Buffer> buffer) {
	commands.push_back(new_ptr<Command>(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::bindVertexBuffer(ptr<VertexBuffer> buffer) {
	commands.push_back(new_ptr<Command>(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::bindIndexBuffer(ptr<IndexBuffer> buffer) {
	commands.push_back(new_ptr<Command>(Command::BindIndexBuffer, buffer));
}

void CommandBuffer::bindUniformBuffer(ptr<UniformBuffer> buffer) {
	commands.push_back(new_ptr<Command>(Command::BindUniformBuffer, buffer));
}

void CommandBuffer::build() {
	GraphicsManager_API_data& d = get_api_data();
	_build(d.swapchainImageIndex, api_data.get(), commands, d, d.windowExcent);
}

void CommandBuffer::buildAll() {
	GraphicsManager_API_data& d = get_api_data();
	uint n = api_data->bufCount();
	for (uint i = 0; i < n; ++i)
		_build(i, api_data.get(), commands, d, d.windowExcent);
}

void CommandBuffer::cleanup() {
    if(!api_data) return;
	clear();
	GraphicsManager_API_data* data = &get_api_data();
	api_data->cleanup(*data);
	delete api_data.release();
}

CommandBuffer::~CommandBuffer() {
    cleanup();
}
NSP_GL_END




