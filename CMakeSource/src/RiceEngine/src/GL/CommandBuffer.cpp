/*
 * CommandBuffer.cpp
 *
 *  Created on: 27 April 2022
 *      Author: FileBinsLapTop
 */

#include "pch.h"

#include <Rice/GL/CommandBuffer.hpp>

#include <Rice/GL/GraphicsComponentBase.hpp>

#include "Vulkan_API_code/api_CommandBuffer_impl.hpp"
#include  "Vulkan_API_code/api_Shader.hpp"

NSP_GL_BEGIN

void _build(CommandBuffer_API_data* api_data, vec<SmartPtr<CommandBuffer::Command>>& commands, GraphicsManager_API_data& data, vk::Extent2D window) {
	api_data->reset();
	api_data->begin(data, window);
	for(auto& cmd : commands) {
		api_data->doCommand(cmd);
	}
	api_data->end();
}

CommandBuffer::CommandBuffer(pGraphicsManager g_mgr) : GraphicsComponentBase(g_mgr) {
	api_data = new CommandBuffer_API_data;
	api_data->build(get_api_data());

	g_mgr->resizeCommandBuffers.subscribe([this](Vector2i size) { // @suppress("Invalid arguments")
		_build(api_data.get(), commands, get_api_data(), vk::Extent2D(size.x, size.y));
	});
}

void CommandBuffer::reset() {
	uint n = commands.size();
	for (uint i = 0; i < n; ++i) {
		commands[i].release();
	}

	commands.clear();
	api_data->reset();
}

void CommandBuffer::drawVertices(uint count) {

	commands.push_back(new Command(Command::Draw, count, 1, 0, 0));
	//api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::setActiveShader(pShader shader) {
	commands.push_back(new Command(Command::SetShader, shader));
	//api_data->doCommand({ Command::SetShader, shader->api_data->pipeline });
}

void CommandBuffer::bindVertexBuffer(pBuffer buffer) {
	commands.push_back(new Command(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::build() {
	_build(api_data.get(), commands, get_api_data(), get_api_data().windowExcent);
}

void CommandBuffer::cleanup() {
	reset();
	api_data->cleanup(get_api_data());
	api_data.release();
}
NSP_GL_END




