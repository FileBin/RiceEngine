/*
 * api_Buffer.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "Rice/GL/GraphicsManager.hpp"
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

	Buffer_API_Data& allocate(GraphicsManager_API_data& api_data, size_t size, BufferUsage usage);
	Buffer_API_Data& free();

	Buffer_API_Data& setData(void* pSrc, size_t nData, size_t offset);
	Buffer_API_Data& getData(void* pDst, size_t nData, size_t offset);

	~Buffer_API_Data();
};

NSP_GL_END

