/*
 * VertexLayout.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

interface IVertex;

typedef vec<SmartPtr<IVertex>> VertexList;

struct VertexInput;
typedef SmartPtr<VertexInput> pVertexInput;

typedef vec<VertexInput> VertexLayout;
typedef SmartPtr<VertexLayout> pVertexLayout;

NSP_GL_END

NSP_GL_BEGIN

struct VertexInput {
	const char* description;
	uint binding;
	uint offset;

	enum Format {
		unknown = -1,
		float1 = 0, float2, float3, float4,
		int1, int2, int3, int4,
	} format;
};

interface IVertex {
	virtual VertexLayout getLayout() const = 0;
	virtual uint getStride() const = 0;
	virtual void* getData() const = 0;
	virtual ~IVertex() {};
};

NSP_GL_END

#pragma once
