#include "Rice/GL/Mesh.hpp"
#include "Rice/GL/ModelData.hpp"
#include "Rice/GL/VertexLayout.hpp"
#include "Rice/Math/Vectors/Vectors.hpp"
#include "Rice/namespaces.h"
#include "pch.h"

#include <Rice/GL/Model.hpp>
#include <mutex>
#include <shared_mutex>

NSP_GL_BEGIN

using ReadLock = std::shared_lock<std::shared_mutex>;
using WriteLock = std::unique_lock<std::shared_mutex>;

const Mesh Mesh::quad = {
    {{{{-.5f, -.5f, 0}, {0, 0, 1}},
      {{.5f, -.5f, 0}, {0, 0, 1}},
      {{.5f, .5f, 0}, {0, 0, 1}},
      {{-.5f, .5f, 0}, {0, 0, 1}}}}, // vertices
    {0, 1, 2, 2, 3, 0},              // indices
};

Mesh Mesh::cube() {
    auto quad = Mesh::quad;
    quad.translate({0, 0, 0.5});
    auto one_third = quad;
    quad.rotate(Quaternion::fromEulerAngles({0, 180, 0}));
    one_third.combine(quad);
    // Z axis
    Mesh cube = one_third;
    // X axis
    one_third.rotate(Quaternion::fromEulerAngles({0, 90, 0}));
    cube.combine(one_third);
    // Y axis
    one_third.rotate(Quaternion::fromEulerAngles({0, 0, 90}));
    cube.combine(one_third);
    return cube;
};

inline const VertexLayout Vertex::vertexLayout({
    VertexInput("POSITION", 0, offsetof(Vertex, pos), VertexInput::float3),
    VertexInput("NORMAL", 0, offsetof(Vertex, norm), VertexInput::float3),
    VertexInput("TEXCOORD0", 0, offsetof(Vertex, tex_coord0), VertexInput::float2),
});

Mesh::Mesh(VertexListT<Vertex> vertices, vec<index_t> indices, bool calcBounds)
    : vertexBuffer(vertices), indexBuffer(indices) {
    if (calcBounds)
        recalculateBounds();
}

Mesh::Mesh(vec<Vector3f> vertices, vec<index_t> indices)
    : vertexBuffer(vertices), indexBuffer(indices) {
    recalculateNormals();
    recalculateBounds();
}

ptr<Mesh> Mesh::clone() const {
    ptr<Mesh> mesh{new Mesh(vertexBuffer, indexBuffer, false)};
    mesh->bounds = bounds;
    return mesh;
}

void Mesh::translate(Vector3 o) {
    Vector3f p;
    p = Vector3f(o);
    auto n = vertexBuffer.count();
    for (size_t i = 0; i < n; i++) {
        vertexBuffer.getVertex(i).pos += p;
    }
}

void Mesh::rotate(Quaternion q) {
    auto n = vertexBuffer.count();
    for (size_t i = 0; i < n; i++) {
        Vector3 p;
        p = Vector3{vertexBuffer.getVertex(i).pos};
        p *= q;
        vertexBuffer.getVertex(i).pos = Vector3f{p};
        p = Vector3{vertexBuffer.getVertex(i).norm};
        p *= q;
        vertexBuffer.getVertex(i).norm = Vector3f{p};
    }
}

void Mesh::scale(Vector3 s) {
    Vector3f p;
    p = Vector3f{s};
    auto n = vertexBuffer.count();
    for (size_t i = 0; i < n; i++) {
        vertexBuffer.getVertex(i).pos.x *= p.x;
        vertexBuffer.getVertex(i).pos.y *= p.y;
        vertexBuffer.getVertex(i).pos.z *= p.z;
    }
}

void Mesh::combine(const Mesh &other) {
    auto s = vertexBuffer.count();
    vertexBuffer.addVertices(other.vertexBuffer);
    auto n = indexBuffer.size();
    indexBuffer.insert(indexBuffer.end(), other.indexBuffer.begin(), other.indexBuffer.end());
    auto m = indexBuffer.size();
    for (size_t i = n; i < m; i++) {
        indexBuffer[i] += s;
    }
}

void Mesh::recalculateNormals() {
    auto vbsize = vertexBuffer.count();
    auto ibsize = indexBuffer.size();
    vec<dbl> nCounts(vbsize);
    for (auto idx : indexBuffer) {
        nCounts[idx] += 1;
    }
    for (size_t i = 0; i < vertexBuffer.count(); i++) {
        auto &n = vertexBuffer.getVertex(i).norm;
        n = {0, 0, 0};
    }
    for (size_t i = 2; i < ibsize; i += 3) {
        Vertex *triangle[3] = {
            &vertexBuffer.getVertex(indexBuffer[i - 2]),
            &vertexBuffer.getVertex(indexBuffer[i - 1]),
            &vertexBuffer.getVertex(indexBuffer[i]),
        };
        auto v1 = triangle[1]->pos - triangle[2]->pos;
        auto v2 = triangle[2]->pos - triangle[0]->pos;
        auto n = Vector3f::cross(v1, v2);
        n.qNormalize();

        triangle[0]->norm += n;
        triangle[1]->norm += n;
        triangle[2]->norm += n;
    }
    for (size_t i = 0; i < vertexBuffer.count(); i++) {
        auto &n = vertexBuffer.getVertex(i).norm;
        n.qNormalize();
    }
}

void Mesh::recalculateBounds() {
    auto vbsize = vertexBuffer.count();
    if (vbsize <= 0) {
        bounds = {{0, 0, 0}, {0, 0, 0}};
    }
    constexpr dbl max = std::numeric_limits<float>::max();
    bounds.min = {max, max, max};
    bounds.max = {-max, -max, -max};
    for (size_t i = 0; i < vbsize; i++) {
        auto &v = vertexBuffer.getVertex(i);
        bounds.min.x = std::min(bounds.min.x, v.pos.x);
        bounds.min.y = std::min(bounds.min.y, v.pos.y);
        bounds.min.z = std::min(bounds.min.z, v.pos.z);
        bounds.max.x = std::max(bounds.max.x, v.pos.x);
        bounds.max.y = std::max(bounds.max.y, v.pos.y);
        bounds.max.z = std::max(bounds.max.z, v.pos.z);
    }
}

bool Mesh::checkVisiblity(ModelData matrix) {
    auto corners = bounds.getCorners();
    auto boxSize = bounds.getSize().sqrLength() * 5.;
    matrix.world.c41 = matrix.world.c42 = matrix.world.c43 = 0;
    auto temp = matrix.world_view * matrix.world;
    for (size_t i = 0; i < 9; i++) {
        auto proj = corners[i] * temp;
        if (proj.sqrLength() < boxSize)
            return true;
        proj = proj * matrix.projection;
        auto b = proj.z * 1.2f; // correction for perspective
        if (proj.x < b && proj.x > -b && proj.y < b && proj.y > -b) {
            return true;
        }
    }
    return false;
}

bool Model::checkVisiblity(ModelData matrix, size_t idx) {
    return subMeshes[idx]->checkVisiblity(matrix);
}

uint Model::getSubMeshesCount() { return subMeshes.size(); }

ptr<Mesh> Model::getSubMesh(uint idx) { return subMeshes[idx]; }

void Model::setSubMeshesCount(uint count) { subMeshes.resize(count, {}); }

void Model::setSubMesh(ptr<Mesh> subMesh, uint idx) { subMeshes[idx] = subMesh; }

vec<Vector3f> Mesh::Bounds::getCorners() {
    return {
        min,
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {min.x, max.y, max.z},
        max,
    };
}

vec<Vector3f> Mesh::Bounds::getPoints() {
    return {
        (min + max) * .5f, // center
        min,
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {min.x, max.y, max.z},
        max,
    };
}

Vector3f Mesh::Bounds::getSize() { return max - min; }

NSP_GL_END