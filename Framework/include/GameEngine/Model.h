#pragma once

#include "Vectors.h"
#include "stdafx.h"
#include "Util.h"
#include "Material.h"

namespace Game {

	using namespace Util;
	struct Mesh {
		struct Bounds {
			Vector3f Min{}, Max{};
			std::vector<Vector3f> GetCorners();
			std::vector<Vector3f> GetPoints();
			Vector3f GetSize();
		};
		Bounds bounds;
		std::vector<Vertex> vertexBuffer = {};
		std::vector<UINT> indexBuffer = {};
		VertexLayout layout = Vertex::GetLayout();

		static const Mesh quad;

		void Rotate(Quaternion q);
		void Translate(Vector3 offset);
		void Scale(Vector3 scale);
		void Combine(Mesh& other);
		void RecalculateNormals();
		void ReclaculateBounds();

		bool IsEmpty() {
			return vertexBuffer.empty(); 
		}

		bool CheckVisiblity(ConstantBufferData WVPm);
	};

	class Model {
	public:
		bool CheckVisiblity(ConstantBufferData WVPm, size_t meshIdx);
		size_t GetSubMeshesCount();
		void SetSubMeshesCount(size_t count);
		void SetSubMesh(std::shared_ptr<Mesh> subMesh, size_t idx);
		std::weak_ptr<Mesh> GetSubMesh(size_t idx);

		bool IsEmpty() { 
			for (auto mesh : subMeshes) {
				if (!mesh->IsEmpty())
					return false;
			}
			return true;
		}

		~Model();

		Vector3* pPos;
		Quaternion* pRot;
		Vector3* pScale;
	private:
		std::vector<std::shared_ptr<Mesh>> subMeshes = {};
	};
}