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

void _build(CommandBuffer_API_data* api_data, List<AutoPtr<CommandBuffer::Command>>& commands, GraphicsManager_API_data& data, vk::Extent2D window) {
	api_data->reset();
	api_data->begin(data, window);
	for(AutoPtr<IEnumeratorT<AutoPtr<CommandBuffer::Command>>> en = commands.getEnumeratorT();
			en->next(); ) {
		api_data->doCommand(
				en->currentT()
				);
	}
	//for(auto& cmd : commands) {
	//	api_data->doCommand(cmd);
	//}
	api_data->end();
}

CommandBuffer::CommandBuffer(pGraphicsManager g_mgr) : GraphicsComponentBase(g_mgr), api_data(new CommandBuffer_API_data) {
	api_data->build(get_api_data());

	g_mgr->resizeCommandBuffers.subscribe(resizeReg, [this](Vector2i size) { // @suppress("Invalid arguments")
		_build(api_data.get(), commands, get_api_data(), vk::Extent2D(size.x, size.y));
	});
}

void CommandBuffer::reset() {
	uint n = commands.length();
	for (uint i = 0; i < n; ++i) {
		commands[i].release();
	}

	commands.clear();
	api_data->reset();
}

using pCmd = RefPtr<CommandBuffer::Command>;

void CommandBuffer::drawVertices(uint count) {

	commands.push(new_ref<Command>(Command::Draw, count, 1, 0, 0));
	//api_data->doCommand({ Command::Draw, count, 1, 0, 0 });
}

void CommandBuffer::setActiveShader(pShader shader) {
	commands.push(new_ref<Command>(Command::SetShader, shader));
	//api_data->doCommand({ Command::SetShader, shader->api_data->pipeline });
}

void CommandBuffer::bindVertexBuffer(pBuffer buffer) {
	commands.push(new_ref<Command>(Command::BindVertexBuffer, buffer));
}

void CommandBuffer::bindVertexBuffer(pVertexBuffer buffer) {
	commands.push(new_ref<Command>(Command::BindVertexBuffer, (pBuffer)buffer));
}

void CommandBuffer::bindIndexBuffer(pBuffer buffer, IndexBufferType type) {
	commands.push(new_ref<Command>(Command::BindIndexBuffer, buffer, type));
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




