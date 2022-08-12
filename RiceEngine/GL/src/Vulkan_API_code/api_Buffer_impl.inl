/*
 * api_Buffer_impl.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "Rice/GL/GraphicsManager.hpp"
#include "api_Buffer.hpp"
#include "api_GraphicsManager.hpp"

NSP_GL_BEGIN

Buffer_API_Data::Buffer_API_Data(vk::Device& dev, vk::PhysicalDevice& gpu) : device(dev), GPU(gpu) {}

Buffer_API_Data& Buffer_API_Data::allocate(GraphicsManager_API_data& api_data, size_t size, BufferUsage usage) {
	using namespace vk;
	if(!allocated) {
		buffer_size = size;

		BufferCreateInfo bufferInfo {};
		bufferInfo.size = size;
		switch (usage) {
		case BufferUsage::Index:
			bufferInfo.usage = BufferUsageFlagBits::eIndexBuffer;
			break;
		case BufferUsage::Vertex:
			bufferInfo.usage = BufferUsageFlagBits::eVertexBuffer;
			break;
		case BufferUsage::Uniform:
			bufferInfo.usage = BufferUsageFlagBits::eUniformBuffer;
			break;
		}

		bufferInfo.sharingMode = SharingMode::eExclusive;

		buffer = device.createBuffer(bufferInfo);

		MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);

		MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = api_data.findMemoryType(memReq.memoryTypeBits,
				MemoryPropertyFlagBits::eHostVisible | MemoryPropertyFlagBits::eHostCoherent);

		memory = device.allocateMemory(allocInfo);
		device.bindBufferMemory(buffer, memory, 0);
	}
	allocated = true;

	return *this;
}

Buffer_API_Data& Buffer_API_Data::setData(void* pData, size_t nData, size_t offset) {
    
	void* mappedData;
	mappedData = device.mapMemory(memory, offset, nData);
	    memcpy(mappedData, pData, nData);
	device.unmapMemory(memory);

	return *this;
}

Buffer_API_Data& Buffer_API_Data::getData(void* pData, size_t nData, size_t offset) {
	void* mappedData;
	mappedData = device.mapMemory(memory, offset, nData);
	memcpy(pData, mappedData, nData);
	device.unmapMemory(memory);

	return *this;
}

Buffer_API_Data& Buffer_API_Data::free() {
	if(allocated) {
		device.destroy(buffer);
		device.freeMemory(memory);
	}
	allocated = false;
	return *this;
}

Buffer_API_Data::~Buffer_API_Data() {
	free();
}

NSP_GL_END
