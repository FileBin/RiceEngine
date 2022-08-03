/*
 * api_CommandBuffer.hpp
 *
 *  Created on: 27 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#pragma once

#include "VulkanHelper.hpp"
#include "api_GraphicsManager.hpp"

#include <Rice/GL/CommandBuffer.hpp>

NSP_GL_BEGIN

struct CommandBuffer_API_data {
	vec<vk::CommandBuffer> cmd;
    bool begin_pass;

	uint bufCount();

	void build(GraphicsManager_API_data& api_data);

	void begin(GraphicsManager_API_data& api_data, vk::Extent2D window, uint i);
	void end(uint i);
	void reset(uint i);

	void doCommand(ptr<CommandBuffer::Command> command, uint i);

	void cleanup(GraphicsManager_API_data& api_data);
};

NSP_GL_END
