#include "../stdafx.hpp"

NSP_GL_BEGIN

struct PTR_PROTO(ModelData);

NSP_GL_END

#pragma once

NSP_GL_BEGIN

struct alignas(16) ModelData {
    Matrix4x4f world = Matrix4x4f::identity;
    Matrix4x4f view = Matrix4x4f::identity;
    Matrix4x4f projection = Matrix4x4f::identity;
};

NSP_GL_END
