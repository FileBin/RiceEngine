#include <GameEngine/Model.h>

namespace Game {

	const Mesh Mesh::quad = { 
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

	size_t Model::GetSubMeshesCount() {
		return subMeshes.size();
	}

	Mesh& Model::GetSubMesh(size_t idx){
		return *subMeshes[idx];
	}

	Model::~Model() {
		for (auto& m : subMeshes) {
			if (m) delete m;
			m = nullptr;
		}
		subMeshes.clear();
	}

	void Model::SetSubMeshesCount(size_t count) {
		subMeshes.resize(count);
	}

	void Model::SetSubMesh(Mesh* subMesh, size_t idx) {
		subMeshes[idx] = subMesh;
	}
}
