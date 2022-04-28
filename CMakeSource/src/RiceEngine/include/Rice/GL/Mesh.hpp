/*
 * Mesh.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"

NSP_GL_BEGIN

struct Vertex;
typedef SmartPtr<Vertex> pVertex;

class Mesh;
typedef SmartPtr<Mesh> pMesh;

NSP_GL_END

#include "../Math.hpp"
#include "VertexLayout.hpp"

NSP_GL_BEGIN

struct Vertex {

	Vertex(Vector3f pos)
	: pos(pos) {}

	Vertex(Vector3f pos, Vector3f norm)
	: pos(pos),
	  norm(norm) {}

	Vertex(Vector3f pos, Vector3f norm, Vector2f texcoord)
	: pos(pos),
	  norm(norm),
	  tex_coord0(texcoord) {}

	Vector3f pos;
	Vector3f norm;
	Vector2f tex_coord0;

	static VertexLayout getVertexLayout() {
		return {
			{ "POSITION", 0, offsetof(Vertex, pos), VertexInput::float3 },
			{ "NORMAL", 0,  offsetof(Vertex, norm), VertexInput::float3 },
			{ "TEXCOORD0", 0, offsetof(Vertex, tex_coord0), VertexInput::float2 },
		};
	}
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
