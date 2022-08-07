#include "stdafx.hpp"

NSP_GL_BEGIN

class Model;

NSP_GL_END

#pragma once

#include "Mesh.hpp"

NSP_GL_BEGIN

class Model {
public:
    bool checkVisiblity(ModelData WVPm, size_t meshIdx);
    uint getSubMeshesCount();
    void setSubMeshesCount(uint count);
    void setSubMesh(ptr<Mesh> subMesh, uint idx);
    ptr<Mesh> getSubMesh(uint idx);

    bool isEmpty();
	bool show = true;

	~Model();
private:
    vec<ptr<Mesh>> subMeshes = {};
};


NSP_GL_END
