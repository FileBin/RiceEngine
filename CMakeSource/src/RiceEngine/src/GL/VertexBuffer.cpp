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

	if(!initialData.empty()) {
		uint s = initialData.front()->getStride();
		uint n = initialData.size();

		current_count = n;
		stride = s;

		allocate(s*n, BufferUsage::Vertex);

		for (uint i = 0; i < n; ++i) {
			setData(initialData[i]->getData(), s, s * i);
		}
	}
}

void VertexBuffer::updateVertice(uint i, const IVertex& vert) {
	uint s = vert.getStride();
	if(s != stride) THROW_EXCEPTION("Incorrect vertex input!");

	setData(vert.getData(), s, s*i);
}

NSP_GL_END




