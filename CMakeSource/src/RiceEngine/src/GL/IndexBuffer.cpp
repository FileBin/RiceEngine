/*
 * IndexBuffer.cpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "pch.h"
#include "Vulkan_API_code/VulkanHelper.hpp"
#include "Vulkan_API_code/api_Buffer.hpp"
#include <Rice/GL/IndexBuffer.hpp>

constexpr auto stride = sizeof(index_t);

NSP_GL_BEGIN

IndexBuffer::IndexBuffer(pGraphicsManager g_mgr, List<index_t> data) : Buffer(g_mgr) {
	reset(data);
}

void IndexBuffer::cleanup() {
	if(is_allocated) {
		api_data->free();
	}
}

void IndexBuffer::reset(List<uint> data) {
	cleanup();
	if(!data.empty()) {
		uint n = data.length();

		current_count = n;

		allocate(n * stride, BufferUsage::Index);
		is_allocated = true;

		setData(data.data_ptr(), n * stride, 0);
	}
}

index_t IndexBuffer::getIndexData(uint pos) {
	index_t ret;
	getData(&ret, stride, pos * stride);
	return ret;
}

List<index_t> IndexBuffer::getIndexData(uint pos, uint count) {
	auto list = List<index_t>(count);
	getData(list.data_ptr(), count * stride, pos * stride);
	return list;
}

void IndexBuffer::updateIndices(List<index_t> indices, uint pos) {
	setData(indices.data_ptr(), indices.length() * stride, pos * stride);
}

NSP_GL_END
