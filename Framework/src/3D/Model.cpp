#include "pch.h"
#include <GameEngine/Model.h>
#include <GameEngine\Math.h>

namespace Game {
	using std::vector;
	const Mesh Mesh::quad = {
		{ {-.5f, -.5f, 0}, {.5f, .5f, 0} },
		{
			{ {-.5f, -.5f, 0 }, { 0,0,1 } },
			{ {.5f, -.5f, 0 }, { 0,0,1 } },
			{ {.5f, .5f, 0 }, { 0,0,1 } },
			{ {-.5f, .5f, 0 }, { 0,0,1 } },
		}, 
		{
			0, 1, 2,
			2, 3, 0 
		},
		Vertex::GetLayout() 
	};
	void Mesh::Rotate(Quaternion q) {
		auto n = vertexBuffer.size();
		for (size_t i = 0; i < n; i++) {
			Vector3 p;
			p = vertexBuffer[i].position;
			p *= q;
			vertexBuffer[i].position = p;
			p = vertexBuffer[i].normal;
			p *= q;
			vertexBuffer[i].normal = p;
		}
	}

	void Mesh::Translate(Vector3 o) {
		Vector3f p;
		p = o;
		auto n = vertexBuffer.size();
		for (size_t i = 0; i < n; i++) {
			vertexBuffer[i].position += p;
		}
	}

	void Mesh::Scale(Vector3 s) {
		Vector3f p;
		p = s;
		auto n = vertexBuffer.size();
		for (size_t i = 0; i < n; i++) {
			vertexBuffer[i].position.x *= p.x;
			vertexBuffer[i].position.y *= p.y;
			vertexBuffer[i].position.z *= p.z;
		}
	}

	void Mesh::Combine(Mesh& other) {
		auto s = vertexBuffer.size();
		vertexBuffer.insert(vertexBuffer.end(), other.vertexBuffer.begin(), other.vertexBuffer.end());
		auto n = indexBuffer.size();
		indexBuffer.insert(indexBuffer.end(), other.indexBuffer.begin(), other.indexBuffer.end());
		auto m = indexBuffer.size();
		for (size_t i = n; i < m; i++) {
			indexBuffer[i] += s;
		}
	}

	void Mesh::RecalculateNormals() {
		auto vbsize = vertexBuffer.size();
		auto ibsize = indexBuffer.size();
		vector<dbl> nCounts(vbsize);
		for (auto idx : indexBuffer) {
			nCounts[idx] += 1;
		}
		for (size_t i = 0; i < vertexBuffer.size(); i++) {
			auto& n = vertexBuffer[i].normal;
			n = { 0,0,0 };
		}
		for (size_t i = 2; i < ibsize; i+=3) {
			Vertex* triangle[3] = {
				&vertexBuffer[indexBuffer[i-2]],
				&vertexBuffer[indexBuffer[i-1]],
				&vertexBuffer[indexBuffer[i]],
			};
			auto v1 = triangle[1]->position - triangle[2]->position;
			auto v2 = triangle[2]->position - triangle[0]->position;
			auto n = Vector3f::Cross(v1, v2).Normalized();
			triangle[0]->normal += n;
			triangle[1]->normal += n;
			triangle[2]->normal += n;
		}
		for (size_t i = 0; i < vertexBuffer.size(); i++) {
			auto& n = vertexBuffer[i].normal;
			n = n.Normalized();
		}
	}

	void Mesh::ReclaculateBounds() {
		if (vertexBuffer.empty()) return;
		bounds.Min = vertexBuffer[0].position;
		bounds.Max = bounds.Min;
		auto n = vertexBuffer.size();
		for (size_t i = 1; i < n; i++) {
			auto& vert = vertexBuffer[i];
			auto& min = bounds.Min;
			auto& max = bounds.Max;
			min.x = Math::Min(min.x, vert.position.x);
			min.y = Math::Min(min.y, vert.position.y);
			min.z = Math::Min(min.z, vert.position.z);

			min.x = Math::Max(max.x, vert.position.x);
			max.y = Math::Max(max.y, vert.position.y);
			max.z = Math::Max(max.z, vert.position.z);
			//max = max.ApplyFunc([&vert](float x, size_t idx) { return Math::Max(x, vert.position[idx]); });
		}
	}

	bool Mesh::CheckVisiblity(ConstantBufferData WVP) {
		auto corners = bounds.GetPoints();
		auto boxSize = bounds.GetSize().SqrLength() * 5;
		WVP.World.c41 = WVP.World.c42 = WVP.World.c43 = 0;
		auto matrix = WVP.World * WVP.View;
		for (size_t i = 0; i < 9; i++) {
			auto proj = corners[i] * matrix;
			if (proj.SqrLength() < boxSize) return true;
			proj = proj * WVP.Projection;
			auto b = proj.z * 1.2f;
			if (proj.x < b && proj.x > -b && proj.y < b && proj.y > -b) { return true; }
		}
		return false;
	}

	bool Model::CheckVisiblity(ConstantBufferData WVP, size_t idx) {
		return subMeshes[idx]->CheckVisiblity(WVP);
	}

	size_t Model::GetSubMeshesCount() {
		return subMeshes.size();
	}

	std::shared_ptr<Mesh> Model::GetSubMesh(size_t idx){
		return subMeshes[idx];
	}

	Model::~Model() {
		pPos = nullptr;
		pRot = nullptr;
		pScale = nullptr;
		subMeshes.clear();
	}

	void Model::SetSubMeshesCount(size_t count) {
		subMeshes.resize(count);
	}

	void Model::SetSubMesh(std::shared_ptr<Mesh> subMesh, size_t idx) {
		subMeshes[idx] = subMesh;
	}

	std::vector<Vector3f> Mesh::Bounds::GetCorners() {
		return {
			Min,
			{ Max.x, Min.y, Min.z },
			{ Min.x, Max.y, Min.z },
			{ Max.x, Max.y, Min.z },
			{ Min.x, Min.y, Max.z },
			{ Max.x, Min.y, Max.z },
			{ Min.x, Max.y, Max.z },
			Max,
		};
	}

	std::vector<Vector3f> Mesh::Bounds::GetPoints() {
		return {
			(Min + Max)*.5f,
			Min,
			{ Max.x, Min.y, Min.z },
			{ Min.x, Max.y, Min.z },
			{ Max.x, Max.y, Min.z },
			{ Min.x, Min.y, Max.z },
			{ Max.x, Min.y, Max.z },
			{ Min.x, Max.y, Max.z },
			Max,
		};
	}

	Vector3f Mesh::Bounds::GetSize() {
		return Max - Min;
	}
}
