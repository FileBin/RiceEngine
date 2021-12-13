#include <GameEngine/Model.h>

namespace Game {

	void Model::SetMaterial(Material& mat, size_t idx) {
		//subMeshes[idx].material = mat;
	}

	size_t Model::GetSubMeshesCount() {
		return subMeshes.size();
	}

	Mesh<Vertex> Model::GetSubMesh(size_t idx) {
		return subMeshes[idx];
	}

	void Model::setSubMeshesCount(size_t count) {
		subMeshes.resize(count);
	}

	void Model::SetSubMesh(Mesh<Vertex>& subMesh, size_t idx) {
		subMeshes[idx] = subMesh;
	}
}
