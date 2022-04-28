/*
 * api_Buffer.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "VulkanHelper.hpp"
#include <Rice/GL/Buffer.hpp>

NSP_GL_BEGIN

struct Buffer_API_Data {
public:
	vk::Device& device;
	vk::PhysicalDevice& GPU;
	vk::Buffer buffer;
	vk::DeviceMemory memory;

	size_t buffer_size = 0;

	bool allocated = false;

	Buffer_API_Data(vk::Device& dev, vk::PhysicalDevice& gpu);

	Buffer_API_Data& allocate(size_t size, BufferUsage usage);

	Buffer_API_Data& copyData(void* pData, size_t nData);

	~Buffer_API_Data();

private:
	uint findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties);
};

NSP_GL_END

