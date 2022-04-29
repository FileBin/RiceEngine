/*
 * VertexBuffer.hpp
 *
 *  Created on: Apr 29, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class VertexBuffer;
typedef SmartPtr<VertexBuffer> pVertexBuffer;

NSP_GL_END

#pragma once

#include "Mesh.hpp"
#include "Buffer.hpp"

NSP_GL_BEGIN

class VertexBuffer : public Buffer {
public:
	VertexBuffer(pGraphicsManager g_mgr, VertexList& initialData);
	void reset(vec<SmartPtr<IVertex>>& initialData);
	void updateVertice(uint index, const IVertex& data);

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
