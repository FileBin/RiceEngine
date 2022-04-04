/*
 * VulkanHelper.hpp
 *
 *  Created on: 2 апр. 2022 г.
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

#pragma once
NSP_GL_BEGIN

class VulkanHelper {
public:

	static vk::CommandPoolCreateInfo command_pool_create_info(
			uint queueFamilyIndex,
			vk::CommandPoolCreateFlags flags);

	static vk::CommandBufferAllocateInfo command_buffer_allocate_info(
			vk::CommandPool pool,
			uint count = 1,
			vk::CommandBufferLevel level = vk::CommandBufferLevel::ePrimary);

};

NSP_GL_END
