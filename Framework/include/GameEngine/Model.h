#pragma once

#include "Vectors.h"
#include "stdafx.h"
#include "Util.h"
#include "Material.h"

namespace Game {
	using namespace Util;

	template<typename T>
	struct Mesh {
		std::vector<T> vertexBuffer = {};
		std::vector<UINT> indexBuffer = {};
		VertexLayout layout = {};
		//Material material = Material();
	};

	class Model {
	public:
		void SetMaterial(Material &mat, size_t idx);
		size_t GetSubMeshesCount();
		void setSubMeshesCount(size_t count);
		void SetSubMesh(Mesh<Vertex> &subMesh, size_t idx);
		Mesh<Vertex> GetSubMesh(size_t idx);
	private:
		std::vector<Mesh<Vertex>> subMeshes = {};
	};
}