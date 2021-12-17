#include <GameEngine/Model.h>

namespace Game {

	size_t Model::GetSubMeshesCount() {
		return subMeshes.size();
	}

	Mesh& Model::GetSubMesh(size_t idx){
		return subMeshes[idx];
	}

	void Model::setSubMeshesCount(size_t count) {
		subMeshes.resize(count);
	}

	void Model::SetSubMesh(Mesh& subMesh, size_t idx) {
		subMeshes[idx] = subMesh;
	}
}
