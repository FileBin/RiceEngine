/*
 * VertexBuffer.hpp
 *
 *  Created on: Apr 29, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class VertexBuffer;
typedef RefPtr<VertexBuffer> pVertexBuffer;

NSP_GL_END

#pragma once

#include "Mesh.hpp"
#include "Buffer.hpp"

NSP_GL_BEGIN

class VertexBuffer : public Buffer {
public:
	VertexBuffer(pGraphicsManager g_mgr, VertexList& initialData);
	void reset(VertexList& initialData);

	template<typename Vertex>
	void updateVertice(const Vertex& data, uint index) {
		constexpr uint s = sizeof(Vertex);
		if(s != stride) THROW_EXCEPTION("Incorrect vertex input!");

		setData((void*)&data, s, s*index);
	}
	void updateVertices(VertexList& data, uint start_index);

	template<typename Vertex>
	Vertex getVertexData(uint index) {
		Vertex vert;
		constexpr uint s = sizeof(Vertex);
		if(s != stride) THROW_EXCEPTION("Incorrect vertex type!");

		getData((void*)&vert, s, s * index);
		return vert;
	}

	template<typename Vertex>
	VertexListT<Vertex> getVertexData(uint index, uint count) {
		VertexListT<Vertex> vertices({count});
		uint s = vertices.getStride();
		if(s != stride) THROW_EXCEPTION("Incorrect vertex type!");

		for (uint i = 0; i < count; ++i) {
			getData(vertices.getData(i), s , s*index);
		}
		return vertices;
	}

	bool isAllocated();

	void cleanup() override;
private:

	uint stride;
	uint current_count;
};

NSP_GL_END
