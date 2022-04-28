/*
 * api_CommandBuffer_impl.hpp
 *
 *  Created on: 27 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "api_CommandBuffer.hpp"
#include "api_Shader.hpp"
#include "api_Buffer.hpp"

NSP_GL_BEGIN

void CommandBuffer_API_data::build(GraphicsManager_API_data& api_data) {
	//allocate the default command buffer that we will use for rendering
	vk::CommandBufferAllocateInfo info = {};

	info.commandPool = api_data.commandPool;
	info.commandBufferCount = api_data.swapchainImages.size();
	info.level = vk::CommandBufferLevel::ePrimary;

	cmd = api_data.device.allocateCommandBuffers(info);
}

void CommandBuffer_API_data::begin(GraphicsManager_API_data& api_data, vk::Extent2D window) {
	uint n = cmd.size();
	for(uint i=0; i<n; i++) {
		vk::CommandBufferBeginInfo cmdBeginInfo = { };
		cmdBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		cmd[i].begin(cmdBeginInfo);

		//make a clear-color from frame number. This will flash with a 120*pi frame period.
		vk::ClearValue clearValue;

		clearValue.color.float32[0] = .1f;
		clearValue.color.float32[1] = .2f;
		clearValue.color.float32[2] = .8f;
		clearValue.color.float32[3] = 1.f;

		//start the main renderpass.
		//We will use the clear color from above, and the framebuffer of the index the swapchain gave us
		vk::RenderPassBeginInfo rpInfo = { };

		//windowExcent.width = window->getWidth();
		//windowExcent.height = window->getHeight();

		rpInfo.renderPass = api_data.def_renderPass;
		rpInfo.renderArea.offset.x = 0;
		rpInfo.renderArea.offset.y = 0;
		rpInfo.renderArea.extent = window;
		rpInfo.framebuffer = api_data.framebuffers[i];

		//connect clear values
		rpInfo.clearValueCount = 1;
		rpInfo.pClearValues = &clearValue;

		cmd[i].beginRenderPass(&rpInfo, vk::SubpassContents::eInline);
	}
}

void CommandBuffer_API_data::doCommand(SmartPtr<CommandBuffer::Command> command) {
	using namespace vk;

	uint n = cmd.size();

	switch (command->cmd) {
		case CommandBuffer::Command::Draw: {
			CommandBuffer::Command::ArgIterator it = command->arg_chain;
			auto count = *(uint*)(it++).current->getData();
			auto instCount = *(uint*)(it++).current->getData();
			auto vert_begin = *(uint*)(it++).current->getData();
			auto inst_begin = *(uint*)(it++).current->getData();
			for(uint i=0; i<n; i++) {
				cmd[i].draw(count, instCount, vert_begin, inst_begin);
			}
		}break;
		case CommandBuffer::Command::SetShader: {
			for(uint i=0; i<n; i++) {
				cmd[i].bindPipeline(vk::PipelineBindPoint::eGraphics, (*(pShader*)command->arg_chain->getData())->api_data->pipeline);
			}
		}break;

		case CommandBuffer::Command::BindVertexBuffer: {
			auto buf = *(pBuffer*)command->arg_chain->getData();
			vk::Buffer vk_buf = buf->api_data->buffer;
			vk::DeviceSize offsets = 0;
			for(uint i=0; i<n; i++) {
				cmd[i].bindVertexBuffers(0, 1, &vk_buf, &offsets); // @suppress("Ambiguous problem")
			}
		}break;
		default:
		THROW_EXCEPTION("Unknown command occured!");
		break;
	}
}

void CommandBuffer_API_data::end() {
	uint n = cmd.size();
	for(uint i=0; i<n; i++) {
		cmd[i].endRenderPass();
		cmd[i].end();
	}
}

void CommandBuffer_API_data::reset() {
	uint n = cmd.size();
	for(uint i=0; i<n; i++) {
		cmd[i].reset();
	}
}

void CommandBuffer_API_data::cleanup(GraphicsManager_API_data& api_data) {
	api_data.device.free(api_data.commandPool, cmd);
}

NSP_GL_END
