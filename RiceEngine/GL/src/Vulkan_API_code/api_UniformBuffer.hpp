/*
 * api_UniformBuffer.hpp
 *
 *  Created on: 25 июн. 2022 г.
 *      Author: filebin
 */

/*
 * api_Buffer.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#pragma once

#include "VulkanHelper.hpp"
#include <Rice/GL/UniformBuffer.hpp>
#include "api_GraphicsManager.hpp"

NSP_GL_BEGIN

struct UniformBuffer_API_Data {
public:
	vk::Device& device;
	vk::PhysicalDevice& GPU;
	vec<vk::Buffer> buffers;
	vec<vk::DeviceMemory> memories;
	vk::PipelineLayout pip_layout;

	vk::DescriptorPool pool;
	vec<vk::DescriptorSet> sets;

	size_t buffer_size = 0;

	uint buffer_count;

	bool allocated = false;

	UniformBuffer_API_Data(GraphicsManager_API_data& api_data, vk::Device& dev, vk::PhysicalDevice& gpu);

	UniformBuffer_API_Data& allocate(size_t size);
	UniformBuffer_API_Data& free();

	void allocateDescriptorSets(vk::DescriptorSetLayout layout, vk::PipelineLayout pip_layout);
	void setBinding(uint binding, uint size);

	UniformBuffer_API_Data& setData(uint i, void* pSrc, size_t nData, size_t offset);
	UniformBuffer_API_Data& getData(uint i, void* pDst, size_t nData, size_t offset);

	~UniformBuffer_API_Data();

private:
	uint findMemoryType(uint typeFilter, vk::MemoryPropertyFlags properties);
};

NSP_GL_END


