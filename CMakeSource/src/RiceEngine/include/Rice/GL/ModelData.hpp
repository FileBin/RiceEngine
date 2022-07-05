#include "../stdafx.hpp"

NSP_GL_BEGIN

struct ModelData;
typedef RefPtr<ModelData> pModelData;

NSP_GL_END

NSP_GL_BEGIN

struct ModelData {
    Matrix4x4f world = Matrix4x4f::identity, view = Matrix4x4f::identity, projection = Matrix4x4f::identity;
};

NSP_GL_END
