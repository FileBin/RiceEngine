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
	void updateVertice(const IVertex& data, uint index);
	void updateVertices(VertexList& data, uint start_index);

	template<uint n>
	void updateVertices(std::array<RefPtr<IVertex>, n>& data, size_t start_index){
		uint s = data.front()->getStride();
		if(s != stride) THROW_EXCEPTION("Incorrect vertex input!");

		for (uint i = 0; i < n; ++i) {
			setData(data[i]->getData(), s, s*i + start_index);
		}
	}

	template<typename Vertex = Vertex>
	Vertex getVertexData(uint index) {
		Vertex vert;
		uint s = vert.getStride();
		if(s != stride) THROW_EXCEPTION("Incorrect vertex type!");

		getData(vert.getData(), s, s * index);
		return vert;
	}

	template<typename Vertex = Vertex, uint count>
	std::array<Vertex, count> getVertexData(uint index) {
		std::array<Vertex, count> vertices;
		uint s = vertices.front().getStride();
		if(s != stride) THROW_EXCEPTION("Incorrect vertex type!");

		for (uint i = 0; i < count; ++i) {
			getData(vertices[i].getData(), s , s*index);
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
