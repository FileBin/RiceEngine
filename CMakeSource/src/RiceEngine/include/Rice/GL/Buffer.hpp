/*
 * Buffer.hpp
 *
 *  Created on: 26 апр. 2022 г.
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class Buffer;
typedef SmartPtr<Buffer> pBuffer;

enum BufferUsage {
	Vertex, Index
};

struct Buffer_API_Data;
typedef SmartPtr<Buffer_API_Data> pBuffer_API_Data;

NSP_GL_END

#include "GraphicsComponentBase.hpp"
#include "GraphicsManager.hpp"

#pragma once

NSP_GL_BEGIN

class Buffer : GraphicsComponentBase {
	pBuffer_API_Data api_data;
public:
	Buffer(pGraphicsManager g_mgr,BufferUsage usage, size_t size) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(size, usage);
	}

	template<typename T>
	Buffer(pGraphicsManager g_mgr, BufferUsage usage, T& objects, size_t objects_count = 1) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(sizeof(T) * objects_count, usage);
		copyData(&objects, sizeof(T) * objects_count);
	}

	~Buffer();

	void cleanup() override;

private:
	void build_api();
	void allocate(size_t size, BufferUsage usage);
	void copyData(void* pData, size_t nData);
};

NSP_GL_END


