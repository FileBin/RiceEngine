/*
 * VertexLayout.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

class VertexList;

struct VertexInput;
typedef RefPtr<VertexInput> pVertexInput;

typedef vec<VertexInput> VertexLayout;
typedef RefPtr<VertexLayout> pVertexLayout;

NSP_GL_END

NSP_GL_BEGIN

class VertexList {
public:
	virtual uint getStride() const = 0;
	virtual VertexLayout getLayout() const = 0;
	virtual void* getData(uint i) const = 0;
	virtual uint count() const = 0;
	virtual ~VertexList() {}
};

template<typename T>
class VertexListT : public VertexList {
private:
	vec<T> vertices;
public:
	VertexListT(vec<T> init_data) : vertices(init_data) {}

	uint getStride() const override {
		return sizeof(T);
	}
	VertexLayout getLayout() const override {
		return T::vertexLayout;
	}
	void* getData(uint i) const override {
		return (void*)&vertices[i];
	}

	uint count() const override {
		return vertices.size();
	}
};

struct VertexInput {
	const char* description;
	uint binding;
	uint offset;

	enum Format {
		unknown = -1,
		float1 = 0, float2, float3, float4,
		int1, int2, int3, int4,
	} format;

	VertexInput(const char* desc, uint binding, uint offset, Format format) : description(desc), binding(binding), offset(offset), format(format) {}
};

NSP_GL_END

#pragma once
