/*
 * Buffer.cpp
 *
 *  Created on: 26 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#include "pch.h"

#include <Rice/GL/Buffer.hpp>

#include "Vulkan_API_code/api_GraphicsManager.hpp"
#include "Vulkan_API_code/api_Buffer_impl.hpp"

NSP_GL_BEGIN

void Buffer::build_api() {
	api_data = RefPtr<Buffer_API_Data> {
		new Buffer_API_Data(get_api_data().device, get_api_data().GPU)
	};
}

void Buffer::allocate(size_t size, BufferUsage usage) {
	api_data->allocate(size, usage);
}

void Buffer::setData(void* pData, size_t nData, size_t offset) {
	api_data->setData(pData, nData, offset);
}

void Buffer::getData(void* pData, size_t nData, size_t offset) {
	api_data->getData(pData, nData, offset);
}

Buffer::~Buffer() { cleanup(); }

void Buffer::cleanup() {
	api_data.release();
}



NSP_GL_END


