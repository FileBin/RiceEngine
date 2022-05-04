/*
 * Mesh.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

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

struct Vertex : public IVertex {
	struct Data {
		Vector3f pos;
		Vector3f norm;
		Vector2f tex_coord0;
	} data;

	Vertex()
	: data({}) {}

	Vertex(Vector3f pos)
	: data({pos}) {}

	Vertex(Vector3f pos, Vector3f norm)
	: data({pos, norm}) {}

	Vertex(Vector3f pos, Vector3f norm, Vector2f texcoord)
	: data({pos, norm, texcoord}) {}


	static VertexLayout getVertexLayout() {
		return {
			{ "POSITION", 0, offsetof(Data, pos), VertexInput::float3 },
			{ "NORMAL", 0,  offsetof(Data, norm), VertexInput::float3 },
			{ "TEXCOORD0", 0, offsetof(Data, tex_coord0), VertexInput::float2 },
		};
	}

	static uint getVertexStride() {
		return sizeof(Data);
	}

	VertexLayout getLayout() const override { return getVertexLayout(); }
	uint getStride() const override {return getVertexStride(); }
	void* getData() const override {return (void*)&data; }


};

class Mesh {
	struct Bounds {
		Vector3f min {}, max {};
		vec<Vector3f> getCorners();
		vec<Vector3f> getPoints();
		Vector3f getSize();
	};
	Bounds bounds;
	vec<Vertex> vertexBuffer = {};
	vec<uint> indexBuffer = {};
	VertexLayout layout = Vertex::getVertexLayout();

	static const Mesh quad;

	void rotate(Quaternion q);
	void translate(Vector3 offset);
	void scale(Vector3 scale);
	void combine(Mesh& other);
	void recalculateNormals();
	void reclaculateBounds();

	bool isEmpty() {
		return vertexBuffer.empty();
	}

	//bool CheckVisiblity(ConstantBufferData WVPm);
};

NSP_GL_END
