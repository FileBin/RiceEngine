#include "../stdafx.hpp"

NSP_GL_BEGIN

struct Mesh;
typedef SmartPtr<Mesh> pMesh;

class Model;
typedef SmartPtr<Model> pModel;

NSP_GL_END

#pragma once

#include "../Util.hpp"
#include "../Math.hpp"

NSP_GL_BEGIN

class Model {
public:
	bool CheckVisiblity(ConstantBufferData WVPm, size_t meshIdx);
	size_t GetSubMeshesCount();
	void SetSubMeshesCount(size_t count);
	void SetSubMesh(SmartPtr<Mesh> subMesh, size_t idx);
	SmartPtr<Mesh> GetSubMesh(size_t idx);

	bool IsEmpty() {
		for (auto mesh : subMeshes) {
			if (!mesh->IsEmpty())
			return false;
		}
		return true;
	}

	bool show = true;

	~Model();
private:
	std::vector<SmartPtr<Mesh>> subMeshes = {};
};


NSP_GL_END
