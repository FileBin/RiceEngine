/*
 * Buffer.hpp
 *
 *  Created on: 26 April 2022
 *      Author: FileBinsLapTop
 */

#include "stdafx.hpp"

NSP_GL_BEGIN

class Buffer;

enum class BufferUsage {
	Vertex, Index, Uniform
};

enum class IndexBufferType {
	Uint16, UInt32
};

struct Buffer_API_Data;

NSP_GL_END

#include "GraphicsComponentBase.hpp"
#include "GraphicsManager.hpp"

#pragma once

NSP_GL_BEGIN

class Buffer : public GraphicsComponentBase {
protected:
	friend struct CommandBuffer_API_data;
	uptr<Buffer_API_Data> api_data;
	Buffer(ptr<GraphicsManager> g_mgr);
    ~Buffer() override;
public:
	Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage, size_t size);
	Buffer(ptr<GraphicsManager> g_mgr, BufferUsage usage, data_t initialData);

	void cleanup() override;

protected:
	void build_api();
	void allocate(size_t size, BufferUsage usage);
	void setData(void* pSrc, size_t nData, size_t offset = 0);
	void getData(void* pDst, size_t nData, size_t offset = 0);
};

NSP_GL_END


