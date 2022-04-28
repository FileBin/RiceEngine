/*
 * Buffer.hpp
 *
 *  Created on: 26 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class Buffer;
typedef SmartPtr<Buffer> pBuffer;

enum class BufferUsage {
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
private:
	friend class CommandBuffer_API_data;
	pBuffer_API_Data api_data;
public:
	Buffer(pGraphicsManager g_mgr, BufferUsage usage, size_t size) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(size, usage);
	}

	Buffer(pGraphicsManager g_mgr, BufferUsage usage, data_t initialData) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(initialData.size(), usage);
		copyData(initialData.data(), initialData.size());
	}

	template<typename T>
	Buffer(pGraphicsManager g_mgr, BufferUsage usage, vec<T>& initialData) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(sizeof(T) * initialData.size(), usage);
		copyData(initialData.data(), sizeof(T) * initialData.size());
	}

	~Buffer();

	void cleanup() override;

private:
	void build_api();
	void allocate(size_t size, BufferUsage usage);
	void copyData(void* pData, size_t nData);
};

NSP_GL_END


