/*
 * Mesh.hpp
 *
 *  Created on: Apr 28, 2022
 *      Author: root
 */

#include "../stdafx.hpp"
#include "Rice/GL/IndexBuffer.hpp"
#include "Rice/GL/ModelData.hpp"
#include "Rice/defines.h"

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

struct Mesh {
	struct Bounds {
		Vector3f min {}, max {};
		vec<Vector3f> getCorners();
		vec<Vector3f> getPoints();
		Vector3f getSize();
	};
    Mesh(VertexListT<Vertex> vertices, vec<index_t> indices, bool calcBounds = true);
    Mesh(vec<Vector3f> vertices, vec<index_t> indices);


    pMesh clone() const;

	Bounds bounds;
	VertexListT<Vertex> vertexBuffer;
	vec<index_t> indexBuffer = {};
	static const Mesh quad;

	void rotate(Quaternion q);
	void translate(Vector3 offset);
	void scale(Vector3 scale);
	void combine(const Mesh& other);
	void recalculateNormals();
	void recalculateBounds();

	bool isEmpty() {
		return vertexBuffer.count() == 0;
	}

	bool checkVisiblity(ModelData matrix);
};

NSP_GL_END
