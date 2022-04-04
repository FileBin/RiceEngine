/*
 * VulkanHelper.cpp
 *
 *  Created on: 2 апр. 2022 г.
 *      Author: FileBinsLapTop
 */
#include "pch.h"
#include <Rice/GL/VulkanHelper.hpp>

NSP_GL_BEGIN

vk::CommandPoolCreateInfo VulkanHelper::command_pool_create_info(uint queueFamilyIndex, vk::CommandPoolCreateFlags flags){
	vk::CommandPoolCreateInfo info = {};
	info.sType = vk::StructureType::eCommandPoolCreateInfo;
	info.pNext = nullptr;

	info.queueFamilyIndex = queueFamilyIndex;
	info.flags = flags;
	return info;
}

vk::CommandBufferAllocateInfo VulkanHelper::command_buffer_allocate_info(vk::CommandPool pool, uint count , vk::CommandBufferLevel level){
	vk::CommandBufferAllocateInfo info = {};
	info.sType = vk::StructureType::eCommandBufferAllocateInfo;
	info.pNext = nullptr;

	info.commandPool = pool;
	info.commandBufferCount = count;
	info.level = level;
	return info;
}

NSP_GL_END
