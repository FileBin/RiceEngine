#include "../stdafx.hpp"

NSP_GL_BEGIN

struct Mesh;
typedef RefPtr<Mesh> pMesh;

class Model;
typedef RefPtr<Model> pModel;

NSP_GL_END

#pragma once

#include "ModelData.hpp"
#include "Mesh.hpp"

NSP_GL_BEGIN

class Model {
public:
    bool checkVisiblity(ModelData WVPm, size_t meshIdx);
    uint getSubMeshesCount();
    void setSubMeshesCount(uint count);
    void setSubMesh(pMesh subMesh, uint idx);
    pMesh getSubMesh(uint idx);

    bool isEmpty();
	bool show = true;

	~Model();
private:
    vec<pMesh> subMeshes = {};
};


NSP_GL_END
