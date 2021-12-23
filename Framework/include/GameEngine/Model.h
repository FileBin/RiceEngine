#pragma once

#include "Vectors.h"
#include "stdafx.h"
#include "Util.h"
#include "Material.h"

namespace Game {
	using namespace Util;

	struct Mesh {
		std::vector<Vertex> vertexBuffer = {};
		std::vector<UINT> indexBuffer = {};
		VertexLayout layout = {};

		static const Mesh quad;

		void Rotate(Quaternion q);
		void Translate(Vector3 offset);
		void Scale(Vector3 scale);
		void Combine(Mesh& other);
	};

	class Model {
	public:
		size_t GetSubMeshesCount();
		void SetSubMeshesCount(size_t count);
		void SetSubMesh(Mesh* subMesh, size_t idx);
		Mesh& GetSubMesh(size_t idx);

		Transform transform;
	private:
		std::vector<Mesh*> subMeshes = {};
	};
}