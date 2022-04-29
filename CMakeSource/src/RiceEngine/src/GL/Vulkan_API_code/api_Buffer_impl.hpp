/*
 * api_Buffer_impl.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "api_Buffer.hpp"

NSP_GL_BEGIN

Buffer_API_Data::Buffer_API_Data(vk::Device& dev, vk::PhysicalDevice& gpu) : device(dev), GPU(gpu) {}

Buffer_API_Data& Buffer_API_Data::allocate(size_t size, BufferUsage usage) {
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
		}

		bufferInfo.sharingMode = SharingMode::eExclusive;

		buffer = device.createBuffer(bufferInfo);

		MemoryRequirements memReq = device.getBufferMemoryRequirements(buffer);

		MemoryAllocateInfo allocInfo;
		allocInfo.allocationSize = memReq.size;
		allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits,
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

uint Buffer_API_Data::findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties) {
	using namespace vk;
	PhysicalDeviceMemoryProperties memprop = GPU.getMemoryProperties();

	for (uint i = 0; i < memprop.memoryTypeCount; ++i) {
		if ((typeFilter & (1 << i))	&& (memprop.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	return 0xffffffff;
}

NSP_GL_END
