/*
 * Mesh.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"
#include "Rice/GL/IndexBuffer.hpp"
#include <sys/types.h>

NSP_GL_BEGIN

struct Vertex;
typedef RefPtr<Vertex> pVertex;

class Mesh;
typedef RefPtr<Mesh> pMesh;

NSP_GL_END

#pragma once

#include "../Math.hpp"
#include "VertexLayout.hpp"

NSP_GL_BEGIN

struct Vertex {
	Vector3f pos;
	Vector3f norm;
	Vector2f tex_coord0;

	static const VertexLayout vertexLayout;
};

inline const VertexLayout Vertex::vertexLayout(
	{
		VertexInput("POSITION", 0, offsetof(Vertex, pos), VertexInput::float3),
		VertexInput("NORMAL", 0,  offsetof(Vertex, norm), VertexInput::float3),
		VertexInput("TEXCOORD0", 0, offsetof(Vertex, tex_coord0), VertexInput::float2),
	}
);

struct Mesh {
	struct Bounds {
		Vector3f min {}, max {};
		vec<Vector3f> getCorners();
		vec<Vector3f> getPoints();
		Vector3f getSize();
	};
	Bounds bounds;
	RefPtr<VertexList> vertexBuffer;
	vec<index_t> indexBuffer = {};
	static const Mesh quad;

	void rotate(Quaternion q);
	void translate(Vector3 offset);
	void scale(Vector3 scale);
	void combine(Mesh& other);
	void recalculateNormals();
	void reclaculateBounds();

	bool isEmpty() {
		return vertexBuffer->count() == 0;
	}

	//bool CheckVisiblity(ConstantBufferData WVPm);
};

NSP_GL_END
