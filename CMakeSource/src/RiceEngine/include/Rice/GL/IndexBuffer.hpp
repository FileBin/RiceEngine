/*
 * IndexBuffer.hpp
 *
 *  Created on: May 14, 2022
 *      Author: filebin
 */

#include "../stdafx.hpp"

typedef uint index_t;
NSP_GL_BEGIN

class IndexBuffer;
typedef RefPtr<IndexBuffer> pIndexBuffer;

NSP_GL_END

#pragma once

#include "Buffer.hpp"

NSP_GL_BEGIN

class IndexBuffer : public Buffer {
private:
	bool is_allocated = false;
	uint current_count = 0;
public:
	IndexBuffer(pGraphicsManager g_mgr, List<index_t> initial_data);
	void reset(List<index_t> initial_data);
	void updateIndices(List<index_t> indices, uint start_pos);
	index_t getIndexData(uint pos);
	List<index_t> getIndexData(uint pos, uint count);

	void cleanup() override;
};

NSP_GL_END
