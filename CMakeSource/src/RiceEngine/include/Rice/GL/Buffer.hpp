/*
 * Buffer.hpp
 *
 *  Created on: 26 ���. 2022 �.
 *      Author: FileBinsLapTop
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class Buffer;
typedef RefPtr<Buffer> pBuffer;

enum class BufferUsage {
	Vertex, Index
};

enum class IndexBufferType {
	Uint16, UInt32
};

struct Buffer_API_Data;
typedef RefPtr<Buffer_API_Data> pBuffer_API_Data;

NSP_GL_END

#include "GraphicsComponentBase.hpp"
#include "GraphicsManager.hpp"

#pragma once

NSP_GL_BEGIN

class Buffer : public GraphicsComponentBase {
protected:
	friend struct CommandBuffer_API_data;
	pBuffer_API_Data api_data;
	Buffer(pGraphicsManager g_mgr) : GraphicsComponentBase(g_mgr) { build_api(); }
public:
	Buffer(pGraphicsManager g_mgr, BufferUsage usage, size_t size) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(size, usage);
	}

	Buffer(pGraphicsManager g_mgr, BufferUsage usage, data_t initialData) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(initialData.size(), usage);
		setData(initialData.data(), initialData.size());
	}

	template<typename T>
	Buffer(pGraphicsManager g_mgr, BufferUsage usage, vec<T>& initialData) : GraphicsComponentBase(g_mgr) {
		build_api();
		allocate(sizeof(T) * initialData.size(), usage);
		setData(initialData.data(), sizeof(T) * initialData.size());
	}

	virtual ~Buffer();

	void cleanup() override;

protected:
	void build_api();
	void allocate(size_t size, BufferUsage usage);
	void setData(void* pSrc, size_t nData, size_t offset = 0);
	void getData(void* pDst, size_t nData, size_t offset = 0);
};

NSP_GL_END


