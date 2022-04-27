/*
 * Buffer.cpp
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: FileBinsLapTop
 */

#include "pch.h"

#include <Rice/GL/Buffer.hpp>

#include "Vulkan_API_code/api_GraphicsManager.hpp"


using namespace ::Rice::Graphics;

NSP_GL_BEGIN

struct Buffer_API_Data {
public:
	vk::Device& device;
	vk::PhysicalDevice& GPU;
	vk::Buffer buffer;
	vk::DeviceMemory memory;

	size_t buffer_size = 0;

	bool allocated = false;

	Buffer_API_Data(vk::Device& dev, vk::PhysicalDevice& gpu) : device(dev), GPU(gpu) {}

	Buffer_API_Data& allocate(size_t size, BufferUsage usage) {
		using namespace vk;
		if(!allocated) {
			buffer_size = size;

			BufferCreateInfo bufferInfo{};
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
			allocInfo.memoryTypeIndex = findMemoryType(memReq.memoryTypeBits, MemoryPropertyFlagBits::eHostVisible | MemoryPropertyFlagBits::eHostCoherent);

			memory = device.allocateMemory(allocInfo);
			device.bindBufferMemory(buffer, memory, 0);
		}
		allocated = true;

		return *this;
	}

	Buffer_API_Data& copyData(void* pData, size_t nData) {
		void* mappedData;
		mappedData = device.mapMemory(memory, 0, nData);
		    memcpy(pData, pData, nData);
		device.unmapMemory(memory);

		return *this;
	}

	~Buffer_API_Data() {
		if(allocated) {
			device.destroy(buffer);
			device.freeMemory(memory);
		}
	}

private:
	uint findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties) {
		using namespace vk;
		PhysicalDeviceMemoryProperties memprop = GPU.getMemoryProperties();

		for (uint i = 0; i < memprop.memoryTypeCount; ++i) {
			if ((typeFilter & (1 << i))	&& (memprop.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		return 0xffffffff;
	}
};

void Buffer::build_api() {
	api_data = new Buffer_API_Data(get_api_data().device, get_api_data().GPU);
}

Buffer::~Buffer() {
	cleanup();
}

void Buffer::cleanup() {
	api_data.release();
}



NSP_GL_END


