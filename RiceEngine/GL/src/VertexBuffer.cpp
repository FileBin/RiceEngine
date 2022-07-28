/*
 * VertexBuffer.cpp
 *
 *  Created on: Apr 29, 2022
 *      Author: root
 */

#include "pch.h"

#include <Rice/GL/VertexBuffer.hpp>

#include "Vulkan_API_code/api_Buffer.hpp"

NSP_GL_BEGIN

VertexBuffer::VertexBuffer(pGraphicsManager g_mgr, VertexList& initialData) : Buffer(g_mgr), stride(0), current_count(0) {
	reset(initialData);
}

bool VertexBuffer::isAllocated() {
	return stride != 0;
}

void VertexBuffer::cleanup() {
	if(isAllocated()) {
		api_data->free();
	}
}

void VertexBuffer::reset(VertexList& initialData) {
	cleanup();

	uint s = initialData.getStride();
	uint n = initialData.count();

	if(n != 0) {
		current_count = n;
		stride = s;

		allocate(s*n, BufferUsage::Vertex);

		for (uint i = 0; i < n; ++i) {
			setData(initialData.getData(i), s, s * i);
		}
	}
}

void VertexBuffer::updateVertices(VertexList& vert, uint start) {
	uint s = vert.getStride();
	if(s != stride) THROW_EXCEPTION("Incorrect vertex input!");

	uint n = vert.count();

	for (uint i = 0; i < n; ++i) {
		setData(vert.getData(i), s, s*i + start);
	}
}

NSP_GL_END




